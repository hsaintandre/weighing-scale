/* Matrix keyboard */
#define ROW1 PORTBbits.RB7
#define ROW2 PORTBbits.RB6
#define ROW3 PORTBbits.RB5
#define ROW4 PORTBbits.RB4

#define COL1 LATB3
#define COL2 LATB2
#define COL3 LATB1
#define COL4 LATB0

/* Interrupt on change enable bit */
#define IOC INTCONbits.RBIE
/* Interrupt on change flag bit */
#define IOF INTCONbits.RBIF

