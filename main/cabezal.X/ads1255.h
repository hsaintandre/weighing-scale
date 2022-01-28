#include "consts.h"
#include <xc.h> // include processor files - each processor file is guarded.  
#include <libpic30.h>
#include <p33FJ128GP802.h>

/* Registers */
#define ads_status  0x00
#define ads_mux     0x01
#define ads_adcon   0x02
#define ads_drate   0x03
#define ads_io      0x04
#define ads_ofc0    0x05
#define ads_ofc1    0x06
#define ads_ofc2    0x07
#define ads_fsc0    0x08
#define ads_fsc1    0x09
#define ads_fsc2    0x0A

/* Commands */
#define ads_wakeup      0x00
#define ads_rdata       0x01
#define ads_rdatac      0x03
#define ads_sdatac      0x0F
#define ads_read        0x10    // must be or'ed
#define ads_write       0x50    // must be or'ed
#define ads_selfcal     0xF0
#define ads_selfocal    0xF1
#define ads_selfgcal    0xF2
#define ads_sysocal     0xF3
#define ads_sysgcal     0xF4
#define ads_sync        0xFC
#define ads_standby     0xFD
#define ads_reset       0xFE

/* Parameters */
#define MUX1    0x01
#define MUX2    0x10
#define PGA1    0
#define PGA2    1
#define PGA4    2
#define PGA8    3
#define PGA16   4
#define PGA32   5
#define PGA64   6
#define SPS1K   0b10100001
#define SPS500  0b10010010
#define SPS100  0b10000010
#define SPS60   0b01110010
#define SPS50   0b01100011
#define SPS30   0b01010011
#define SPS25   0b01000011
#define SPS15   0b00110011
#define SPS10   0b00100011
#define SPS5    0b00010011
#define SPS2    0b00000011

/**
 * @Summary
 *      This function returns the status byte containing the ID and other stuff.
 * @return  Status byte
 */
byte ads_get_status();

/**
 * @Summary
 *      This function returns the state of data ready pin by reading both
 *      the pin and the bit in status register.
 * @return  Whether data is ready or not
 */
byte ads_drdy();

/**
 * @Summary
 *      Performs the configuration of the ADC module.
 * @param mux_param     Signal polarity, MUX1 for standard, MUX2 inverted
 * @param adcon_param   Gain factor
 * @param drate_param   Output data rate
 */
void ads_config(byte mux_param, byte adcon_param, byte drate_param);

/**
 * @Summary
 *      Performs full offset calibration taking into account internal corrections
 *      and the external sensor. The OFC register is updated and the function 
 *      also saves in EEPROM the value to restore it on reboot.
 */
void ads_offset();

/**
 * @Summary
 *      Load offset calibration from memory
 */
void ads_load_offset();

/**
 * @Summary
 *      Reads data from the ADC
 * @return  Data read, by now it returns the 16 noise-free bits
 */
long ads_get_data();

/**
 * @Summary
 *      Perform a synchronization, necessary after writing to PGA, data rate,
 *      buffer status, OFC or FSC
 */
void ads_synchronize();

/**
 * @Summary
 *      This is for debugging purposes, reads a register
 * @param register_name     Register name or number
 * @return  The value of the register
 */
byte ads_read_reg(byte register_name);

/**
 * @Summary
 *      Customizable function taking the measure and returning a float
 * @param adc_measure
 * @return 
 */
float compute_weight (long adc_measure);