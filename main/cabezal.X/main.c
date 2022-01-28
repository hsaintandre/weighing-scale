/*
 * File:   main.c
 * Author: hsaintandre
 *
 * Created on 4 de mayo de 2018, 10:49
 */


#include <xc.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include "config.h"
#include "consts.h"
#include "funciones.h"
#include "fsm.h"
#include "24LC512.h"
#include "w5100.h"
#include "ads1255.h"
#include "i2c.h"
#include "spi.h"
#include <libpic30.h>
#include <p33FJ128GP802.h>

/* FSM variables */
volatile FSM_STATES status = await;
volatile FSM_STATES_CALIBRATION calibration = not_calibrated;
volatile FSM_STATES_CALIBRATION_OPTION calibration_option = zero;
volatile FSM_STATES_CONFIGURE configuration = serial;
/********************************************/

/* Socket variables */
sock_state global_socket_state;
volatile byte eth_init = 0;
byte ethernet_in_data[100];
char ethernet_out_data[100];
/********************************************/

/* Global variables */
volatile word postscaler = 0, ads_postscaler = 0;    // timer postscaler
volatile byte buff[10], k = 0;   // UART Rx buffer and index
volatile long in_cmd = 0;
volatile byte screen_ready = 0;
volatile byte calibration_flags = 0;

/********************************************/

/* Weight related variables */
volatile long zero_offset, pattern_value, adc_pattern_value;    // load cell values
volatile byte decimal;    // amount of decimal places
volatile byte filterTau;
volatile byte stepSize;
volatile long analog_data;
volatile int printable_analog;
volatile float peso_final = 0;
volatile word stable_range;
volatile long weight_threshold = 0;
volatile long tara = 0;
/****************************/

/* Filter variables */
volatile float x0 = 0, x1 = 0, x2 = 0;
volatile float Y0 = 0, Y1 = 0, Y2 = 0;
volatile float T = 0.04, tau = 0.2;


void putch (byte data) {
    U1TXREG = data;
    while (!_U1TXIF);   // wait transmission complete
    _U1TXIF = 0;
}

void __attribute__((interrupt(auto_psv))) _U1RXInterrupt(void) { //UART1 Rx
    byte inchar, i;

    _U1RXIF = 0; // clear UART1 int flag bit
    if (!U1STAbits.FERR && !U1STAbits.OERR) { //received data has no errors
        while(U1STAbits.URXDA) {    // the interrupt can be due to one or more bytes to read
            inchar = U1RXREG;
            buff[k] = inchar;
            k++;
        }
    } else { //this drops the 5 bytes in the fifo, a loop for keeping them is possible
        U1STAbits.OERR = 0;
    }
    if (buff[k-1] == '\n') {    // message end
        if (buff[0] == 0x02) {  // stream caught at beginning
            switch (buff[1]) {
                case '0':
                    break;
                case '1':
                    break;
                case '2':
                    break;
                case '3':
                    break;
                case '4':
                    break;
                case '5':
                    break;
                case '6':
                    break;
                case '7':
                    break;
                case '8':
                    break;
                case '9':
                    break;
                case 'A':   // screen available to receive weights
                    screen_ready = 1;
                    break;
                case 'B':   // back command
                    break;
                case 'C':
                    status = calibrate;
                    switch (buff[2]) {
                        case 'Z':   // zero
                            calibration_option = zero;
                            break;
                        case 'P':   // pattern
                            calibration_option = pattern;
                            in_cmd = 0;
                            for (i=0;i<(k-4);i++) {
                                in_cmd *= 10;
                                in_cmd += (buff[i+3] - 48);
                            }
                            //in_cmd = (buff[3] - 48) * 10000 + (buff[4] - 48) * 1000 + (buff[5] - 48) * 100 + (buff[6] - 48) * 10 + (buff[7] - 48);
                            break;
                        case 'D':   // divisions
                            calibration_option = divisions;
                            in_cmd = buff[3] - 48;  // could be 0/1/2/3
                            break;
                        case 'F':   // filter selection
                            calibration_option = filter;
                            in_cmd = buff[3] - 48;
                            break;
                        case 'S':   // increase step
                            calibration_option = step;
                            in_cmd = buff[3] - 48;
                            break;
                        case 'T':   // threshold
                            calibration_option = threshold;
                            in_cmd = 0;
                            for (i=0;i<(k-4);i++) {
                                in_cmd *= 10;
                                in_cmd += (buff[i+3] - 48);
                            }
                            break;
                        case 'R':   // range
                            calibration_option = range;
                            in_cmd = 0;
                            for (i=0;i<(k-4);i++) {
                                in_cmd *= 10;
                                in_cmd += (buff[i+3] - 48);
                            }
                            break;
                        case 'E':
                            calibration_option = tare;
                            in_cmd = 0;
                            for (i=0;i<(k-4);i++) {
                                in_cmd *= 10;
                                in_cmd += (buff[i+3] - 48);
                            }
                            break;
                        case 'X':   // enter calibration mode
                            calibration_option = store;
                            break;
                    }
                    break;
                case 'D':
                    break;
                case 'E':   // Ethernet module configuration
                    status = configure;
                    configuration = ethernet;
                    break;
                case 'F':   // setup finished, start weighing
                    screen_ready = 1;
                    status = run;
                    break;
                case 'I':
                    
                    break;
                case 'P':   // change password
                    status = configure;
                    configuration = password;
                    break;
                case 'R': // screen ready
                    screen_ready = 1;
                    status = boot;
                    break;
                case 'S':   // serial port configuration
                    status = configure;
                    configuration = serial;
                    break;
                case 'W':   // when entering the menu
                    status = await;
                    break;
                case 'Z':   // save zero calibration
                    
                    break;
            }
        }
        k = 0;
    }
}

void __attribute__((interrupt(auto_psv))) _U2RXInterrupt(void) { //UART2 Rx
    unsigned int inchar;

    _U2RXIF = 0; //clears UART1 int flag bit
    if (!U2STAbits.FERR && !U2STAbits.OERR) { //received data has no errors
        inchar = U2RXREG;
    } else { //this drops the 5 bytes in the fifo, a loop for keeping them is possible
        U2STAbits.OERR = 0;
    }
}

void __attribute__((interrupt(auto_psv))) _T2Interrupt(void) {    //Timer 2
    _T2IF = 0;      //clears Timer 2 int flag bit
    
    if (postscaler < 9) { // this executes every 10 m sec
        postscaler++;
    } else {
        postscaler = 0;
        
        fsm_keeper(status);   // refresh FSM state
        
        if (global_socket_state == sck_established) {
            switch_polarity(eth_pol);
            __delay_us(10);
            send_keep();    // check Ethernet connection
            switch_polarity(adc_pol);
        }
    }
    
    if (eth_init) {
        switch_polarity(eth_pol);
        __delay_us(10);
        socket(&global_socket_state);
        switch_polarity(adc_pol);
    }
}

void __attribute__((interrupt(auto_psv))) _RTCCInterrupt(void) {    //RTC
    _RTCIF = 0;     //RTC int flag bit
}

void __attribute__((interrupt(auto_psv))) _INT0Interrupt(void) {    //external int 0
    
}

int main(void) {
    
    startup();
    
    while(1);
    return -1;
}

void startup () {
    core_config();
    i2c_init();
    spi_init();
    uart_config(1,BAUD1,ON);
    timer2_config(P1,40000,ON);  //40e6/1/40000 = 1 kHz
}

void core_config (void) {
    /*CORCON: core controller*/
    _US = 0;        //signed multiplication
    _RND = 0;       //convergent rounding
    
    /*INTCON: interrupts*/
    _NSTDIS = 1;    //nested interrupts disabled
    
//    /*OSCCON: oscillator control register*/
//    
//    //_NOSC = 0b011;  //HS w/PLL
//    //_OSWEN = 1;
//    // Xtal = 16MHz, 16e6/2/4*40 = 80MHz
//    /*CLKDIV: clock divisor*/   
//    _DOZE = 0b000;
//    _PLLPOST = 0b00; //PLL output divided by 2
//    _PLLPRE = 0b00010;  //PLL input divided by 4
//    /*PLLFBD: PLL multiplier*/
//    PLLFBD = 0x0026;    //PLL feedback divided by 40 (PLL multiplier) 
    _TUN = 0b000010;    //7.43MHz
    PLLFBD = 41;
    CLKDIVbits.PLLPOST = 0;
    CLKDIVbits.PLLPRE = 0;
    // Initiate Clock Switch to Internal FRC with PLL (NOSC = 0b001)
    __builtin_write_OSCCONH(0x01);
    __builtin_write_OSCCONL(0x01);
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC != 0b001);
    // Wait for PLL to lock
    while(OSCCONbits.LOCK!=1);
    
    /*_INT0EP = 1;    //interrupt on negative edge (external on pin16)
    _INT0IE = 1;
    _INT0IF = 0;
    _INT0IP = 0b101;    //priority 5 (higher than others)*/
    
    AD1PCFGL = 0xFFFF;
    _ADON = 0;
    _AD1MD = 1; // peripheral disabled
    
    /* Config 1 */
//    TRISBbits.TRISB10 = 0;  // SS
//    TRISBbits.TRISB11 = 0;  // RST
//    TRISBbits.TRISB13 = 0;  // MOSI
//    TRISBbits.TRISB14 = 1;  // MISO
//    TRISBbits.TRISB15 = 0;  // CS
    /* Config 2 */
    TRISBbits.TRISB10 = 0;  // RST
    TRISBbits.TRISB11 = 0;  // CS
    TRISBbits.TRISB12 = 0;  // MOSI
    TRISBbits.TRISB13 = 1;  // MISO
    TRISBbits.TRISB15 = 0;  // SS
    
    TRISAbits.TRISA0 = 1;   // DRDY
}

void timer2_config (byte pre, word eoc, byte en) {
    /* Prescaler is 1, 8, 64 and 256*/
    _T2IF = 0;  //interrupt flag
    _T2IE = en;  //interupt enable
    _T2IP = 0b100;  //interrupt priority middle
    T2CONbits.TCKPS = pre & 0x03;    //prescaler
    T2CONbits.TGATE = 0;    //ignored when TCS = 1 tho
    T2CONbits.T32 = 0;  //16 bit timer
    T2CONbits.TCS = 0;   //Internal clock input
    PR2 = eoc;
    T2CONbits.TON = en;
}

void system_boot () {
    byte status, aux1, aux2, calib_count = 0;
    byte mem_read = 0;
    configuration = serial;
    
    mem_read = read_eeprom(0x0000, &status);
    if (mem_read == 0xAA) {  // if initialized, load parameters
        /* UART configuration */
        do {
            aux1 = read_eeprom(0x0003, &status);
        } while (status != 0);
        do {
            aux2 = read_eeprom(0x0004, &status);
        } while (status != 0);
        switch (aux1) {
            case 1:
                uart_config(2,BAUD1,aux2);
                break;
            case 2:
                uart_config(2,BAUD2,aux2);
                break;
            case 3:
                uart_config(2,BAUD3,aux2);
                break;
            case 4:
                uart_config(2,BAUD4,aux2);
                break;
        }
    } else {
        uart_config(2,BAUD1,OFF);
    }
    /****************************************/
        
    /* Load cell parameters */
    ads_config(MUX1,PGA64,SPS25);
    ads_synchronize();
    
    zero_offset = 0;
    pattern_value = 0;
    adc_pattern_value = 0;
    decimal = 1;
    filterTau = 1;
    weight_threshold = 100;
    stable_range = 100;
    tara = 0;
    
    
    mem_read = read_eeprom(0x0005, &status);
    if (mem_read == 0xAA) { // zero flag
        ads_load_offset();
        ads_synchronize();
        do {
            aux1 = read_eeprom(0x0006, &status);    // zero
        } while (status != 0);
        zero_offset = aux1;
        zero_offset = zero_offset << 8;
        do {
            aux1 = read_eeprom(0x0007, &status);    // zero
        } while (status != 0);
        zero_offset += aux1;
        zero_offset = zero_offset << 8;
        do {
            aux1 = read_eeprom(0x0008, &status);    // zero
        } while (status != 0);
        zero_offset += aux1;
        zero_offset = zero_offset << 8;
        do {
            aux1 = read_eeprom(0x0009, &status);    // zero
        } while (status != 0);
        zero_offset += aux1;
        
        calib_count++;
    }
    
    mem_read = read_eeprom(0x000A, &status);
    if (mem_read == 0xAA) { // pattern flag
        do {
            aux1 = read_eeprom(0x000B, &status);    // pattern
        } while (status != 0);
        pattern_value = aux1;
        pattern_value = pattern_value << 8;
        do {
            aux1 = read_eeprom(0x000C, &status);    // pattern
        } while (status != 0);
        pattern_value += aux1;
        pattern_value = pattern_value << 8;
        do {
            aux1 = read_eeprom(0x000D, &status);    // pattern
        } while (status != 0);
        pattern_value += aux1;
        pattern_value = pattern_value << 8;
        do {
            aux1 = read_eeprom(0x000E, &status);    // pattern
        } while (status != 0);
        pattern_value += aux1;
        
        do {
            aux1 = read_eeprom(0x000F, &status);    // ADC pattern value
        } while (status != 0);
        adc_pattern_value = aux1;
        adc_pattern_value = adc_pattern_value << 8;
        do {
            aux1 = read_eeprom(0x0010, &status);    // ADC pattern value
        } while (status != 0);
        adc_pattern_value += aux1;
        adc_pattern_value = adc_pattern_value << 8;
        do {
            aux1 = read_eeprom(0x0011, &status);    // ADC pattern value
        } while (status != 0);
        adc_pattern_value += aux1;
        adc_pattern_value = adc_pattern_value << 8;
        do {
            aux1 = read_eeprom(0x0012, &status);    // ADC pattern value
        } while (status != 0);
        adc_pattern_value += aux1;
        
        calib_count++;
    }
    
    mem_read = read_eeprom(0x00013, &status);
    if (mem_read == 0xAA) { // divisions flag   
        do {
            aux1 = read_eeprom(0x0014, &status);    // divisions
        } while (status != 0);
        decimal = aux1;
        
        printf("D: %d\n", decimal); // print the number of decimal places
        
        calib_count++;
    }
    
    mem_read = read_eeprom(0x00015, &status);
    if (mem_read == 0xAA) { // filter flag
        do {
            aux1 = read_eeprom(0x0016, &status);    // divisions
        } while (status != 0);
        filterTau = aux1;
    }
    
    mem_read = read_eeprom(0x00017, &status);
    if (mem_read == 0xAA) { // threshold flag
        do {
            aux1 = read_eeprom(0x0018, &status);
        } while (status != 0);
        weight_threshold = aux1;
        weight_threshold = weight_threshold << 8;
        do {
            aux1 = read_eeprom(0x0019, &status);
        } while (status != 0);
        weight_threshold += aux1;
        weight_threshold = weight_threshold << 8;
        do {
            aux1 = read_eeprom(0x001A, &status);
        } while (status != 0);
        weight_threshold += aux1;
        weight_threshold = weight_threshold << 8;
        do {
            aux1 = read_eeprom(0x001B, &status);
        } while (status != 0);
        weight_threshold += aux1;
    }
    
    mem_read = read_eeprom(0x0001C, &status);
    if (mem_read == 0xAA) { // range flag
        do {
            aux1 = read_eeprom(0x001D, &status);
        } while (status != 0);
        stable_range = aux1;
        stable_range = stable_range << 8;
        do {
            aux1 = read_eeprom(0x001E, &status);
        } while (status != 0);
        stable_range += aux1;
    }
    
    mem_read = read_eeprom(0x0001F, &status);
    if (mem_read == 0xAA) { // tare flag
        do {
            aux1 = read_eeprom(0x0020, &status);
        } while (status != 0);
        tara = aux1;
        tara = tara << 8;
        do {
            aux1 = read_eeprom(0x0021, &status);
        } while (status != 0);
        tara += aux1;
        tara = tara << 8;
        do {
            aux1 = read_eeprom(0x0022, &status);
        } while (status != 0);
        tara += aux1;
        tara = tara << 8;
        do {
            aux1 = read_eeprom(0x0023, &status);
        } while (status != 0);
        tara += aux1;
    }
    
    gpio(0x01); // set threshold
    gpio(0x02); // set range
    gpio(0x03); // set tare
    
    if (calib_count == 3) { // this means that all if were executed
        calibration = calibrated;
    }
    
    switch_polarity(eth_pol);
    __delay_us(10);
    boot_ethernet();
    eth_init = 1;
    switch_polarity(adc_pol);
}

void uart_config (byte number, unsigned long baud, byte enable) {
    byte drop;
    
    switch (number) {
        case 1:
            U1MODEbits.UARTEN = 0;  // disable before configure
            
            __builtin_write_OSCCONL(0x00);
            //_U1RXR = 11;    // Rx1 mapped to RP11 (p22)
            //_RP10R = 3;     // Tx1 mapped to RP10 (p21)
            /* Config 1 */
            _U1RXR = 2;    // Rx1 mapped to RP2 (p6)
            _RP3R = 3;     // Tx1 mapped to RP3 (p7)
            /* Config 2 */
//            _U1RXR = 3;    // Rx1 mapped to RP2 (p6)
//            _RP2R = 3;     // Tx1 mapped to RP3 (p7)
            __builtin_write_OSCCONL(0x40);
            _U1RXIE = enable;    // enable Rx interrupt
            _U1RXIF = 0; // clears UART1 int flag bit
            U1STAbits.UTXISEL0 = 1;
            //TRISBbits.TRISB11 = 1;
            U1MODEbits.UARTEN = enable;
            U1MODEbits.RTSMD = 1;
            U1MODEbits.BRGH = 1;    // high speed
            U1BRG = FCY / 4 / baud - 1;   // baud rate
            while (U1MODEbits.UARTEN != enable);
            U1STAbits.UTXEN = enable;
            _U1TXIF = 0;
            __delay_ms(1);  // required by the hardware (1 bit time)
            U1STAbits.OERR = 0;
            while (U1STAbits.URXDA) {
                drop = U1RXREG;
            }
            break;
        case 2:
            U2MODEbits.UARTEN = 0;  // disable before configure
            
            __builtin_write_OSCCONL(0x00);
//            _U2RXR = 13;    // Rx1 mapped to RP13 (p24)
//            _RP12R = 5;     // Tx1 mapped to RP12 (p23)
            _U2RXR = 0;    // Rx1 mapped to RP0 (p5)
            _RP1R = 5;     // Tx1 mapped to RP1 (p5)
            __builtin_write_OSCCONL(0x40);
            //_U2RXIE = enable;    // enable Rx interrupt
            _U2RXIF = 0; // clears UART1 int flag bit
            TRISBbits.TRISB13 = 1;
            U2MODEbits.UARTEN = enable;
            U2MODEbits.RTSMD = 1;
            U2MODEbits.BRGH = 1;    // high speed
            U2BRG = FCY / (4 * baud) - 1;   // baud rate
            while (U2MODEbits.UARTEN != enable);
            U2STAbits.UTXEN = enable;
            _U2TXIF = 0;
            __delay_ms(1);  // required by the hardware (1 bit time)
            U2STAbits.OERR = 0;
            while (U2STAbits.URXDA) {
                drop = U2RXREG;
            }
            break;
    }
}

void rtc_config (byte year, byte month, byte day, byte hour, byte minute) {
    
    //_T1IF interrupt flag
    //_T1IE interrupt enable
    //_T1IP interrupt priority (3b = 100)
    T1CONbits.TCKPS = 0b00;    //prescaler = 1
    T1CONbits.TSYNC = 0; //Not synced with internal oscillator
    T1CONbits.TCS = 1;   //External clock input
    //_RTCIF RTC interrupt flag
    //_RTCIE RTC interrupt enable
    //_RTCIP RTC interrupt priority (3b = 100)
    _RTCPTR = 0b11;
    NVMKEY = 0x55;  //unlock sequence
    NVMKEY = 0xAA;
    _RTCWREN = 1;   //enable register write operation
    _RTCEN = 0; //disable module to avoid conflicts on writing
    RTCVAL = year;
    RTCVAL = concatenate(&month,&day);
    month = 0;
    RTCVAL = concatenate(&month,&hour);
    RTCVAL = concatenate(&minute,&month);
    _RTCEN = 1; //enable module
    _RTCWREN = 0;   //disable register write operation
}

float compute_weight (long adc_measure) {
    float rta, true_pattern;
    byte i = 0;
    
    true_pattern = pattern_value;
    true_pattern /= 1000;
    x0 = adc_measure - zero_offset; // remove offset
    x0 = x0 * true_pattern / (adc_pattern_value - zero_offset);
    //y0 = (x0 + 2*x1 + x2 - (2-8*(tau^2)/(T^2))*y1 - (1-4*tau/T+4*(tau^2)/(T^2))*y2 )/(1+4*tau/T+4*(tau^2)/(T^2));
    //Y0 = (x0 + 2*x1 + x2 + 6*Y1 - 1*Y2) / 9; // Fs = 2.5, tau = 0.4
    
    /*
     * During calibration, when calculating the slope with the pattern
     * the constant at the end of the filter formula can be a variable
     * and set to 1, then the ratio between the true value and
     * the result of the filter can be used to accurately compute the constant
     */
    
    switch (filterTau) {
        case 1:
            Y0 = (x0 + 2*x1 + x2 + 22.5*Y1 - 6.25*Y2) * 0.04938271605;  // Fs = 25, tau = 0.07 C=0.049382716049383
            break;
        case 2:
            Y0 = (x0 + 2*x1 + x2 + 110.5*Y1 - 42.25*Y2) * 0.01384083;  // Fs = 25, tau = 0.15
            break;
        case 3:
            Y0 = (x0 + 2*x1 + x2 + 448*Y1 - 196*Y2) * 0.00390625;  // Fs = 25, tau = 0.3
            break;
    }
    
    x2 = x1;
    x1 = x0;
    Y2 = Y1;
    Y1 = Y0;
    
    rta = Y0; 
    for (i=0;i<decimal;i++) {
        rta *= 10;
    }
    
    return rta;
}

void fsm_keeper (FSM_STATES estado) {
    float round_ceil = 0, round_floor = 0;
    static byte last_rdy = 1;
    
    switch (estado) {
        case boot:  // load all parameters
            fsm_boot();
            break;
        case calibrate:
            fsm_calibrate();
            break;
        case configure:
            fsm_configure();
            break;
        case run:
            if (ads_drdy()) {   // check data ready pin
                if (last_rdy) { // check toggle to avoid re-reading
                    analog_data = ads_get_data();
                    peso_final = compute_weight(analog_data);
                    last_rdy = 0;    
                }
            } else {
                last_rdy = 1;
            }
            if (ads_postscaler < 19) {
                ads_postscaler++;
            } else if (screen_ready) {
                printable_analog = peso_final;
                gpio(0x00);
                ads_postscaler = 0;
                round_floor = printable_analog;
                round_ceil = round_floor + 1;
                if ((round_ceil - peso_final) < (peso_final - round_floor)) {
                    printable_analog++;
                }
                if (printable_analog < 0) {
                    printable_analog = 0 - printable_analog;
                    printf("P: -%05d\n", printable_analog); // change this for N: or P: depending on whether the sign be negative or positive
                } else {
                    printf("P: %05d\n", printable_analog);
                }
                screen_ready = 0;
            }
            break;
        case await:
            break;
    }
}

void fsm_boot () {
    system_boot();
    if (calibration == not_calibrated) {
        status = calibrate;
        printf("C\n");
        calibration_option = dummy;
    } else {
        status = run;
    }
}

void fsm_calibrate () {
    byte i = 0;
    long samples = 0;
    
    switch (calibration_option) {
        case zero:      // set the zero offset
            ads_offset();
            ads_synchronize();
            for (i=0;i<128;i++) {
                while (ads_drdy() == 0);    // wait for the ads to finish conversion
                samples += ads_get_data();
            }
            samples /= 128;
            zero_offset = samples;
            save_calibration(1);
            
            printf("DK\n");
            calibration_option = dummy;
            break;
        case pattern:   // set the pattern weight (relative to divisions)
            for (i=0;i<128;i++) {
                while (ads_drdy() == 0);    // wait for the ads to finish conversion
                samples += ads_get_data();
            }
            samples /= 128;
            adc_pattern_value = samples;
            pattern_value = in_cmd;
            save_calibration(2);
            
            printf("DK\n");
            calibration_option = dummy;
            break;
        case divisions: // set the number of divisions
            decimal = in_cmd;
            save_calibration(3);
            gpio(0x01);
            gpio(0x02);
            gpio(0x03);
            
            printf("DK\n");
            calibration_option = dummy;
            break;
        case filter:    // set filter's cutoff frequency
            // yet not implemented
            filterTau = in_cmd;
            save_calibration(4);
            
            printf("DK\n");
            calibration_option = dummy;
            break;
        case step:      // set the increase step
            // not implemented yet
            save_calibration(5);
            
            printf("DK\n");
            calibration_option = dummy;
            break;
        case store:
            printf("DK\n");
            calibration_option = dummy;
            break;
        case threshold:
            weight_threshold = in_cmd;
            save_calibration(6);
            gpio(0x01);
            
            printf("DK\n");
            calibration_option = dummy;
            break;
        case range:
            stable_range = in_cmd;
            save_calibration(7);
            gpio(0x02);
            
            printf("DK\n");
            calibration_option = dummy;
            break;
        case tare:
            tara = in_cmd;
            save_calibration(8);
            gpio(0x03);
            
            printf("DK\n");
            calibration_option = dummy;
            break;
        case dummy:
            break;
    }
}

void fsm_configure () {
    unsigned long conf_baud = 0;
    
    switch (configuration) {
        case serial:
            buff[2] -= 48;  // convert to integer number of serial port
            buff[6] -= 48;  // convert to integer enable/disable flag
            switch (buff[4]) {
                case '1':
                    conf_baud = BAUD1;
                    break;
                case '2':
                    conf_baud = BAUD2;
                    break;
                case '3':
                    conf_baud = BAUD3;
                    break;
                case '4':
                    conf_baud = BAUD4;
                    break;
            }
            uart_config(buff[2],conf_baud,buff[6]);
            status = run;
            break;
        case ethernet:
            break;
        case password:
            break;
    }
}

void fsm_event_handler () {
    
}

void save_calibration (byte option) {
    byte auxByte = 0;
    long auxLong = 0;
    word auxInt = 0;
    
    switch (option) {
        case 1: // zero
            auxLong = zero_offset;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x0009, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x0008, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x0007, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x0006, auxByte);
            write_eeprom(0x0005, 0xAA);
            break;
        case 2: // pattern
            auxLong = pattern_value;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x000E, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x000D, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x000C, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x000B, auxByte);

            auxLong = adc_pattern_value;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x0012, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x0011, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x0010, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x000F, auxByte);
            write_eeprom(0x000A, 0xAA);
            break;
        case 3: // divisions
            write_eeprom(0x0014, decimal);
            write_eeprom(0x0013, 0xAA);
            break;
        case 4: // filter
            write_eeprom(0x0016, filterTau);
            write_eeprom(0x0015, 0xAA);
            break;
        case 5: // step
            write_eeprom(0x00, stepSize);
            break;
        case 6: // threshold
            auxLong = weight_threshold;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x001B, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x001A, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x0019, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x0018, auxByte);
            write_eeprom(0x0017, 0xAA);
            break;
        case 7: // range
            auxInt = stable_range;
            auxByte = auxInt & 0x00FF;
            write_eeprom(0x001E, auxByte);
            auxInt = auxInt >> 8;
            auxByte = auxInt & 0x00FF;
            write_eeprom(0x001D, auxByte);
            write_eeprom(0x001C, 0xAA);
            break;
        case 8: // tare
            auxLong = tara;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x0023, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x0022, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x0021, auxByte);
            auxLong = auxLong >> 8;
            auxByte = auxLong & 0x000000FF;
            write_eeprom(0x0020, auxByte);
            write_eeprom(0x001F, 0xAA);
            break;
    }
}

void save_configuration () {

}

void gpio (byte opt) {
    //static float last_weight;
    static int last_weight;
    //float aux_float;
    int aux_int, aux_int2;
    static byte std_count;
    byte i = 0;
    static long weight_threshold_comp;
    static word std_range;
    static long tare_value;
    
    switch (opt) {
        case 0x00:  // normal operation option
            aux_int2 = printable_analog - tare_value;
            if (aux_int2 > weight_threshold_comp) {
                printf("I1H\n");
                if (printable_analog > last_weight) {
                    aux_int = printable_analog - last_weight;
                }
                else {
                    aux_int = last_weight - printable_analog;
                }
                if (aux_int < std_range) {
                    std_count++;
                    if (std_count > 9) {
                        std_count = 9;
                        printf("I2H\n");
                    }
                }
                else {
                    std_count = 0;
                    printf("I2L\n");
                }
            } else {
                printf("I1L\n");
                std_count = 0;
                printf("I2L\n");
            }
            last_weight = printable_analog;
            break;
        case 0x01:  // compute threshold according to decimal places selected
            weight_threshold_comp = weight_threshold;
            for (i=0;i<(3-decimal);i++) {
                weight_threshold_comp /= 10;
            }
            break;
        case 0x02:  // compute stabilization range
            std_range = stable_range;
            for (i=0;i<(3-decimal);i++) {
                std_range /= 10;
            }
            break;
        case 0x03:  // compute tare
            tare_value = tara;
            for (i=0;i<(3-decimal);i++) {
                tare_value /= 10;
            }
            break;
    }
}

void socket (sock_state *socket_status) {
    byte aux1=0xFF;
    word t2_i = 0, total_data_rx;
    
    aux1 = get_status();
    switch (aux1) {
        case 0x00:  // socket closed, open it
            *socket_status = sck_open;
            socket_fsm(socket_status);
            break;
        case 0x13:  // socket open, start listening
            *socket_status = sck_listen;
            socket_fsm(socket_status);
            break;
        case 0x14:  // socket listening

            break;
        case 0x17:  // connection with a peer established
            if (*socket_status != sck_data_ready) { // avoid changing status before reading
                if (*socket_status != sck_post_send || *socket_status != sck_close) {
                    *socket_status = sck_established; // this status also means able to communicate
                }
                socket_fsm(socket_status);
            }
            break;
        case 0x1C:  // connection termination received
            *socket_status = sck_close;
            socket_fsm(socket_status);
            break;
    }
    if (global_socket_state == sck_data_ready) {
        t2_i = 0;
        total_data_rx = get_eth_data(ethernet_in_data);
        if (ethernet_in_data[1] == 'P') {
            strcpy(ethernet_out_data, "Ethernet working!\n");
            send_eth_data(ethernet_out_data, 18);
        }
        global_socket_state = sck_established;
    }
}

/*         _________
          |°        |
    MCLR -|1      28|- AVdd
         -|2      27|- AVss
         -|3      26|- CS
         -|4      25|- MISO
         -|5      24|- MOSI
    R1x  -|6      23|- SCK
    T1x  -|7      22|- RST
    Vss  -|8      21|- SS
    OSC1 -|9      20|- Vcap
    OSC2 -|10     19|- Vss
         -|11     18|- SDA
         -|12     17|- SCL
    Vdd  -|13     16|- DR
         -|14     15|- SCK
          |_________|
 
 * EEPROM mapping
 * 0x0000   UARTS initialization flag 0xAA
 * 0x0001   UART1 baud rate 1/2/3/4
 * 0x0002   UART1 enable / disable
 * 0x0003   UART2 baud rate 1/2/3/4
 * 0x0004   UART2 enable / disable
 * 0x0005   zero saved = 0xAA
 * 0x0006   zero high byte
 * 0x0007   zero mid high byte
 * 0x0008   zero mid low byte
 * 0x0009   zero low byte
 * 0x000A   pattern saved   = 0xAA
 * 0x000B   pattern high byte
 * 0x000C   pattern mid high byte
 * 0x000D   pattern mid low byte
 * 0x000E   pattern low byte
 * 0x000F   weight high
 * 0x0010   weight mid high
 * 0X0011   weight mid low
 * 0x0012   weight low
 * 0x0013   divisions flag = 0xAA
 * 0x0014   divisions configuration 0/1/2/3
 * 0x0015   filter flag = 0xAA
 * 0x0016   filter selection
 * 0x0017   threshold flag = 0xAA
 * 0x0018   threshold high
 * 0x0019   threshold
 * 0x001A   threshold
 * 0x001B   threshold low
 * 0x001C   range flag = 0xAA
 * 0x001D   range high
 * 0x001E   range low
 * 0x001F   tare flag = 0xAA
 * 0x0020   tare high
 * 0x0021   tare
 * 0x0022   tare
 * 0x0023   tare low
 * ...
 * 0x0100   gateway[3]
 * 0x0101   gateway[2]
 * 0x0102   gateway[1]
 * 0x0103   gateway[0]
 * 0x0104   subnet[3]
 * 0x0105   subnet[2]
 * 0x0106   subnet[1]
 * 0x0107   subnet[0]
 * 0x0108   MAC[5]
 * 0x0109   MAC[4]
 * 0x010A   MAC[3]
 * 0x010B   MAC[2]
 * 0x010C   MAC[1]
 * 0x010D   MAC[0]
 * 0x010E   IP[3]
 * 0x010F   IP[2]
 * 0x0110   IP[1]
 * 0x0111   IP[0]
 * 0x0112   port[1]
 * 0x0113   port[0]
 * ...
 * 0x0200   MUX
 * 0x0201   PGA
 * 0x0202   SPS
 * 0x0203   OFC0
 * 0x0204   OFC1
 * 0x0205   OFC2
 */