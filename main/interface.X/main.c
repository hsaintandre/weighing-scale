/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC18F46K80
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/



#include "mcc_generated_files/mcc.h"
#include <stdio.h>
#include <string.h>
#include "display_consts.h"
#include "definitions.h"
#include "tft.h"

char spiDummy, neg = 0;
unsigned char text[6], indice[4];    //display arrays
volatile char buff[15];  //uart reception
volatile char rxBuffer[15];
volatile char uartBuff;
char k = 0, q = 0;
char read_key = 0, key = 0, k_count = 0, key_text[6]; //variables for keyboard
char i2c_received_byte;
char i2c_transmit_byte;
char mensaje[20];
char printEnable = 0;
/*
                         Main application
 */


void putch (char data) {
    while(!PIR3bits.TX2IF)
        continue;
    TXREG2 = data;
}



char get_key(char opt) {

    COL1 = 0;   //everything turns off
    COL2 = 0;
    COL3 = 0;
    COL4 = 0;
    switch (opt) {  //from the interrupt the row is already known
        case 1:
            COL1 = 1;
            __delay_us(10);
            if (ROW1) {
                return 1;
            }
            COL1 = 0;
            COL2 = 1;
            __delay_us(10);
            if (ROW1) {
                return 2;
            }
            COL2 = 0;
            COL3 = 1;
            __delay_us(10);
            if (ROW1) {
                return 3;
            }
            COL3 = 0;
            COL4 = 1;
            __delay_us(10);
            if (ROW1) {
                return 10;
            }
            COL4 = 0;
            break;
        case 2:
            COL1 = 1;
            __delay_us(10);
            if (ROW2) {
                return 4;
            }
            COL1 = 0;
            COL2 = 1;
            __delay_us(10);
            if (ROW2) {
                return 5;
            }
            COL2 = 0;
            COL3 = 1;
            __delay_us(10);
            if (ROW2) {
                return 6;
            }
            COL3 = 0;
            COL4 = 1;
            __delay_us(10);
            if (ROW2) {
                return 11;
            }
            COL4 = 0;
            break;
        case 3:
            COL1 = 1;
            __delay_us(10);
            if (ROW3) {
                return 7;
            }
            COL1 = 0;
            COL2 = 1;
            __delay_us(10);
            if (ROW3) {
                return 8;
            }
            COL2 = 0;
            COL3 = 1;
            __delay_us(10);
            if (ROW3) {
                return 9;
            }
            COL3 = 0;
            COL4 = 1;
            __delay_us(10);
            if (ROW3) {
                return 12;
            }
            COL4 = 0;
            break;
        case 4:
            COL1 = 1;
            __delay_us(10);
            if (ROW4) {
                return 14;
            }
            COL1 = 0;
            COL2 = 1;
            __delay_us(10);
            if (ROW4) {
                return 0;
            }
            COL2 = 0;
            COL3 = 1;
            __delay_us(10);
            if (ROW4) {
                return 15;
            }
            COL3 = 0;
            COL4 = 1;
            __delay_us(10);
            if (ROW4) {
                return 13;
            }
            COL4 = 0;
            break;
    }
    return 20;
}

void save_uart_data (char inData, char index) {
    *(rxBuffer + index) = inData;
}

void uart2_rx_custom_isr () {
        
    if(1 == RCSTA2bits.OERR) {
        RCSTA2bits.CREN = 0;
        RCSTA2bits.CREN = 1;
    }
    if (!RCSTA2bits.FERR) {
        while (PIR3bits.RC2IF) {
            //*(buff + k) = RCREG2;
            uartBuff = RCREG2;
            save_uart_data(uartBuff, k);
            if (uartBuff != '\n') {  // if not end of message, increase index
                k++;
            }
        }
    }

    
}

void tmr2_custom_isr () {
    char local, eoc;
    signed int peso = 0, multiplicador = 10000;
    
    
    if (rxBuffer[k] == '\n') {
        for (char i=0;i<k+1;i++) {
            putch(rxBuffer[i]);
        }
        switch (rxBuffer[0]) {
            case 'P':
                if (rxBuffer[3] == '-') {
                    for (char i=0;i<5;i++) {
                        eoc = rxBuffer[4+i]-48;
                        for (char j=0;j<eoc;j++) {
                            peso += multiplicador;
                        }
                        multiplicador /= 10;
                    }
                    peso = 0 - peso;
                } else {
                    for (char i=0;i<5;i++) {
                        eoc = rxBuffer[3+i]-48;
                        for (char j=0;j<eoc;j++) {
                            peso += multiplicador;
                        }
                        multiplicador /= 10;
                    }
                }
                if (printEnable) {
                    printEnable = 0;
                    lcd_number(peso);
                    printEnable = 1;
                }
                break;
            case 'R':
                
                break;
            case 'C':
                strcpy(mensaje,"CALIBRACION    ");
                lcd_text(1,mensaje);
                strcpy(mensaje,"REQUERIDA      ");
                lcd_text_color(2,mensaje);
                lcd_point(0);
                break;
        }
        k = 0;
        q = 0;
    }

    /*
    if (read_key > 1) { //if an interrupt was generated, the countdown starts
        read_key--; 
    } else if (read_key == 1) { //when it reaches 1, the key is read
        read_key = 0;
        local = get_key(key);
        COL1 = 1;   //since the function exits is made through a return
        COL2 = 1;   //outputs must be turned on outside the function
        COL3 = 1;
        COL4 = 1;
        if (local < 10) {
            //EUSART2_Write(local + 48);
        } else if (local < 19) {
            //EUSART2_Write(local + 55);
        }
        if (local < 19) {
            print_key(local);
        }
    } else {
        if (ROW1 | ROW2 | ROW3 | ROW4) {
            //here the system must wait for the button to be released 
        } else if (!IOC) {
            IOF = 0;    //clear flag bit
            IOC = 1;    //re enables the interrupt
        }
    }
    */
}

void I2C_StatusCallback2(I2C_SLAVE_DRIVER_STATUS i2c_bus_state) {
    switch (i2c_bus_state) {
        case I2C_SLAVE_WRITE_REQUEST:
            printf("Write output\n");
            break;
        case I2C_SLAVE_WRITE_COMPLETED:
            printf("%d", i2c_received_byte);
            break;
        case I2C_SLAVE_READ_REQUEST:
            printf("Write input\n");
            break;
        case I2C_SLAVE_READ_COMPLETED:
            SSPBUF = 0xAA;
            break;
    }
}

void I2C_custom_isr () {
    printf("I2C isr\n");
    
    uint8_t     i2c_data                = 0x55;

    PIR1bits.SSPIF = 0; // clear the slave interrupt flag
    i2c_data = SSPBUF; // read SSPBUF to clear BF

    // check for an I2C upper address transaction
    if ((0 == SSPSTATbits.R_nW) && (0 == SSPSTATbits.D_nA)) {
        
        // callback routine should prepare to receive data from the master
        I2C_StatusCallback2(I2C_SLAVE_WRITE_REQUEST);
        
    } else if (1 == SSPSTATbits.R_nW) {
        
        if ((1 == SSPSTATbits.D_nA) && (1 == SSPCON2bits.ACKSTAT)) {
            // callback routine can perform any post-read processing
            I2C_StatusCallback2(I2C_SLAVE_READ_COMPLETED);
        } else {
            // callback routine should write data into SSPBUF
            I2C_StatusCallback2(I2C_SLAVE_READ_REQUEST);
        }
        
    } else if (1 == SSPSTATbits.D_nA) {
        
        i2c_received_byte = i2c_data;

        // callback routine should process I2C_slaveWriteData from the master
        I2C_StatusCallback2(I2C_SLAVE_WRITE_COMPLETED);
        
    }

    SSPCON1bits.CKP = 1; // release SCL
}

void isr_IOCB4 () { //the number is used as a countdown for de-bouncing
    IOC = 0;    //disable interrupt to service it
    key = 4;
    read_key = 50;
}

void isr_IOCB5 () {
    IOC = 0;    //disable interrupt to service it
    key = 3;
    read_key = 50;
}

void isr_IOCB6 () {
    IOC = 0;    //disable interrupt to service it
    key = 2;
    read_key = 50;
}

void isr_IOCB7 () {
    IOC = 0;    //disable interrupt to service it
    key = 1;
    read_key = 50;
}

void main(void) {
    // Initialize the device
    SYSTEM_Initialize();
    __delay_ms(1);
    /* Custom initialization */
    TMR2_SetInterruptHandler(tmr2_custom_isr);
    EUSART2_SetRxInterruptHandler(uart2_rx_custom_isr);
    /*COL1 = 1;
    COL2 = 1;
    COL3 = 1;
    COL4 = 1;  //outputs set high, inputs must be pulled down
    IOCB4_SetInterruptHandler(isr_IOCB4);
    IOCB5_SetInterruptHandler(isr_IOCB5);
    IOCB6_SetInterruptHandler(isr_IOCB6);
    IOCB7_SetInterruptHandler(isr_IOCB7);
    */

    I2C_SetInterruptHandler(I2C_custom_isr);
    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();
    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    lcd_init();
    lcd_set_background(0xFFFF);
    strcpy(mensaje,"PESO           ");
    lcd_text(1,mensaje);
    lcd_point(1);
    putch(2);
    putch('R');
    putch('\n');
    printEnable = 1;
    while (1);
}