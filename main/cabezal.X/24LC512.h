#include <xc.h> // include processor files - each processor file is guarded.  
#include <libpic30.h>
#include <p33FJ128GP802.h>
#include "i2c.h"

#define CSB     0x00 // chip select bits
#define addr    (0xA0 + CSB * 2)    // control code + CSB
#define w_op    addr
#define r_op    addr + 1

#ifndef byte
    #define byte unsigned char
#endif

#ifndef word
    #define word unsigned int
#endif




/**
 * @Summary
 *  This function writes a byte on eeprom memory
 * 
 * @param address   Address to write to
 * @param w_data    Data to be written
 * @return          whether successful or not
 */
byte write_eeprom (word address, byte w_data);

/**
 * @Summary
 *  This function reads a byte from eeprom memory
 * 
 * @param address   Address to read from
 * @param mem_status    pointer to the local mem_status register, if zero everything ok
 * @return          data stored at the pointed address
 */
byte read_eeprom (word address, byte *mem_status);