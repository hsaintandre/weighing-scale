#include "keyboard.h"

void keyboard_init () {
    row1 = 1;
    row2 = 1;
    row3 = 1;
    row4 = 1;
}

char key_pressed () {
    char keyRta = 20;
    char inPort = 0;
    char colPressed = 0;
    char outPort = 1;
    
    colPressed = PORTC & 0x0F;  // 1 2 4 8
    __delay_us(10);
    row1 = 0;
    row2 = 0;
    row3 = 0;
    row4 = 0;
    __delay_us(10);
    switch (colPressed) {
        case 1:
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x01;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = 'D';
            }
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x02;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = 'C';
            }
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x04;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = 'B';
            }
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x08;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = 'A';
            }
            break;
        case 2:
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x01;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = '#';
            }
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x02;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = '9';
            }
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x04;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = '6';
            }
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x08;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = '3';
            }
            break;
        case 4:
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x01;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = '0';
            }
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x02;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = '8';
            }
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x04;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = '5';
            }
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x08;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = '2';
            }
            break;
        case 8:
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x01;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = '*';
            }
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x02;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = '7';
            }
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x04;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = '4';
            }
            LATD = LATD & 0xF0; // clear lower nibble
            __delay_us(10);
            LATD = LATD | 0x08;
            __delay_us(10);
            inPort = PORTC & 0x0F;
            if (inPort) {
                keyRta = '1';
            }
            break;
    }
    __delay_us(10);
    
    row1 = 1;
    row2 = 1;
    row3 = 1;
    row4 = 1;
    
    return keyRta;
}

char key_trigger () {
    if (col1 || col2 || col3 || col4) {
        return 1;
    } else {
        return 0;
    }
}