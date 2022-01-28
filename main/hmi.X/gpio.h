/* 
 * File:   gpio.h
 * Author: hsaintandre
 *
 * Created on 19 de febrero de 2019, 13:59
 */

#ifndef GPIO_H
#define	GPIO_H

#include <xc.h>
#include "definitions.h"

#define OUT1    LATCbits.LATC4
#define OUT2    LATCbits.LATC5
#define OUT3    LATAbits.LATA6
#define TRIGGER PORTAbits.RA7
#define IN1     PORTDbits.RD4
#define IN2     PORTDbits.RD5

/**
 * @brief
 *  Set corresponding TRIS bits and clears outputs
 */
void gpio_init ();

/**
 * @brief
 *  Read all GPIOs
 * @return
 *  8 bits containing the reads in the following order
 *     <b>OUT1</b>
 *     <b>OUT2</b>
 *     <b>OUT3</b>
 *     <b>TRIGGER</b>
 *     <b>IN1</b>
 *     <b>IN2</b>
 */
uint8_t gpio_read ();

/**
 * @brief
 *  Write the 3 outputs
 * @param out
 *  The out byte in the following order
 *     <b>OUT1</b>
 *     <b>OUT2</b>
 *     <b>OUT3</b>
 */
void gpio_write (uint8_t out);

#endif	/* GPIO_H */