/*
 * File:   hmiMain.c
 * Author: hsaintandre
 *
 * Created on 6 de noviembre de 2018, 12:06
 */


#include <xc.h>
#include "configs.h"
#include "definitions.h"
#include <stdio.h>
#include <string.h>
#include "menu.h"
#include "tft.h"
#include "keyboard.h"
#include "gpio.h"


/* UART variables */
volatile char buff1[15], k1 = 0;
volatile char buff2[15], k2 = 0;
volatile char outBuff2[20], m2 = 0;
volatile char mux = 1;  // multiplex between uart 1 and 2

/* LCD variables */
char lcdText[16];
char menu_shown = 0;
char menu_rowColor = 1;
char menu_update = 0;
char menu_text[16] = "               ";
char decimal_point = 0;

/* Keyboard variables */
const char debounceTime = 25;
char debounceCounter = 0;
char lastKey = 20;
char keyEnable = 1;
char keyConfirm = 0;

/* TIMER variables */
unsigned int divisor = 0;
char printEnable = 0;

/* Global variables */
char globalVar1 = 1;
char g1 = 0;
unsigned int tara = 0;

/* Configuration options */

void putch (char data) {
    if (mux) {
        while(!PIR3bits.TX2IF)
            continue;
        TXREG2 = data;
    } else {
        while(!PIR1bits.TX1IF)
            continue;
        TXREG1 = data;
    }
}

unsigned char rw_eeprom(unsigned char addr, unsigned char data, unsigned char rw){
    
    if (rw) {   //write = 1
        EEADR = addr;
        EEADRH = 0;
        EEDATA = data;  //writes data's value on the eeprom data reg
        EECON1bits.EEPGD = 0;   // data memory access
        EECON1bits.CFGS = 0;    // memory
        EECON1bits.WREN = 1;
        INTCONbits.GIE = 0;
        EECON2 = 0x55;  //?? mandatory
        EECON2 = 0xAA;  //?? mandatory
        EECON1bits.WR = 1;  //initiates write operation 
        INTCONbits.GIE = 1;
        while (!PIR4bits.EEIF); //waits for the end of the write operation
        PIR4bits.EEIF = 0;
        return 0;
    }
    else {      //read = 0
        EEADR = addr;   //sets the address
        EEADRH = 0;
        EECON1bits.EEPGD = 0;   // data memory access
        EECON1bits.CFGS = 0;    // memory
        EECON1bits.RD = 1;  // initiates read operation
        asm("NOP");
        return EEDATA;  //returns read
    }
}

void timer2_config(char pre, char post, char eoc) {
    char option = 0;
    
    option = post - 1;
    option = option << 3;
    T2CON = option;
    T2CON += pre;
    PR2 = eoc;
    PIE1bits.TMR2IE = 1;
    PIR1bits.TMR2IF = 0;
    T2CONbits.TMR2ON = 1;
}

void uart1_config(unsigned long baudRate) {
    unsigned int option16;
    char option8;
    
    baudRate *= 4;
    option16 = _XTAL_FREQ / baudRate;
    option8 = option16 & 0x00FF;
    SPBRG1 = option8;
    option16 = option16 >> 8;
    option8 = option16 & 0x00FF;
    SPBRGH1 = option8;
    
    TXSTA1 = 0b00100100;
    RCSTA1 = 0b10010000;
    BAUDCON1bits.BRG16 = 1;
    
    PIE1bits.TX1IE = 0;
    PIR1bits.TX1IF = 0;
    PIE1bits.RC1IE = 1;
    PIR1bits.RC1IF = 0;
    __delay_ms(1);
}

void uart2_config(unsigned long baudRate) {
    unsigned int option16;
    char option8;
    
    baudRate *= 4;
    option16 = _XTAL_FREQ / baudRate;
    option8 = option16 & 0x00FF;
    SPBRG2 = option8;
    option16 = option16 >> 8;
    option8 = option16 & 0x00FF;
    SPBRGH2 = option8;
    
    TXSTA2 = 0b00100100;
    RCSTA2 = 0b10010000;
    BAUDCON2bits.BRG16 = 1;
    
    PIE3bits.TX2IE = 0;
    PIR3bits.TX2IF = 0;
    PIE3bits.RC2IE = 1;
    PIR3bits.RC2IF = 0;
    __delay_ms(1);
}

/**
 * @brief
 *      This function will print a legend on screen. The key functionality is
 *      to provide the user with relevant information about the weighing like
 *      the tare value and the stability condition
 * @param opt 1: tare; 2: stability
 */
void lcd_legend (uint8_t opt) {
    uint16_t aux = 0;
    
    switch (opt) {
        case 1:
            strcpy(lcdText, "TARE:           ");    // write from 6th position
            aux = tara;
            if (aux >= 10000) {
                lcdText[6] = 48;
                while (aux > 10000) {
                    lcdText[6]++;
                    aux -= 10000;
                }
            }
            lcdText[7] = 48;
            while (aux >= 1000) {
                lcdText[7]++;
                aux -= 1000;
            }
            lcdText[8] = '.';
            lcdText[9] = 48;
            while (aux >= 100) {
                lcdText[9]++;
                aux -= 100;
            }
            lcdText[10] = 48;
            while (aux >= 10) {
                lcdText[10]++;
                aux -= 10;
            }
            lcdText[11] = 48 + aux;
            lcd_text(1, lcdText, 0);
            break;
        case 2:
            break;
        default:
            break;
    }
}

void interrupt isr () {
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;
        
        char aux1 = 0;
        unsigned int aux2 = 0;
        
        if(divisor < 499) {
            divisor++;
        } else {
            divisor = 0;
        }
        
        if (key_trigger()) {
            if (debounceCounter < debounceTime) {
                debounceCounter++;
            } else {
                if (keyEnable) {
                    keyEnable = 0;
                    lastKey = key_pressed();
                }
            }
        } else {
            debounceCounter = 0;
            if (keyEnable == 0) {
                if (lastKey < 58 && lastKey > 47) { // a number
                    menu_text[g1] = lastKey;
                    g1++;
                }
                
                if (lastKey == '*') {
                    for (char i=0;i<15;i++) {
                        menu_text[i] = ' ';
                    }
                    g1 = 0;
                }
                
                if (menu_shown == 0 && lastKey != 'C') {    // clear keys when meno not shown
                    for (char i=0;i<15;i++) {
                        menu_text[i] = ' ';
                    }
                    g1 = 0;
                }
                        
                
                if (lastKey == 'C') {
                    putch(2);
                    printf("W\n");
                }
                
                keyConfirm = menu_next(&menu_shown,&menu_rowColor,lastKey,menu_text);
                /* commands that must be issued according to menus */
                if (keyConfirm == 21) { // record zero value
                    putch(2);
                    printf("CZ\n");
                }
                
                if (keyConfirm == 22) { // save decimal places
                    decimal_point = menu_rowColor - 1;
                    putch(2);
                    printf("CD%d\n", decimal_point);
                }
                
                if (keyConfirm == 23) { // save pattern value
                    putch(2);
                    printf("CP");
                    for (char i=0;i<g1;i++) {
                        putch(menu_text[i]);
                    }
                    putch('\n');
                    for (char i=0;i<15;i++) {   // clear keyboard input buffer
                        menu_text[i] = ' ';
                    }
                    g1 = 0;
                }
                
                if (keyConfirm == 24) { // save filter
                    putch(2);
                    printf("CF%d\n", menu_rowColor);
                }
                
                if (keyConfirm == 25) { // save tare
                    putch(2);
                    printf("CE");
                    for (char i=0;i<g1;i++) {
                        putch(menu_text[i]);
                    }
                    putch('\n');
                    
                    tara = 0;
                    for (char i=0;i<g1;i++) {
                        tara *= 10;
                        tara += menu_text[i] - 48;
                    }
                    aux2 = tara;
                    aux1 = aux2 & 0x00FF;
                    rw_eeprom(0x02,aux1,1);
                    aux2 = aux2 >> 8;
                    aux1 = aux2 & 0x00FF;
                    rw_eeprom(0x01,aux1,1);
                    rw_eeprom(0x00,0xAA,1);
                    
                    for (char i=0;i<15;i++) {   // clear keyboard input buffer
                        menu_text[i] = ' ';
                    }
                    g1 = 0;
                }
                
                if (keyConfirm == 26) { // save threshold
                    putch(2);
                    printf("CT");
                    for (char i=0;i<g1;i++) {
                        putch(menu_text[i]);
                    }
                    putch('\n');
                    
                    for (char i=0;i<15;i++) {   // clear keyboard input buffer
                        menu_text[i] = ' ';
                    }
                    g1 = 0;
                }
                
                if (keyConfirm == 27) { // save range
                    putch(2);
                    printf("CR");
                    for (char i=0;i<g1;i++) {
                        putch(menu_text[i]);
                    }
                    putch('\n');
                    
                    for (char i=0;i<15;i++) {   // clear keyboard input buffer
                        menu_text[i] = ' ';
                    }
                    g1 = 0;
                }
                
                if (menu_shown == 31 && keyConfirm) {
                    putch(2);
                    printf("B%d\n",menu_rowColor);
                }
                
                if ((menu_shown/10) == 3 && keyConfirm) {
                    if (menu_rowColor == 2) {
                        putch(2);
                        printf("BE1\n");
                    }
                    if (menu_rowColor == 3) {
                        putch(2);
                        printf("BE0\n");
                    }
                }
                
                if (menu_shown == 0) {   // this means that setup has finished
                    putch(2);
                    printf("F\n");
                    lcd_legend(1);
                }
                
                keyEnable = 1;
            }
        }
        
        if (menu_shown) {   // since timer2 is started after LCD configuration, there SHOULD be no problem
            printEnable = 0;
        } else {
            printEnable = 1;
        }
    }
    
    if (PIR1bits.RC1IF) {   //UART1
        if (RCSTA1bits.OERR) {
            RCSTA1bits.CREN = 0;
            asm("NOP");
            RCSTA1bits.CREN = 1;
        } else {
            if (RCSTA1bits.FERR) {
                k1 = RCREG1;
                k1 = 0;
                // issue warning command
            } else {
                *(buff1 + k1) = RCREG1;
                if (*(buff1 + k1) != '\n') {
                    k1++;
                }
            }
        }
    }
    
    if (PIR3bits.RC2IF) {   //UART2
        char eoc;
        signed int peso = 0, multiplicador = 10000, peso_cpy = 0;
        unsigned int tara_resta = 0;
        
        if (RCSTA2bits.OERR) {
            RCSTA2bits.CREN = 0;
            asm("NOP");
            RCSTA2bits.CREN = 1;
        } else {
            if (RCSTA2bits.FERR) {
                k2 = RCREG2;
                k2 = 0;
                // issue warning command
            } else {
                *(buff2 + k2) = RCREG2;
                k2++;
            }
        }
       
        
        if (buff2[k2-1] == '\n') {
            switch (buff2[0]) {
                case 'C':
                    break;
                case 'D':
                    switch (buff2[1]) {
                        case 'K':   // this is a confirmation of parameter exchanged successfully
                            menu_navigate(menu_shown,1);
                            break;
                        case ':':
                            decimal_point = buff2[3] - 48;
                            break;
                    }
                    break;
                case 'I':   // information about inputs and outputs
                    switch (buff2[1]) {
                        case '1':   // OUT1
                            if (buff2[2] == 'H')
                                OUT1 = 1;
                            else
                                OUT1 = 0;
                            break;
                        case '2':   // OUT2
                            if (buff2[2] == 'H')
                                OUT2 = 1;
                            else
                                OUT2 = 0;
                            break;
                        case '3':   // OUT3
                            if (buff2[2] == 'H')
                                OUT3 = 1;
                            else
                                OUT3 = 0;
                            break;
                        case '4':   // IN1
                            putch(2);
                            printf("I1");
                            if (IN1)
                                putch('H');
                            else 
                                putch('L');
                            putch('\n');
                            break;
                        case '5':   // IN2
                            putch(2);
                            printf("I2");
                            if (IN2)
                                putch('H');
                            else 
                                putch('L');
                            putch('\n');
                            break;
                        case '6':   // TRIGGER
                            putch(2);
                            printf("I3");
                            if (TRIGGER)
                                putch('H');
                            else 
                                putch('L');
                            putch('\n');
                            break;
                    }
                    break;
                case 'P':
                    //mux = 0;    // switch UART port
                    if (buff2[3] == '-') {  // multiplicador starts at 10,000
                        //putch(buff2[3]);
                        for (char i=0;i<5;i++) {    // assembly of the number starting at the 5th position
                            //putch(buff2[4+i]);
                            eoc = buff2[4+i]-48;
                            for (char j=0;j<eoc;j++) {  // on each iteration multiplicador is added
                                peso += multiplicador;  // as many times as the individual chars
                            }                           
                            multiplicador /= 10;        // decimal shift right
                        }
                        peso = 0 - peso;
                    } else {
                        for (char i=0;i<5;i++) {
                            //putch(buff2[3+i]);
                            eoc = buff2[3+i]-48;
                            for (char j=0;j<eoc;j++) {
                                peso += multiplicador;
                            }
                            multiplicador /= 10;
                        }
                    }
                    peso_cpy = peso;
                    //putch('\n');
                    //mux = 1;
                    if (printEnable) { 
                       printEnable = 0;
                        lcd_point(decimal_point);
                        tara_resta = tara;
                        for (char i=0;i<(3-decimal_point);i++) {
                            tara_resta /= 10;
                        }
                        peso -= tara_resta;   // tare value set manually
                        lcd_number(peso);
                        putch(2);
                        putch('A');
                        putch('\n');
                        printEnable = 1;
                    }
                    
                    
                    if ((peso_cpy == 0) || OUT2) {
                        mux = 0;
                        if (peso >= 0) {
                            putch('+');
                        } else {
                            peso = 0 - peso;
                            putch('-');
                        }
                        printf("%05d\n", peso);
                        mux = 1;
                    }
                    
                    break;
            }
            k2 = 0;
        }
    }
}

void main(void) {
    /* Oscillator configuration */
    OSCCON = 0b01110000;
    OSCTUNE = 0b01000000;
    
    /* PIN configuration */
    TRISA = 0x00;
    TRISB = 0x00;
    TRISC = 0xBF;
    TRISD = 0xB0;
    ANCON0 = 0x00;
    ANCON1 = 0x00;
    
    /* Interrupt configuration */
    INTCON = 0b11000000;
    IPR1 = 0x00;
    IPR2 = 0x00;
    IPR3 = 0x20;    // UART2 has priority since it is the information
    IPR4 = 0x00;
    IPR5 = 0x00;
    RCONbits.IPEN = 1;
    
    /* GPIO initialization */
    gpio_init();
//    gpio_write(0x06);   // turn 24v outs on
    
    /* UART configuration */
    uart1_config(9600);
    uart2_config(9600);
    
    /* LCD configuration */
    lcd_init();
    lcd_set_background(0xFFFF);
    __delay_ms(1);
    printEnable = 1;
    putch(2);
    printf("R\n");  // boots DSP
    
    /* Keyboard configuration */
    keyboard_init();
    
    /* Menu initialization */
    menu_init();
    
    /* EEPROM read */
    if (rw_eeprom(0x00,0,0) == 0xAA) {   // if initialized
        tara = rw_eeprom(0x01,0,0);
        tara = tara << 8;
        tara += rw_eeprom(0x02,0,0);
    }
    lcd_legend(1);
    
    /* Timer 2 configuration */
    timer2_config(PRE16, 10, 100);
    
    while (1);
    return;
}

/*
 * EEPROM mapping
 * 0x00 tare init flag
 * 0x01 tare high
 * 0x02 tare low
 * 0x20 menu parameters init flag
 * 0x21 calibration -> divisions
 * 0x22 calibration -> filter
 * 0x23 serial -> baud
 */