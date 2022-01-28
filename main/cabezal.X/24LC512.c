/*
 * void i2c_init (void);
 * void i2c_start (void);
 * void i2c_r_start (void);
 * void i2c_stop (void);
 * byte i2c_write (byte data_tx);
 * byte i2c_read ();
 * void i2c_ack (byte ack_bit);
 */

#include "24LC512.h"

byte write_eeprom (word address, byte w_data) {
    byte high, low, rta;
    word hold;
    
    hold = address;
    hold = hold & 0x00FF;
    low = hold;
    hold = address;
    hold = hold >> 8;
    hold = hold & 0x00FF;
    high = hold;
    
    i2c_start();
    rta = i2c_write(w_op); 
    if (rta) {
        return 0;
    }
    rta = i2c_write(high);
    if (rta) {
        return 0;
    }
    rta = i2c_write(low);
    if (rta) {
        return 0;
    }
    rta = i2c_write(w_data);
    if (rta) {
        return 0;
    }
    i2c_stop();
    
    do {    // ACK polling
        i2c_start();
        rta = i2c_write(w_op);
    } while (rta);

    return 1;
}

byte read_eeprom (word address, byte *mem_status) {
    byte high, low, rta, rx_data;
    
    low = address & 0x00FF;
    address = address >> 8;
    high = address & 0x00FF;
    *mem_status = 0;    // if mem_status remains in zero, everything went good
    i2c_start();
    rta = i2c_write(w_op);
    if (rta) {
        *mem_status += 0b00000001;  // mem_status code 1
    }
    rta = i2c_write(high);
    if (rta) {
        *mem_status += 0b00000010;  // mem_status code 2
    }
    rta = i2c_write(low);
    if (rta) {
        *mem_status += 0b00000100;  // mem_status code 4
    }
    i2c_start();    // should be repeated start??
    rta = i2c_write(r_op);
    if (rta) {
        *mem_status += 0b00001000;  // mem_status code 8
    }
    rx_data = i2c_read();   // check NACK reception could be added
    i2c_stop();
    
    return rx_data;
}