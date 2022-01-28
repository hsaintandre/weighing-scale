#include <xc.h>
#include "configs.h"

#define row1    LATDbits.LATD3  
#define row2    LATDbits.LATD2  
#define row3    LATDbits.LATD1  
#define row4    LATDbits.LATD0  
#define col1    PORTCbits.RC3   
#define col2    PORTCbits.RC2   
#define col3    PORTCbits.RC1
#define col4    PORTCbits.RC0 

/**
 * @Summary
 *      Initialize the keyboard library by setting all outputs
 */
void keyboard_init ();

/**
 * @Summary
 *      This function returns the key pressed in the range of 0-15
 * @return 
 */
char key_pressed ();

/**
 * @Summary
 *      Returns a flag indicating if keyboard needs to be serviced
 * @return 
 */
char key_trigger ();