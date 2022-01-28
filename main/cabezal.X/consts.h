#include <p33FJ128GP802.h>

#define FCY 39936250
//#define FCY 40000000
//#define FCY 3700000 // internal RC oscillator

#ifndef byte
    #define byte unsigned char
#endif
    
#ifndef word
    #define word unsigned int
#endif

/* Input */
/* Output */

#define OFF         0
#define ON          1

#define BAUD1   9600
#define BAUD2   19200
#define BAUD3   38400
#define BAUD4   115200

#define P1      0
#define P8      1
#define P64     2
#define P256    3

/* Config 1 */
//#define CS      _LATB15
//#define RST     _LATB11
//#define SS      _LATB10
/* Config 2 */

#define CS      _LATB11
#define RST     _LATB10
#define SS      _LATB15
#define DRDY    PORTAbits.RA0

//#define     _LATB5
//#define     _LATB6
//#define     _LATB7
//#define     _LATB8
//#define     _LATB9
//#define     _LATB10
//#define     _LATB11
//#define     _LATB12
//#define     _LATB13
//#define     _LATB14
//#define     _LATB15

/* Function prototypes */

/**
 * @Summary
 *      This function must execute only once at power on.
 *      In this function the device specific configurations that enable
 *      the operation are made.
 *      It sets up the oscillator, I2C and timer 2.
 */
void startup ();

/**
 * @Summary
 *      Boots the system loading configurations from memory.
 *      It configures peripherals related with the overall operation.
 *      Can be called as many times as needed.
 */
void system_boot ();

/**
 * @Summary
 *      Configures the core of DSP.
 */
void core_config();

/**
 * @Summary
 *      UART setup function.
 * @param number    1 or 2 for UART1 or UART2
 * @param baud      BAUD1/2/3/4 for 9600/19200/38400/115200
 * @param enable    ON/OFF
 */
void uart_config(byte number, unsigned long baud, byte enable);

/**
 * @Summary
 *      Real time clock configuration.
 * @param year      year tens
 * @param month     number of month
 * @param day       day of the month
 * @param hour      hour in 24 hs format
 * @param minute    minutes
 */
void rtc_config(byte year, byte month, byte day, byte hour, byte minute);  //Timer 1 used as a RTC

/**
 * @Summary
 *      Configuration of timer 2 running at Fcy.
 * @param pre   prescaler 0, 1, 2, 3. Corresponding to 1, 8, 64 and 256
 * @param eoc   word: the end of count
 * @param en    0 or 1 to disable/enable
 */
void timer2_config(byte pre, word eoc, byte en);

/**
 * @brief
 *  This function will handle information about the GPIOS
 * @param
 *  opt options
 */
void gpio(byte opt);