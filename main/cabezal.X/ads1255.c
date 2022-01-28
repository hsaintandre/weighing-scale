#include "ads1255.h"
#include "consts.h"
#include "spi.h"
#include "24LC512.h"


byte ads_get_status() {
    byte rta = 0xAA;
    
    SS = 0;
    __delay_us(1);
    spi(ads_read | ads_adcon);
    spi(0); // read only one byte
    __delay_us(10);
    rta = spi(0);
    __delay_us(2);
    SS = 1;
    
    return rta;
}

byte ads_read_reg(byte register_name) {
    byte rta = 0xAA;
    
    SS = 0;
    __delay_us(1);
    spi(ads_read | register_name);
    spi(0); // read only one byte
    __delay_us(10);
    rta = spi(0);
    __delay_us(2);
    SS = 1;
    
    return rta;
}

byte ads_drdy() {
    byte rta = 0;
    
//    rta = ads_get_status(); // read status register
//    rta = rta & 0x01;  // perform and wise operation to get the less significant bit
    if (DRDY) {
        rta = 0;    // polarity is inverted for a more intuitive use
    } else {
        rta = 1;
    }
    
    return rta;
}

void ads_config(byte mux_param, byte adcon_param, byte drate_param) {
    
    SS = 0;
    __delay_us(1);
    spi(ads_write | ads_status);
    spi(3); // write 4 consecutive registers
    spi(0x00);  // STATUS
    spi(mux_param);  // MUX
    spi(adcon_param);  // ADCON
    spi(drate_param);  // DRATE
    __delay_us(2);
    SS = 1;
    __delay_us(10);
    
    // SELFGCAL
    SS = 0;
    __delay_us(1);
    spi(ads_selfgcal);
    __delay_us(2);
    SS = 1;
    __delay_ms(1000);
    while (!ads_drdy());   // wait for the DRDY line to go low
}

void ads_offset() {
    byte ofc0,ofc1,ofc2;
    
    SS = 0;
    __delay_us(1);
    spi(ads_sysocal);
    __delay_us(2);
    SS = 1;
    __delay_ms(1000);
    while (!ads_drdy());   // wait for the DRDY line to go low
    
    SS = 0;
    __delay_us(1);
    spi(ads_read | ads_ofc0);
    spi(2); // read all the 3 values
    __delay_us(10);
    ofc0 = spi(0);  // low byte
    ofc1 = spi(0);  // mid byte
    ofc2 = spi(0);  // high byte
    __delay_us(2);
    SS = 1;
    
    write_eeprom(0x0203, ofc0);  // save values
    write_eeprom(0x0204, ofc1);
    write_eeprom(0x0205, ofc2);
}

void ads_load_offset() {
    byte mem_status;
    byte ofc0,ofc1,ofc2;
    
    ofc0 = read_eeprom(0x0203, &mem_status);
    ofc1 = read_eeprom(0x0204, &mem_status);
    ofc2 = read_eeprom(0x0205, &mem_status);
    
    SS = 0;
    __delay_us(1);
    spi(ads_write | ads_ofc0);
    spi(2); // write 3 consecutive registers
    spi(ofc0);
    spi(ofc1);
    spi(ofc2);
    __delay_us(2);
    SS = 1;
    __delay_us(10);
}

long ads_get_data() {
    byte rta1,rta2,rta3;
    long conversion;
    
    SS = 0;
    __delay_us(1);
    spi(ads_rdata);
    __delay_us(30);
    rta1 = spi(0);  // high
    __delay_us(30);
    rta2 = spi(0);  // mid
    __delay_us(30);
    rta3 = spi(0);  // low
    __delay_us(30);
    SS = 1;
    __delay_us(10);
    
    conversion = rta1;
    conversion = conversion << 8;
    conversion += rta2;
    conversion = conversion << 8;
    conversion += rta3;
    if (rta1 & 0x80) {  // check if negative
        conversion += 0xFF000000;   // complete most significant byte
    }
    return conversion;
}

void ads_synchronize() {
    // SYNC
    SS = 0;
    __delay_us(1);
    spi(ads_sync);
    __delay_us(2);
    SS = 1;
    __delay_us(10);
    // WAKEUP
    SS = 0;
    __delay_us(1);
    spi(ads_wakeup);
    __delay_us(2);
    SS = 1;
    __delay_us(10);
}