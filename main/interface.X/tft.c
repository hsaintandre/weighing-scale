#include "tft.h"

char bitRead (unsigned int number, unsigned int position) {
    position = 1 << position;
    if (number & position) {
        return 1;
    } else {
        return 0;
    }
}

void lcd_write_bus (char data) {
    BUS = data;
    LCD_WR = 0; // esto podría ser un problema de sincro
    //asm("NOP");   // in case of
    LCD_WR = 1;
}

void lcd_write_command (char cmd) {
    LCD_RS = 0;
    lcd_write_bus(cmd);
    LCD_RS = 1;
}

void lcd_set_address (unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
    lcd_write_command(0x2A); // column address
    lcd_write_bus(x1>>8);
    lcd_write_bus(x1);
    lcd_write_bus(x2>>8);
    lcd_write_bus(x2);
    
    lcd_write_command(0x2B); // page address
    lcd_write_bus(y1>>8);
    lcd_write_bus(y1);
    lcd_write_bus(y2>>8);
    lcd_write_bus(y2);
}

void lcd_memory_start () {
    lcd_write_command(0x2C);
}

void lcd_init () {
    CONTROL = 0xFF; // everything high
    __delay_ms(5);
    LCD_RST = 0;
    __delay_ms(15);
    LCD_RST = 1;
    __delay_ms(15);
    
    LCD_CS = 0;
    
    lcd_write_command(0x11);  // sleep out
    __delay_ms(20);
    lcd_write_command(0xD0);  // power setting
    lcd_write_bus(0x07);
    lcd_write_bus(0x42);
    lcd_write_bus(0x18);

    lcd_write_command(0xD1);  // VCOM control
    lcd_write_bus(0x00);
    lcd_write_bus(0x07);
    lcd_write_bus(0x10);

    lcd_write_command(0xD2);  // power setting for normal mode
    lcd_write_bus(0x01);
    lcd_write_bus(0x00);

    lcd_write_command(0xC0);  // panel driving setting
    lcd_write_bus(0x10);
    lcd_write_bus(0x3B);
    lcd_write_bus(0x00);
    lcd_write_bus(0x02);
    lcd_write_bus(0x11);

    lcd_write_command(0xC5);  // frame rate and inversion control
    lcd_write_bus(0x03);

    lcd_write_command(0x36);  // address mode
    lcd_write_bus(0x0A); // 

    lcd_write_command(0x3A);  // pixel format
    lcd_write_bus(0x55); // 0x55

    lcd_write_command(0x2A);  // column address
    lcd_write_bus(0x00);
    lcd_write_bus(0x00);
    lcd_write_bus(0x01);
    lcd_write_bus(0x3F);

    lcd_write_command(0x2B);  // page address
    lcd_write_bus(0x00);
    lcd_write_bus(0x00);
    lcd_write_bus(0x01);
    lcd_write_bus(0xE0);
    __delay_ms(120);
    lcd_write_command(0x29);  // display on
    
    LCD_CS = 1;
}

void lcd_set_background(unsigned int color) {
    LCD_CS = 0;
    lcd_set_address(0,0,319,479);
    lcd_memory_start();
    for (unsigned int i=0;i<320;i++) {
        for (unsigned int j=0;j<480;j++) {
            lcd_write_bus(color >> 8);
            lcd_write_bus(color);
        }
    }
    LCD_CS = 1;
}

void lcd_pix (unsigned int x, unsigned int y, char c, char size) {
    unsigned int i;
    char n1;
    
    x *= size;
    y *= size;
    n1 = size * size;
    LCD_CS = 0;
    lcd_set_address(x,y,x+size-1,y+size-1);
    lcd_memory_start();

    switch (c) {
        case 1:
            for(i=0;i<n1;i++) {
                lcd_write_bus(0x00);
                lcd_write_bus(0x00);
            }
            break;
        case 0:
            for(i=0;i<n1;i++) {
                lcd_write_bus(0xFF);
                lcd_write_bus(0xFF);
            }
            break;
    }

    LCD_CS = 1;
}

void lcd_pix_color (unsigned int x, unsigned int y, char c, char size) {
    unsigned int i;
    char n1;
    
    x *= size;
    y *= size;
    n1 = size * size;
    LCD_CS = 0;
    lcd_set_address(x,y,x+size-1,y+size-1);
    lcd_memory_start();

    switch (c) {
        case 1:
            for(i=0;i<n1;i++) {
                lcd_write_bus(0xF8);
                lcd_write_bus(0x00);
            }
            break;
        case 0:
            for(i=0;i<n1;i++) {
                lcd_write_bus(0xFF);
                lcd_write_bus(0xFF);
            }
            break;
    }

    LCD_CS = 1;
}

void lcd_sign (char enable) {
    lcd_pix(50,1,enable,5);
    lcd_pix(50,2,enable,5);
    lcd_pix(50,3,enable,5);
    lcd_pix(50,4,enable,5);
    lcd_pix(50,5,enable,5);
    lcd_pix(49,1,enable,5);
    lcd_pix(49,2,enable,5);
    lcd_pix(49,3,enable,5);
    lcd_pix(49,4,enable,5);
    lcd_pix(49,5,enable,5);
}

void lcd_point(char place) {
    lcd_pix(62,96 - 18 * place,1,5);
    lcd_pix(61,96 - 18 * place,1,5);
    lcd_pix(62,97 - 18 * place,1,5);
    lcd_pix(61,97 - 18 * place,1,5);
}

void lcd_number(signed int number) {
    char i,j,k,index[5] = {10,0,0,0,0};

    if (number<0) {  // compute the sign
        lcd_sign(1);
        number = 0 - number;
    } else {
        lcd_sign(0);
    }
    if (number > 9999) {
        index[0] = 0;
    }
    while (number > 9999) {
        index[0] += 1;
        number -= 10000;
    }
    while (number > 999) {
        index[1] += 1;
        number -= 1000;
    }
    if (index[0] == 10 && index[1] == 0) {
        index[1] = 10;
    }
    while (number > 99) {
        index[2] += 1;
        number -= 100;
    }
    if (index[1] == 10 && index[2] == 0) {
        index[2] = 10;
    }
    while (number > 9) {
        index[3] += 1;
        number -= 10;
    }
    index[4] = number;

    for (k=0;k<5;k++) {
        for (i=0;i<22;i++) {
            for (j=0;j<16;j++) {
                lcd_pix(40+i,(18*k+8)+j,bitRead(digitos[index[k]][i],15-j),5);
            }
        }
    }
}

void lcd_text (char renglon, char *msjPtr) {
    unsigned int i,j,k;

    for (k=0;k<15;k++) {
        if (*(msjPtr+k) == 32 || *(msjPtr+k) == 0) {
            *(msjPtr+k) = 38;
        }
        if (*(msjPtr+k) > 47 && *(msjPtr+k) < 58) {
            *(msjPtr+k) -= 48;
        }
        if (*(msjPtr+k) > 64) {
            *(msjPtr+k) -= 53;
        }
        
        for (i=0;i<8;i++) {
            for (j=0;j<8;j++) {
                lcd_pix(1+j+(renglon-1)*9,1+i+k*8,bitRead(texto[*(msjPtr+k)][i],j),4);
            }
        }
    }
}

void lcd_text_color (char renglon, char *msjPtr) {
    unsigned int i,j,k;

    for (k=0;k<15;k++) {
        if (*(msjPtr+k) == 32 || *(msjPtr+k) == 0) {
            *(msjPtr+k) = 38;
        }
        if (*(msjPtr+k) > 47 && *(msjPtr+k) < 58) {
            *(msjPtr+k) -= 48;
        }
        if (*(msjPtr+k) > 64) {
            *(msjPtr+k) -= 53;
        }
        
        for (i=0;i<8;i++) {
            for (j=0;j<8;j++) {
                lcd_pix_color(1+j+(renglon-1)*9,1+i+k*8,bitRead(texto[*(msjPtr+k)][i],j),4);
            }
        }
    }
}