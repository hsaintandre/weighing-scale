#include "gpio.h"

void gpio_init () {
    TRISAbits.TRISA6 = 0;   // 220v
    TRISCbits.TRISC4 = 0;   // 24v OUT1
    TRISCbits.TRISC5 = 0;   // 24v OUT2
    TRISAbits.TRISA7 = 1;   // trigger
    TRISDbits.TRISD4 = 1;   // 24v IN1
    TRISDbits.TRISD5 = 1;   // 24v IN2
    OUT1 = 0;
    OUT2 = 0;
    OUT3 = 0;
}

uint8_t gpio_read () {
    uint8_t ans = 0;
    
    ans = OUT1;
    ans << 1;
    ans += OUT2;
    ans << 1;
    ans += OUT3;
    ans << 1;
    ans += TRIGGER;
    ans << 1;
    ans += IN1;
    ans << 1;
    ans += IN2;
    
    return ans;
}

void gpio_write (uint8_t out) {
    OUT3 = out && 0x01;
    OUT2 = out && 0x02;
    OUT1 = out && 0x04;
}