#include "consts.h"

#define adc_pol 1
#define eth_pol 2

/* Function prototypes */

/**
 * @Summary
 *      SPI hardware initialization
 */
void spi_init ();

/**
  * @Summary
 *      This function exchanges 1 byte of data
 * @param data  Data to be written or dummy in read operation
 * @return      Data read or dummy in case of write operation
 */
byte spi (byte data);

/**
 * @Summary
 *      Overflow check
 * @return  1 if overflow has occurred
 */
byte spi_ov ();

/**
 * @Summary
 *      Flushes the receive buffer to clear overflow
 */
void spi_flush ();

/**
 * @Summary
 *      This function switch between polarities to handle both devices
 * @param opt   adc or eth
 */
void switch_polarity (byte opt);