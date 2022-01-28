/**
  MSSP Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    i2c.c

  @Summary
    This is the generated header file for the MSSP driver using 
    PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for I2C.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.65.2
        Device            :  PIC18F46K80
        Driver Version    :  2.01
    The generated drivers are tested against the following:
        Compiler          :  XC8 1.45
        MPLAB 	          :  MPLAB X 4.15	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "i2c.h"

#define I2C_SLAVE_ADDRESS 0x22 
#define I2C_SLAVE_MASK    0x7F

typedef enum 
{
    SLAVE_NORMAL_DATA,
    SLAVE_DATA_ADDRESS,
} SLAVE_WRITE_DATA_TYPE;

/**
 Section: Global Variables
*/

volatile uint8_t    I2C_slaveWriteData      = 0x55;
volatile uint8_t    i2c_addressLow          = 0;
volatile uint8_t    i2c_addressUpper        = 0;
volatile uint8_t    i2c_upperAddressFlag    = true;

/**
 Section: Local Functions
*/
void I2C_StatusCallback(I2C_SLAVE_DRIVER_STATUS i2c_bus_state);


/**
  Prototype:        void I2C_Initialize(void)
  Input:            none
  Output:           none
  Description:      I2C_Initialize is an
                    initialization routine that takes inputs from the GUI.
  Comment:          
  Usage:            I2C_Initialize();

*/
void I2C_Initialize(void)
{
    // initialize the hardware
    // SMP High Speed; CKE disabled; 
    SSPSTAT = 0x00;
    // Configure slave mask
    SSPCON1 = 0x09;
    // MASK 0b01111111; 
    SSPADD = I2C_SLAVE_MASK;
    // SSPEN enabled; WCOL no_collision; CKP disabled; SSPM 7 Bit Polling; SSPOV no_overflow; 
    SSPCON1 = 0x26;
    // ACKEN disabled; GCEN disabled; PEN disabled; ACKDT acknowledge; RSEN disabled; RCEN disabled; ACKSTAT received; SEN disabled;
    SSPCON2 = 0x00;
    // SSPADD 34; 
    SSPADD = I2C_SLAVE_ADDRESS;
    
    
    I2C_SetInterruptHandler(I2C_ISR);
    // clear the slave interrupt flag
    PIR1bits.SSPIF = 0;
    // enable the master interrupt
    PIE1bits.SSPIE = 1;

}

void I2C_ISR ( void )
{
    uint8_t     i2c_data                = 0x55;


    // NOTE: The slave driver will always acknowledge
    //       any address match.

    PIR1bits.SSPIF = 0; // clear the slave interrupt flag
    i2c_data = SSPBUF; // read SSPBUF to clear BF

    // check for an I2C upper address transaction
    if ((0 == SSPSTATbits.R_nW) && (0 == SSPSTATbits.D_nA)) {
        
        // callback routine should prepare to receive data from the master
        I2C_StatusCallback(I2C_SLAVE_WRITE_REQUEST);
        
    } else if (1 == SSPSTATbits.R_nW) {
        
        if ((1 == SSPSTATbits.D_nA) && (1 == SSPCON2bits.ACKSTAT)) {
            // callback routine can perform any post-read processing
            I2C_StatusCallback(I2C_SLAVE_READ_COMPLETED);
        } else {
            // callback routine should write data into SSPBUF
            I2C_StatusCallback(I2C_SLAVE_READ_REQUEST);
        }
        
    } else if (1 == SSPSTATbits.D_nA) {
        
        I2C_slaveWriteData = i2c_data;

        // callback routine should process I2C_slaveWriteData from the master
        I2C_StatusCallback(I2C_SLAVE_WRITE_COMPLETED);
        
    }

    SSPCON1bits.CKP = 1; // release SCL

} // end I2C_ISR()

void I2C_StatusCallback(I2C_SLAVE_DRIVER_STATUS i2c_bus_state)
{
    switch (i2c_bus_state) {
        case I2C_SLAVE_WRITE_REQUEST:
            break;
        case I2C_SLAVE_WRITE_COMPLETED:
            break;
        case I2C_SLAVE_READ_REQUEST:
            break;
        case I2C_SLAVE_READ_COMPLETED:
            break;
        default:;

    } // end switch(i2c_bus_state)
}

void I2C_SetInterruptHandler(void (* interruptHandler)(void)){
    I2C_InterruptHandler = interruptHandler;
}