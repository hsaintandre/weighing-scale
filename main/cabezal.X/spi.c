#include <xc.h> // include processor files - each processor file is guarded.  
#include "spi.h"
#include <libpic30.h>
#include <p33FJ128GP802.h>

void spi_init () {
    SPI1STAT = 0;  // disable the SPI module (just in case)
    SPI2STAT = 0;  // disable the SPI module (just in case)
    
    __builtin_write_OSCCONL(0x00);
//    _RP0R = _RPOUT_SCK1OUT;  // SCK
//    _SCK1R = 0; // SCK
//    _RP1R = _RPOUT_SDO1;  // MOSI
//    _SDI1R = 2; // MISO
    
//    _RP0R = 0b01011;  // SCK
//    _SCK2R = 0; // SCK
//    _RP1R = 0b01010;  // MOSI
//    _SDI2R = 2; // MISO
    /* Congif 1*/
//    _RP12R = 0b01011;  // SCK
//    _SCK2R = 12; // SCK
//    _RP13R = 0b01010;  // MOSI
//    _SDI2R = 14; // MISO
    /* Config 2*/
    _RP14R = 0b01011;  // SCK
    _SCK2R = 14; // SCK
    _RP12R = 0b01010;  // MOSI
    _SDI2R = 13; // MISO
    __builtin_write_OSCCONL(0x40);
    
    
    CS = 1; // idle state
    SS = 1;
    
//    _DISSCK = 0;    // internal clock enabled
//    _DISSDO = 0;    // SDO pin controlled by spi module
//    SPI1CON1bits.MODE16 = 0;    // 8 bits wide
//    _SMP = 0;   // data sampled at middle
//    _CKE = 1;   // mode 0
//    _CKP = 0;   // mode 0
//    _SSEN = 0;  // SS pin controlled by port function
//    _MSTEN = 1; // master mode
//    _SPRE = 3;  // prescaler by 5
//    _PPRE = 0;  // postscaler by 64
//    _SPIROV = 0;
//    _SPIEN = 1; // SPI module enabled
    
    SPI2CON1bits.DISSCK = 0;
    SPI2CON1bits.DISSDO = 0;
    SPI2CON1bits.MODE16 = 0;
    SPI2CON1bits.SSEN = 0;
    SPI2CON1bits.SPRE = 2;  // clock divided by 2
    SPI2CON1bits.PPRE = 0;  // clock divided by 64
    SPI2CON1bits.MSTEN = 1;
    SPI2CON1bits.SMP = 0;
    SPI2CON1bits.CKE = 0;
    SPI2CON1bits.CKP = 0;
    SPI2STATbits.SPIROV = 0;
    SPI2STATbits.SPIEN = 1;

}

byte spi (byte data) {
    byte rta;
    
//    _SPI1IF = 0;
//    while(_SPITBF); // wait buffer to be clear
//    rta = SPI1BUF;  // dummy read of the SPI1BUF register to clear the SPIRBF flag
//    SPI1BUF = data; // clear interrupt
//    while(_SPITBF); // wait to the data byte to be moved to shift register
//    while (!_SPI1IF);   // interrupt flag indicating process completed
//    while (!_SPIRBF);   // wait received byte to be moved to readable register
//    rta = SPI1BUF;
//    _SPI1IF = 0;    // clear interrupt
    
    //SPI2STATbits.SPIROV = 0;
    //_SPI2IF = 0;
    rta = SPI2BUF;  // dummy read of the SPI1BUF register to clear the SPIRBF flag
    SPI2BUF = data; // clear interrupt
    while(SPI2STATbits.SPITBF); // wait to the data byte to be moved to shift register
//    while (!_SPI2IF);   // interrupt flag indicating process completed
    while (!SPI2STATbits.SPIRBF);   // wait received byte to be moved to readable register
    rta = SPI2BUF;
    _SPI2IF = 0;    // clear interrupt
    
    return rta;
}

byte spi_ov () {
    byte rta;
    
//    rta = _SPIROV;
    rta = SPI2STATbits.SPIROV;
    
    return rta;
}

void spi_flush () {
    byte drop;
    
//    while (_SPIRBF) {
//        drop = SPI1BUF; // flush receive buffer
//    }
//    if(_SPIROV) {
//        _SPIROV = 0;
//    }
    
    while (SPI2STATbits.SPIRBF) {
        drop = SPI2BUF; // flush receive buffer
    }
    if(SPI2STATbits.SPIROV) {
        SPI2STATbits.SPIROV = 0;
    }
}

void switch_polarity (byte opt) {
    switch (opt) {
        case adc_pol:
            SPI2STAT = 0;  // disable the SPI module (just in case)
            SPI2STATbits.SPIEN = 0;
            SPI2CON1bits.DISSCK = 0;
            SPI2CON1bits.DISSDO = 0;
            SPI2CON1bits.MODE16 = 0;
            SPI2CON1bits.SSEN = 0;
            SPI2CON1bits.SPRE = 2;  // clock divided by 2
            SPI2CON1bits.PPRE = 0;  // clock divided by 64
            SPI2CON1bits.MSTEN = 1;
            SPI2CON1bits.SMP = 0;
            SPI2CON1bits.CKE = 0;
            SPI2CON1bits.CKP = 0;
            SPI2STATbits.SPIROV = 0;
            SPI2STATbits.SPIEN = 1;
            break;
        case eth_pol:
            SPI2STAT = 0;  // disable the SPI module (just in case)
            SPI2STATbits.SPIEN = 0;
            SPI2CON1bits.DISSCK = 0;
            SPI2CON1bits.DISSDO = 0;
            SPI2CON1bits.MODE16 = 0;
            SPI2CON1bits.SSEN = 0;
            SPI2CON1bits.SPRE = 2;  // clock divided by 2
            SPI2CON1bits.PPRE = 0;  // clock divided by 64
            SPI2CON1bits.MSTEN = 1;
            SPI2CON1bits.SMP = 0;
            SPI2CON1bits.CKE = 1;
            SPI2CON1bits.CKP = 0;
            SPI2STATbits.SPIROV = 0;
            SPI2STATbits.SPIEN = 1;
            break;
    }
}