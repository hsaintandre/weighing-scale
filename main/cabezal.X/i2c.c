#include <xc.h>
#include "i2c.h"

void i2c_init (void) {
    //I2C1CON
    _I2CEN = 1; // I2C enable
    _I2CSIDL = 1;   // continue operation in idle
    _A10M = 0;      // 7 bits slave address
    _DISSLW = 0;    // slew rate disabled (100KHz)
    I2C1BRG = 393;  // Fcy*(1/Fscl - 120ns) - 2
} 

void i2c_start (void) {
    _MI2C1IF = 0;   // clear interrupt flag
    _SEN = 1;   // start condition
    while (!_S);    //waits for the start condition to be completed
    while (!_MI2C1IF);  // wait the 9th clock
    _MI2C1IF = 0;   // clear interrupt flag
}

void i2c_r_start (void) {
    _MI2C1IF = 0;   // clear interrupt flag
    _RSEN = 1;  // repeated start condition
    while (!_MI2C1IF);  // wait the 9th clock
    _MI2C1IF = 0;   // clear interrupt flag
}

void i2c_stop (void) {
    _MI2C1IF = 0;   // clear interrupt flag
    _PEN = 1;   // stop condition
    while (!_MI2C1IF);
    _MI2C1IF = 0;
}

byte i2c_write (byte data_tx) {
    _MI2C1IF = 0;   // clear interrupt flag
    I2C1TRN = data_tx;  // load buffer with transmit data
    while (!_MI2C1IF);  // wait the 9th clock
    _MI2C1IF = 0;   // clear interrupt flag
    return _ACKSTAT;
}

byte i2c_read () {
    _MI2C1IF = 0;   // clear interrupt flag
    _RCEN = 1;  // enable reception
    while (!_MI2C1IF);  // wait the 9th clock
    _MI2C1IF = 0;   // clear interrupt flag
    return I2C1RCV;
}

void i2c_ack (byte ack_bit) {
    switch (ack_bit) {  // set polarity
        case ACK:
            _ACKDT = 0;
            break;
        case NACK:
            _ACKDT = 1;
            break;
    }
    _ACKEN = 1; // issue ACK signal
    while (!_MI2C1IF);
    _MI2C1IF = 0;
}