/* 
 * File:   eeprom.h
 * Author: hsaintandre
 *
 * Created on 14 de febrero de 2019, 15:16
 */
#include "definitions.h"

void mem_write (uint8_t address, uint8_t data);
uint8_t mem_read (uint8_t address);