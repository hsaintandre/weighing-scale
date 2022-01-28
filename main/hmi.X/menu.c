#include "menu.h"
#include "tft.h"
#include "eeprom.h"

/* Constants */
char msjCALIBRATE[16] = "CALIBRATE      ";
char msjZERO[16] = "ZERO           ";
//char msjDIVISIONS[16] = "DIVISIONS      ";
char msjDIVISIONS[16] = "DECIMALS       ";
char msjPATTERN[16] = "PATTERN        ";
char msjFILTER[16] = "FILTER         ";
char msjSTEP[16] = "TARE           ";
char msjTHRESHOLD[16] = "THRESHOLD      ";
char msjRANGE[16] = "STABLE RANGE   ";

char msjSERIAL[16] = "SERIAL         ";
char msjBAUD[16] = "BAUD           ";

char msjETHERNET[16] = "ETHERNET       ";
char msjGATEWAY[16] = "GATEWAY        ";
char msjSUBNETMASK[16] = "SUBNET MASK    ";
char msjMAC[16] = "MAC            ";
char msjIP[16] = "IP             ";
char msjPORT[16] = "PORT           ";

char msjPASSWORD[16] = "PASSWORD       ";
char msjCHANGE[16] = "CHANGE         ";

char msjRESET[16] = "RESET          ";

char msjFINISH[16] = "FINISH         ";
char msjCONFIRM[16] = "CONFIRM        ";
char msjCANCEL[16] = "CANCEL         ";

char msjON[16] = "ON             ";
char msjOFF[16] = "OFF            ";

char msjDONE[16] = "DONE           ";
char msjWAIT[16] = "WAIT           ";

char msjDIVISION0[16] = "0              ";
char msjDIVISION1[16] = "0.0            ";
char msjDIVISION2[16] = "0.00           ";
char msjDIVISION3[16] = "0.000          ";

char msjFILTER1[16] = "WEAK           ";
char msjFILTER2[16] = "MEDIUM         ";
char msjFILTER3[16] = "STRONG         ";

char msjBAUD1[16] = "9600           ";
char msjBAUD2[16] = "19200          ";
char msjBAUD3[16] = "38400          ";
char msjBAUD4[16] = "115200         ";

char msjWEIGHTINGR[16] = "WEIGHT IN GR   ";
char msjTAREINGR[16] = "TARE IN GR     ";
char msjVALUEINGR[16] = "VALUE IN GR    ";

/* Global variables */
char m1 = 0;
char memBuff1[16] = "               ";
char memDivision = 1;
char memFilter = 1;
char memBaud = 1;
char memGateway[16] = "192.168.0.254  ";
unsigned int inGateway[4] = {0,0,0,0};
char memSubnet[16] = "255.255.255.0  ";
unsigned int inSubnet[4] = {0,0,0,0};
char memIp[16] = "192.168.0.10   ";
unsigned int inIp[4] = {0,0,0,0};
char memMac[16] = "021EE3800AA2   ";
char inMac = 0;
char memPort[16] = "8080           ";
unsigned int inPort = 0;

void menu_init () {
    if (mem_read(0x20) == 0xAA) {
        memDivision = mem_read(0x21);
        memFilter = mem_read(0x22);
        memBaud = mem_read(0x23);
    }
    else {
        mem_write(0x21, memDivision);
        mem_write(0x22, memFilter);
        mem_write(0x23, memBaud);
        mem_write(0x20, 0xAA);  // mem init
    }
}

void menu_1 (char colorRow) {   // main menu
    char mBlank[16] = "               ";
    if (colorRow == 1) {
        lcd_text(1,msjCALIBRATE,1);
    } else {
        lcd_text(1,msjCALIBRATE,0);
    }
    if (colorRow == 2) {
        lcd_text(2,msjSERIAL,1);
    } else {
        lcd_text(2,msjSERIAL,0);
    }
    if (colorRow == 3) {
        lcd_text(3,msjETHERNET,1);
    } else {
        lcd_text(3,msjETHERNET,0);
    }
    if (colorRow == 4) {
        lcd_text(4,msjPASSWORD,1);
    } else {
        lcd_text(4,msjPASSWORD,0);
    }
    if (colorRow == 5) {
        lcd_text(5,msjRESET,1);
    } else {
        lcd_text(5,msjRESET,0);
    }
    if (colorRow == 6) {
        lcd_text(6,msjFINISH,1);
    } else {
        lcd_text(6,msjFINISH,0);
    }
    
    lcd_text(7,mBlank,0);
}

void menu_2 (char colorRow) {   // calibration menu
    char mBlank[16] = "               ";
    if (colorRow == 1) {
        lcd_text(1,msjZERO,1);
    } else {
        lcd_text(1,msjZERO,0);
    }
    if (colorRow == 2) {
        lcd_text(2,msjDIVISIONS,1);
    } else {
        lcd_text(2,msjDIVISIONS,0);
    }
    if (colorRow == 3) {
        lcd_text(3,msjPATTERN,1);
    } else {
        lcd_text(3,msjPATTERN,0);
    }
    if (colorRow == 4) {
        lcd_text(4,msjFILTER,1);
    } else {
        lcd_text(4,msjFILTER,0);
    }
    if (colorRow == 5) {
        lcd_text(5,msjSTEP,1);
    } else {
        lcd_text(5,msjSTEP,0);
    }
    if (colorRow == 6) {
        lcd_text(6,msjTHRESHOLD,1);
    } else {
        lcd_text(6,msjTHRESHOLD,0);
    }
    if (colorRow == 7) {
        lcd_text(7,msjRANGE,1);
    } else {
        lcd_text(7,msjRANGE,0);
    }
    
//    for (char i=0;i<2;i++) {
//        lcd_text(6+i,mBlank,0);
//    }
}

void menu_21 (char colorRow) {  // WAIT/DONE
    char mBlank[16] = "               ";
    if (colorRow) {
        lcd_text(1,msjDONE,0);
    } else {
        lcd_text(1,msjWAIT,0);
    }
    for (char i=0;i<6;i++) {
        lcd_text(2+i,mBlank,0);
    }
}

void menu_22 (char colorRow) {  // decimal places menu
    char mBlank[16] = "               ";
    if (colorRow == 1) {
        lcd_text(1,msjDIVISION0,1);
    } else {
        lcd_text(1,msjDIVISION0,0);
    }
    if (colorRow == 2) {
        lcd_text(2,msjDIVISION1,1);
    } else {
        lcd_text(2,msjDIVISION1,0);
    }
    if (colorRow == 3) {
        lcd_text(3,msjDIVISION2,1);
    } else {
        lcd_text(3,msjDIVISION2,0);
    }
    if (colorRow == 4) {
        lcd_text(4,msjDIVISION3,1);
    } else {
        lcd_text(4,msjDIVISION3,0);
    }
    
    for (char i=0;i<3;i++) {
        lcd_text(5+i,mBlank,0);
    }
}

void menu_23 (char colorRow) {  // pattern menu
    char mBlank[16] = "               ";
    lcd_text(1,msjWEIGHTINGR,0);
    for (char i=0;i<6;i++) {
        lcd_text(2+i,mBlank,0);
    }
}

void menu_24 (char colorRow) {  // filter menu
    char mBlank[16] = "               ";
    if (colorRow == 1) {
        lcd_text(1,msjFILTER1,1);
    } else {
        lcd_text(1,msjFILTER1,0);
    }
    if (colorRow == 2) {
        lcd_text(2,msjFILTER2,1);
    } else {
        lcd_text(2,msjFILTER2,0);
    }
    if (colorRow == 3) {
        lcd_text(3,msjFILTER3,1);
    } else {
        lcd_text(3,msjFILTER3,0);
    }
    
    for (char i=0;i<4;i++) {
        lcd_text(4+i,mBlank,0);
    }
}
void menu_25 (char colorRow) {  // tare menu
    char mBlank[16] = "               ";
    lcd_text(1,msjTAREINGR,0);
    for (char i=0;i<6;i++) {
        lcd_text(2+i,mBlank,0);
    }
}

void menu_26 (char colorRow) {  // threshold menu
    char mBlank[16] = "               ";
    lcd_text(1,msjVALUEINGR,0);
    for (char i=0;i<6;i++) {
        lcd_text(2+i,mBlank,0);
    }
}

void menu_27 (char colorRow) {  // range menu
    char mBlank[16] = "               ";
    lcd_text(1,msjVALUEINGR,0);
    for (char i=0;i<6;i++) {
        lcd_text(2+i,mBlank,0);
    }
}

void menu_3 (char colorRow) {   // serial menu
    char mBlank[16] = "               ";
    if (colorRow == 1) {
        lcd_text(1,msjBAUD,1);
    } else {
        lcd_text(1,msjBAUD,0);
    }
    if (colorRow == 2) {
        lcd_text(2,msjON,1);
    } else {
        lcd_text(2,msjON,0);
    }
    if (colorRow == 3) {
        lcd_text(3,msjOFF,1);
    } else {
        lcd_text(3,msjOFF,0);
    }
    
    for (char i=0;i<4;i++) {
        lcd_text(4+i,mBlank,0);
    }
}

void menu_31 (char colorRow) {  // baud menu
    char mBlank[16] = "               ";
    if (colorRow == 1) {
        lcd_text(1,msjBAUD1,1);
    } else {
        lcd_text(1,msjBAUD1,0);
    }
    if (colorRow == 2) {
        lcd_text(2,msjBAUD2,1);
    } else {
        lcd_text(2,msjBAUD2,0);
    }
    if (colorRow == 3) {
        lcd_text(3,msjBAUD3,1);
    } else {
        lcd_text(3,msjBAUD3,0);
    }
    if (colorRow == 4) {
        lcd_text(4,msjBAUD4,1);
    } else {
        lcd_text(4,msjBAUD4,0);
    }
    
    for (char i=0;i<3;i++) {
        lcd_text(5+i,mBlank,0);
    }
}

void menu_4 (char colorRow) {   // Ethernet menu
    char mBlank[16] = "               ";
    if (colorRow == 1) {
        lcd_text(1,msjGATEWAY,1);
    } else {
        lcd_text(1,msjGATEWAY,0);
    }
    if (colorRow == 2) {
        lcd_text(2,msjSUBNETMASK,1);
    } else {
        lcd_text(2,msjSUBNETMASK,0);
    }
    if (colorRow == 3) {
        lcd_text(3,msjMAC,1);
    } else {
        lcd_text(3,msjMAC,0);
    }
    if (colorRow == 4) {
        lcd_text(4,msjIP,1);
    } else {
        lcd_text(4,msjIP,0);
    }
    if (colorRow == 5) {
        lcd_text(5,msjPORT,1);
    } else {
        lcd_text(5,msjPORT,0);
    }
    if (colorRow == 6) {
        lcd_text(6,msjON,1);
    } else {
        lcd_text(6,msjON,0);
    }
    if (colorRow == 7) {
        lcd_text(7,msjOFF,1);
    } else {
        lcd_text(7,msjOFF,0);
    }
    
}

void menu_41 (char colorRow) {  // gateway menu
    char mBlank[16] = "               ";
    lcd_text(1,memGateway,colorRow);
    for (char i=0;i<6;i++) {
        lcd_text(2+i,mBlank,0);
    }
}

void menu_42 (char colorRow) {  // subnet menu
    char mBlank[16] = "               ";
    lcd_text(1,memSubnet,colorRow);
    for (char i=0;i<6;i++) {
        lcd_text(2+i,mBlank,0);
    }
}

void menu_43 (char colorRow) {  // MAC menu
    char mBlank[16] = "               ";
    lcd_text(1,memMac,colorRow);
    for (char i=0;i<6;i++) {
        lcd_text(2+i,mBlank,0);
    }
}

void menu_44 (char colorRow) {  // IP menu
    char mBlank[16] = "               ";
    lcd_text(1,memIp,colorRow);
    for (char i=0;i<6;i++) {
        lcd_text(2+i,mBlank,0);
    }
}

void menu_45 (char colorRow) {  // port menu
    char mBlank[16] = "               ";
    lcd_text(1,memPort,colorRow);
    for (char i=0;i<6;i++) {
        lcd_text(2+i,mBlank,0);
    }
}

void menu_46 (char colorRow);
void menu_47 (char colorRow);

void menu_5 (char colorRow) {   // password menu
    char mBlank[16] = "               ";
    if (colorRow == 1) {
        lcd_text(1,msjCHANGE,1);
    } else {
        lcd_text(1,msjCHANGE,0);
    }
    if (colorRow == 2) {
        lcd_text(2,msjON,1);
    } else {
        lcd_text(2,msjON,0);
    }
    if (colorRow == 3) {
        lcd_text(3,msjOFF,1);
    } else {
        lcd_text(3,msjOFF,0);
    }
    
    for (char i=0;i<4;i++) {
        lcd_text(4+i,mBlank,0);
    }
}

void menu_6 (char colorRow) {   // reset menu
    char mBlank[16] = "               ";
    if (colorRow == 1) {
        lcd_text(1,msjCONFIRM,1);
    } else {
        lcd_text(1,msjCONFIRM,0);
    }
    if (colorRow == 2) {
        lcd_text(2,msjCANCEL,1);
    } else {
        lcd_text(2,msjCANCEL,0);
    }
    
    for (char i=0;i<5;i++) {
        lcd_text(3+i,mBlank,0);
    }
}

void menu_navigate (char opt, char rowColor) {
    switch (opt) {
        case 1: // main menu
            menu_1(rowColor);
            break;
        case 2: // calibrate menu
            menu_2(rowColor);
            break;
        case 3: // serial menu
            menu_3(rowColor);
            break;
        case 4: // Ethernet menu
            menu_4(rowColor);
            break;
        case 5: // password menu
            menu_5(rowColor);
            break;
        case 6: // reset menu
            menu_6(rowColor);
            break;
        case 21:
            menu_21(rowColor);
            break;
        case 22:
            menu_22(rowColor);
            break;
        case 23:
            menu_23(rowColor);
            break;
        case 24:
            menu_24(rowColor);
            break;
        case 25:
            menu_25(rowColor);
            break;
        case 26:
            menu_26(rowColor);
            break;
        case 27:
            menu_27(rowColor);
            break;
        case 31:
            menu_31(rowColor);
            break;
        case 41:
            menu_41(rowColor);
            break;
        case 42:
            menu_42(rowColor);
            break;
        case 43:
            menu_43(rowColor);
            break;
        case 44:
            menu_44(rowColor);
            break;
        case 45:
            menu_45(rowColor);
            break;
        
    }
}

char menu_next (char *menuShown, char *menuRow, char command, char *menuText) {
    char aux1 = 0;
    char rta = 0;
    
    switch (command) {
        case 'A':   // up
            if (*(menuShown) == 0) {
                // alternative function when weighing
            }
            if (*(menuRow) > 1 && *(menuShown) > 0) {
                (*(menuRow))--;
            } else {
                if (*(menuShown) == 1 && *(menuRow) == 1) { // 1st menu can be surf both directions
                    *(menuRow) = 6;
                }
            }
            
            menu_navigate(*(menuShown),*(menuRow));
            break;
        case 'B':   // down
            switch(*(menuShown)) {
                case 0: // alternative function when weighing
                    break;
                case 1: // main menu
                    if (*(menuRow) < 6) {
                        (*(menuRow))++;
                    }
                    break;
                case 2: // calibrate menu
                    if (*(menuRow) < 7) {
                        (*(menuRow))++;
                    }
                    break;
                case 3: // serial menu
                    if (*(menuRow) < 3) {
                        (*(menuRow))++;
                    }
                    break;
                case 4: // Ethernet menu
                    if (*(menuRow) < 7) {
                        (*(menuRow))++;
                    }
                    break;
                case 5: // password menu
                    if (*(menuRow) < 3) {
                        (*(menuRow))++;
                    }
                    break;
                case 6: // reset menu
                    if (*(menuRow) < 2) {
                        (*(menuRow))++;
                    }
                    break;
                case 21:
                    break;
                case 22:    // divisions menu
                    if (*(menuRow) < 4) {
                        (*(menuRow))++;
                    }
                    break;
                case 24:    // filter menu
                    if (*(menuRow) < 3) {
                        (*(menuRow))++;
                    }
                    break;
                case 31:
                    if (*(menuRow) < 4) {
                        (*(menuRow))++;
                        memBaud = *(menuRow);
                    }
                    break;
                case 43:
                    
                    break;
            }
            menu_navigate(*(menuShown),*(menuRow));
            break;
        case '#':   // enter
            switch (*(menuShown)) { 
               case 1:
                    switch (*(menuRow)) {
                        case 1: // calibrate
                            *(menuShown) = 2;
                            *(menuRow) = 1;
                            menu_navigate(*(menuShown),1);
                            break;
                        case 2: // serial
                            *(menuShown) = 3;
                            *(menuRow) = 1;
                            menu_navigate(*(menuShown),1);
                            break;
                        case 3: // Ethernet
                            *(menuShown) = 4;
                            *(menuRow) = 1;
                            menu_navigate(*(menuShown),1);
                            break;
                        case 4: // password
                            *(menuShown) = 5;
                            *(menuRow) = 1;
                            menu_navigate(*(menuShown),1);
                            break;
                        case 5: // reset
                            *(menuShown) = 6;
                            *(menuRow) = 1;
                            menu_navigate(*(menuShown),1);
                            break;
                        case 6: // finish
                            lcd_set_background(0xF800);
                            lcd_set_background(0x07E0);
                            lcd_set_background(0x001F);
                            lcd_set_background(0xFFFF);
                            *(menuShown) = 0;
                            *(menuRow) = 1;
                            rta = 0;
                            break;
                    }
                    break;
                case 2:
                    switch (*(menuRow)) {
                        case 1: // calibrate -> zero
                            *(menuShown) = 21;
                            *(menuRow) = 1;
                            menu_navigate(*(menuShown),0);
                            rta = 21;
                            break;
                        case 2: // calibrate -> divisions
                            *(menuShown) = 22;
                            *(menuRow) = memDivision;
                            menu_navigate(*(menuShown),*(menuRow));
                            break;
                        case 3: // calibrate -> pattern
                            *(menuShown) = 23;
                            *(menuRow) = 1;
                            menu_navigate(*(menuShown),*(menuRow));
                            break;
                        case 4: // calibrate -> filter
                            *(menuShown) = 24;
                            *(menuRow) = memFilter;
                            menu_navigate(*(menuShown),*(menuRow));
                            break;
                        case 5: // calibrate -> tare
                            *(menuShown) = 25;
                            *(menuRow) = 1;
                            menu_navigate(*(menuShown),*(menuRow));
                            break;
                        case 6: // calibrate -> threshold
                            *(menuShown) = 26;
                            *(menuRow) = 1;
                            menu_navigate(*(menuShown),*(menuRow));
                            break;
                        case 7: // calibrate -> range
                            *(menuShown) = 27;
                            *(menuRow) = 1;
                            menu_navigate(*(menuShown),*(menuRow));
                            break;
                    }
                    break;
                case 22:
                    *(menuShown) = 21;
                    memDivision = *(menuRow);
                    mem_write (0x21, memDivision);
                    menu_navigate(*(menuShown),0);
                    rta = 22;
                    break;
                case 23:
                    *(menuShown) = 21;
                    menu_navigate(*(menuShown),0);
                    rta = 23;
                    break;
                case 24:
                    *(menuShown) = 21;
                    memFilter = *(menuRow);
                    mem_write(0x22, memFilter);
                    menu_navigate(*(menuShown),0);
                    rta = 24;
                    break;
                case 25:
                    *(menuShown) = 21;
                    menu_navigate(*(menuShown),1);
                    rta = 25;
                    break;
                case 26:
                    *(menuShown) = 21;
                    menu_navigate(*(menuShown),1);
                    rta = 26;
                    break;    
                case 27:
                    *(menuShown) = 21;
                    menu_navigate(*(menuShown),1);
                    rta = 27;
                    break;
                case 3:
                    switch (*(menuRow)) {
                        case 1: // serial -> baud
                            *(menuShown) = 31;
                            *(menuRow) = memBaud;
                            memBaud = mem_read(0x23);
                            menu_navigate(*(menuShown),*(menuRow));
                            break;
                        case 2: // serial -> ON
                            menu_21(1);
                            *(menuShown) = 32;
                            rta = 1;
                            break;
                        case 3: // serial -> OFF
                            menu_21(1);
                            *(menuShown) = 33;
                            rta = 1;
                            break;
                    }
                    break;
                case 31:
                    menu_21(1);
                    rta = 1;
                    break;
                case 4: // Ethernet
                    switch (*(menuRow)) {
                        case 1: // Ethernet -> gateway
                            *(menuShown) = 41;
                            *(menuRow) = 0;
                            menu_navigate(*(menuShown),*(menuRow));
                            break;
                        case 2: // Ethernet -> subnet
                            *(menuShown) = 42;
                            *(menuRow) = 0;
                            menu_navigate(*(menuShown),*(menuRow));
                            break;
                        case 3: // Ethernet -> MAC
                            *(menuShown) = 43;
                            *(menuRow) = 0;
                            menu_navigate(*(menuShown),*(menuRow));
                            break;
                        case 4: // Ethernet -> IP
                            *(menuShown) = 44;
                            *(menuRow) = 0;
                            menu_navigate(*(menuShown),*(menuRow));
                            break;
                        case 5: // Ethernet -> port
                            *(menuShown) = 45;
                            *(menuRow) = 0;
                            menu_navigate(*(menuShown),*(menuRow));
                            break;
                        case 6: // Ethernet -> on
                            menu_21(1);
                            *(menuShown) = 46;
                            rta = 1;
                            break;
                        case 7: // Ethernet -> off
                            menu_21(1);
                            *(menuShown) = 47;
                            rta = 1;
                            break;
                        
                    }
                    break;
                case 43:
                    
                    break;
            }
            m1 = 0;
            for (char i=0;i<15;i++) {
                memBuff1[i] = ' ';
            }
            break;
        case '*':   // back
            if (*(menuShown) > 1) {
                if (*(menuShown) < 10) {
                    *(menuRow) = *(menuShown) - 1;
                    *(menuShown) = 1;
                } else {
                    aux1 = *(menuShown);
                    (*(menuShown)) /= 10;
                    *(menuRow) = aux1 - (*(menuShown)) * 10;
                }
            }
            menu_navigate(*(menuShown),*(menuRow));
            m1 = 0;
            for (char i=0;i<15;i++) {
                memBuff1[i] = ' ';
            }
            break;
        case 'C':   // show menu
            if (*(menuShown) == 0) {
                lcd_set_background(0xFFFF);
                *(menuShown) = 1;
                *(menuRow) = 1;
                menu_navigate(*(menuShown),*(menuRow));
            } else {
                // alternative function
            }
            
            break;
        case 'D':
            if (*(menuShown) == 41) {   // Ethernet -> gateway
                memBuff1[m1] = '.';     // prints a point
                m1++;
                lcd_text(2,memBuff1,1);
            }
            if (*(menuShown) == 42) {   // Ethernet -> subnet mask
                memBuff1[m1] = '.';     // prints a point
                m1++;
                lcd_text(2,memBuff1,1);
            }
            if (*(menuShown) == 43) {   // Ethernet -> MAC
                if (m1 < 11) {          // next digit
                    m1++;
                }
                memBuff1[m1] = '0';
                lcd_text(2,memBuff1,1);
            }
            if (*(menuShown) == 44) {
                memBuff1[m1] = '.';
                m1++;
                lcd_text(2,memBuff1,1);
            }
            break;
        default:    // numbers
            if (*(menuShown) == 23) {
                lcd_text(2,menuText,0);
            }
            if (*(menuShown) == 25) {
                lcd_text(2,menuText,0);
            }
            if (*(menuShown) == 26) {
                lcd_text(2,menuText,0);
            }
            if (*(menuShown) == 27) {
                lcd_text(2,menuText,0);
            }
            if (*(menuShown) == 41) {   // Ethernet -> gateway
                memBuff1[m1] = command;
                m1++;
                lcd_text(2,memBuff1,1);
            }
            if (*(menuShown) == 42) {   // Ethernet -> subnet mask
                memBuff1[m1] = command;
                m1++;
                lcd_text(2,memBuff1,1);
            }
            if (*(menuShown) == 43 && command == '1') {         // Ethernet -> MAC
                if (memBuff1[m1] > 47 && memBuff1[m1] < 58) {
                    memBuff1[m1]++;
                }
                if (memBuff1[m1] == 32 || memBuff1[m1] == 'F') {
                    memBuff1[m1] = '0';
                }
                if (memBuff1[m1] > 64) {
                    memBuff1[m1]++;
                }
                if (memBuff1[m1] == 58) {
                    memBuff1[m1] = 'A';
                }
                lcd_text(2,memBuff1,1);
            }
            if (*(menuShown) == 44) {
                memBuff1[m1] = command;
                m1++;
                lcd_text(2,memBuff1,1);
            }
            if (*(menuShown) == 45) {
                memBuff1[m1] = command;
                m1++;
                lcd_text(2,memBuff1,1);
            }
            break;
    }
    
    return rta;
}