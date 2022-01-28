#include <xc.h> // include processor files - each processor file is guarded.  
#include <libpic30.h>
#include <p33FJ128GP802.h>

#define NACK    1
#define ACK     0

#ifndef byte
    #define byte unsigned char
#endif

void i2c_init (void);
void i2c_start (void);
void i2c_r_start (void);
void i2c_stop (void);
byte i2c_write (byte data_tx);
byte i2c_read ();
void i2c_ack (byte ack_bit);