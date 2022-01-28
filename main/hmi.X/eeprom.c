#include <xc.h>
#include "definitions.h"
#include "eeprom.h"

void mem_write (uint8_t address, uint8_t data) {
    EEADR = address;
    EEADRH = 0;
    EEDATA = data;  // writes data's value on the eeprom data reg
    EECON1bits.EEPGD = 0;   // data memory access
    EECON1bits.CFGS = 0;    // memory
    EECON1bits.WREN = 1;
    INTCONbits.GIE = 0;
    EECON2 = 0x55;  // ?? mandatory
    EECON2 = 0xAA;  // ?? mandatory
    EECON1bits.WR = 1;  // initiates write operation 
    INTCONbits.GIE = 1;
    while (!PIR4bits.EEIF); // waits for the end of the write operation
    PIR4bits.EEIF = 0;
}

uint8_t mem_read (uint8_t address) {
    EEADR = address;   // sets the address
    EEADRH = 0;
    EECON1bits.EEPGD = 0;   // data memory access
    EECON1bits.CFGS = 0;    // memory
    EECON1bits.RD = 1;  // initiates read operation
    asm("NOP");
    return EEDATA;  // returns read
}