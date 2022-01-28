#include "w5100.h"
#include "consts.h"
#include "24LC512.h"
#include "spi.h"

void boot_ethernet () {
    //byte *status_ptr; uncomment for eeprom saved values
    byte i;
    
    /************** TEST PARAMETERS ***************************/
    /* MAC */
    byte MAC_s[6] = {0x02,0x1E,0xE3,0x80,0x0A,0xA4};    //LAA

    /* IP */
    byte subnet_mask[4] = {255,255,255,0};
    byte IP_gateway[4] = {10,1,109,254};
    byte IP_s[4] = {10,1,109,29};

    /* TCP */
    byte TCP_sp[2] = {0x08,0xB0};   //port 2224
    //byte TCP_sp[2] = {0x00,0x50};   //port 80
    /**********************************************************/
    
    RST = 0;
    CS = 1;
    __delay_ms(1000);
    RST = 1;
    __delay_ms(1000);
    
    while (spi_ov()) {  // prepare spi module to transmission
        spi_flush();
    }
    __delay_us(5);
    for (i=0;i<4;i++) {    //set gateway
        CS = 0;
        __delay_us(5);
        spi(wr);
        spi(0x00);
        spi(gateway + i);
        //spi(read_eeprom(gateway_ad + i, status_ptr));
        spi(IP_gateway[i]);
        __delay_us(5);
        CS = 1;
        __delay_us(5);
    }
    __delay_us(5);
    for (i=0;i<4;i++) {    //set subnet mask
        CS = 0;
        __delay_us(5);
        spi(wr);
        spi(0x00);
        spi(subnet + i);
        //spi(read_eeprom(subnet_ad + i, status_ptr));
        spi(subnet_mask[i]);
        __delay_us(5);
        CS = 1;
        __delay_us(5);
    }
    __delay_us(5);
    for (i=0;i<6;i++) {    //set source MAC
        CS = 0;
        __delay_us(5);
        spi(wr);
        spi(0x00);
        spi(s_mac + i);
        //spi(read_eeprom(MAC_ad + i, status_ptr));
        spi(MAC_s[i]);
        __delay_us(5);
        CS = 1;
        __delay_us(5);
    }
    __delay_us(5);
    for (i=0;i<4;i++) {    //set source IP
        CS = 0;
        __delay_us(5);
        spi(wr);
        spi(0x00);
        spi(s_ip + i);
        //spi(read_eeprom(IP_ad + i, status_ptr));
        spi(IP_s[i]);
        __delay_us(5);
        CS = 1;
        __delay_us(5);
    }
    __delay_us(5);
    CS = 0;
    __delay_us(5);
    spi(wr);
    spi(0x00);
    spi(imr);
    spi(0b00000000); //Socket 0 interrupt enable
    __delay_us(5);
    CS = 1;
    __delay_us(10);
    CS = 0;
    __delay_us(5);
    spi(wr);
    spi(skt0);
    spi(mode);
    spi(0b00100001); //TCP mode
    __delay_us(5);
    CS = 1;
    __delay_us(5);
    for (i=0;i<2;i++) {    //set source port
        CS = 0;
        __delay_us(5);
        spi(wr);
        spi(skt0);
        spi(s_port + i);
        //spi(read_eeprom(port_ad + i, status_ptr));
        spi(TCP_sp[i]);
        __delay_us(5);
        CS = 1;
        __delay_us(5);
    }
    __delay_us(5);
}

byte check_int () {
    byte int_vector;
    
    while (spi_ov()) {  // prepare spi module to transmission
        spi_flush();
    }
    
    CS = 0;
    __delay_us(5);
    spi(rd);
    spi(skt0);
    spi(s_int);
    int_vector = spi(0x00); //Get status
    __delay_us(5);
    CS = 1;
    return int_vector;
}

byte memoria () {
    byte memoria_rta;
    
    while (spi_ov()) {  // prepare spi module to transmission
        spi_flush();
    }
    __delay_us(5);
    CS = 0;
    __delay_us(5);
    spi(rd);
    spi(0x00);
    spi(0x1A);
    memoria_rta = spi(0);
    __delay_us(5);
    CS = 1;
    return memoria_rta;
}

byte get_status() {
    byte estado;
    
    CS = 0;
    __delay_us(5);
    spi(rd);
    spi(skt0);
    spi(stat);
    estado = spi(0x00); //Get status
    __delay_us(5);
    CS = 1;
    
    return estado;
}

void get_ip (byte *ptr) {
    byte i;
    
    while (spi_ov()) {  // prepare spi module to transmission
        spi_flush();
    }
    for (i=0;i<4;i++) {
        __delay_us(5);
        CS = 0;
        __delay_us(5);
        spi(rd);
        spi(0x00);
        spi(s_ip + i);
        *(ptr + i) = spi(0);
        __delay_us(5);
        CS = 1;
    }
}

word get_eth_data (byte *ptr) {
    word rx_size, rx_offset, rx_pointer, rx_address, hold, split_end, total_data = 0, j;
    byte up_ad, low_ad;
    
    while (spi_ov()) {  // prepare spi module to transmission
        spi_flush();
    }
    
    CS = 0;
    __delay_us(5);
    spi(rd);
    spi(skt0);
    spi(rx_rcv);
    rx_size = spi(0x00); //Get received size upper byte
    __delay_us(5);
    CS = 1;
    __delay_us(5);
    rx_size = rx_size << 8; //shift left one byte
    __delay_us(5);
    CS = 0;
    spi(rd);
    spi(skt0);
    spi(rx_rcv + 1);
    rx_size += spi(0x00); //Get received size lower byte
    __delay_us(5);
    CS = 1;
    __delay_us(10);
    
    CS = 0;
    __delay_us(5);
    spi(rd);
    spi(skt0);
    spi(rx_rp);
    rx_pointer = spi(0x00); //Get received pointer upper byte
    __delay_us(5);
    CS = 1;
    __delay_us(5);
    rx_pointer = rx_pointer << 8; //shift left one byte
    CS = 0;
    __delay_us(5);
    spi(rd);
    spi(skt0);
    spi(rx_rp + 1);
    rx_pointer += spi(0x00); //Get received pointer lower byte
    __delay_us(5);
    CS = 1;
    
    rx_offset = rx_pointer & mask;
    rx_address = rx_base + rx_offset;    //This is the address to start reading
    if ((rx_address + rx_size) >= 0x6800) {  //split read
        rx_offset = 0x6800 - rx_address;    //this is the offset to continue
        for (j=0;j<rx_offset;j++) {  //read up to 0x6800
            hold = rx_address + j;
            low_ad = hold & 0x00FF; //lower byte
            hold = hold >> 8;   //shift right
            up_ad = hold & 0x00FF;  //upper byte
            __delay_us(5);
            CS = 0;
            spi(rd);
            spi(up_ad);
            spi(low_ad);
            *(ptr + j) = spi(0x00); //Get data
            total_data++;
            __delay_us(5);
            CS = 1;
        }
        
        __delay_us(5);
        split_end = rx_size - rx_offset;
        rx_address = rx_base;
        for (j=0;j<split_end;j++) {  //read remaining
            hold = rx_address + j;
            low_ad = hold & 0x00FF; //lower byte
            hold = hold >> 8;   //shift right
            up_ad = hold & 0x00FF;  //upper byte
            CS = 0;
            __delay_us(5);
            spi(rd);
            spi(up_ad);
            spi(low_ad);
            *(ptr + j + rx_offset) = spi(0x00); //Get data
            total_data++;
            __delay_us(5);
            CS = 1;
        }
    } else {
        for (j=0;j<rx_size;j++) {  //continuous reading
            hold = rx_address + j;
            low_ad = hold & 0x00FF; //lower byte
            hold = hold >> 8;   //shift right
            up_ad = hold & 0x00FF;  //upper byte
            CS = 0;
            __delay_us(5);
            spi(rd);
            spi(up_ad);
            spi(low_ad);
            *(ptr + j) = spi(0x00); //Get data
            total_data++;
            __delay_us(5);
            CS = 1;
        }
    }
    rx_pointer += rx_size;
    hold = rx_pointer;
    low_ad = hold & 0x00FF; //lower byte
    hold = hold >> 8;   //shift right
    up_ad = hold & 0x00FF;  //upper byte
    CS = 0;
    __delay_us(5);
    spi(wr);
    spi(skt0);
    spi(rx_rp);
    spi(up_ad); //Increase read pointer
    __delay_us(5);
    CS = 1;
    __delay_us(10);
    CS = 0;
    __delay_us(5);
    spi(wr);
    spi(skt0);
    spi(rx_rp + 1);
    spi(low_ad); //Increase read pointer
    __delay_us(5);
    CS = 1;
    __delay_us(10);
    CS = 0;
    spi(wr);
    spi(skt0);
    spi(cmd);
    spi(0x40); //Issue RECV command
    __delay_us(5);
    CS = 1;
    __delay_us(10);
    CS = 0;
    __delay_us(5);
    spi(wr);
    spi(skt0);
    spi(s_int);
    spi(0x04); //Clear receive interrupt
    __delay_us(5);
    CS = 1;
    
    return total_data;
}

byte send_eth_data (char *ptr, byte length) {
    word tx_size, tx_offset, tx_pointer, tx_address, hold, split_end;
    byte up_ad, low_ad, j;
    
    CS = 0;
    __delay_us(5);
    spi(rd);
    spi(skt0);
    spi(tx_free);
    tx_size = spi(0x00); //Get free size upper byte
    __delay_us(5);
    CS = 1;
    __delay_us(5);
    tx_size = tx_size << 8; //shift left one byte
    CS = 0;
    __delay_us(5);
    spi(rd);
    spi(skt0);
    spi(tx_free + 1);
    tx_size += spi(0x00); //Get free size lower byte
    __delay_us(5);
    CS = 1;
    __delay_us(10);
    
    if (tx_size < length) {
        return 0;   //could not transmit due to lack of space
    }
    
    CS = 0;
    __delay_us(5);
    spi(rd);
    spi(skt0);
    spi(tx_rp);
    tx_pointer = spi(0x00); //Get write pointer upper byte
    __delay_us(5);
    CS = 1;
    __delay_us(5);
    tx_pointer = tx_pointer << 8; //shift left one byte
    CS = 0;
    __delay_us(5);
    spi(rd);
    spi(skt0);
    spi(tx_rp + 1);
    tx_pointer += spi(0x00); //Get transmission pointer lower byte
    __delay_us(5);
    CS = 1;
    
    tx_offset = tx_pointer & mask;
    tx_address = tx_base + tx_offset;    //This is the address to start writing
    
    if ((tx_address + length) >= 0x4800) {  //split read
        tx_offset = 0x4800 - tx_address;    //this is the offset to continue
        for (j=0;j<tx_offset;j++) {  //write up to 0x400
            hold = tx_address + j;
            low_ad = hold & 0x00FF; //lower byte
            hold = hold >> 8;   //shift right
            up_ad = hold & 0x00FF;  //upper byte
            CS = 0;
            __delay_us(5);
            spi(wr);
            spi(up_ad);
            spi(low_ad);
            spi(*(ptr+j)); //write data
            __delay_us(5);
            CS = 1;
        }
        split_end = length - tx_offset;
        tx_address = tx_base;
        for (j=0;j<split_end;j++) {  //write remaining
            hold = tx_address + j;
            low_ad = hold & 0x00FF; //lower byte
            hold = hold >> 8;   //shift right
            up_ad = hold & 0x00FF;  //upper byte
            CS = 0;
            __delay_us(5);
            spi(wr);
            spi(up_ad);
            spi(low_ad);
            spi(*(ptr+j+tx_offset)); //Get data
            __delay_us(5);
            CS = 1;
        }
    } else {
        for (j=0;j<length;j++) {  //continuous writing
            hold = tx_address + j;
            low_ad = hold & 0x00FF; //lower byte
            hold = hold >> 8;   //shift right
            up_ad = hold & 0x00FF;  //upper byte
            CS = 0;
            __delay_us(5);
            spi(wr);
            spi(up_ad);
            spi(low_ad);
            spi(*(ptr+j)); //Get data
            __delay_us(5);
            CS = 1;
        }
    }
        
    tx_pointer += length;
    hold = tx_pointer;
    low_ad = hold & 0x00FF; //lower byte
    hold = hold >> 8;   //shift right
    up_ad = hold & 0x00FF;  //upper byte
    CS = 0;
    __delay_us(5);
    spi(wr);
    spi(skt0);
    spi(tx_wr);
    spi(up_ad); //Increase write pointer
    __delay_us(5);
    CS = 1;
    __delay_us(10);
    CS = 0;
    __delay_us(5);
    spi(wr);
    spi(skt0);
    spi(tx_wr + 1);
    spi(low_ad); //Increase write pointer
    __delay_us(5);
    CS = 1;
    __delay_us(10);
    CS = 0;
    __delay_us(5);
    spi(wr);
    spi(skt0);
    spi(cmd);
    spi(0x20); //Issue SEND command
    __delay_us(5);
    CS = 1;
    
    return 1;
}

void socket_fsm (sock_state *socket_status) {
    byte vector;
    
    switch (*socket_status) {
        case sck_disabled: // does nothing
            break;
        case sck_open: // open TCP port
            CS = 0;
            __delay_us(5);
            spi(wr);
            spi(skt0);
            spi(cmd);
            spi(0x01); // Open
            __delay_us(5);
            CS = 1;
            break;
        case sck_listen: // listen for a connection
            CS = 0;
            __delay_us(5);
            spi(wr);
            spi(skt0);
            spi(cmd);
            spi(0x02); // Listen
            __delay_us(5);
            CS = 1;
            break;
        case sck_established: // this is the established status, 'inter' register must be polled
            vector = check_int();
            if (vector & 0x04) {    // data received
                *socket_status = sck_data_ready;
            }
            if (vector & 0x10) {    // Data sent ok
                CS = 0;
                __delay_us(5);
                spi(wr);
                spi(skt0);
                spi(s_int);
                spi(0x10); // Clear send_ok interrupt
                __delay_us(5);
                CS = 1;
            }
            if (vector & 0x08) {
                *socket_status = sck_close;
            }
            break;
        case sck_data_ready:    // data must be read
            
            break;
        case sck_post_send: // post sending status
            vector = check_int();
            if (vector & 0x10) {    // Data sent ok
                CS = 0;
                __delay_us(5);
                spi(wr);
                spi(skt0);
                spi(s_int);
                spi(0x10); // Clear send_ok interrupt
                __delay_us(5);
                CS = 1;
                *socket_status = sck_established;
            }
            break;
        case sck_close:
            CS = 0;
            __delay_us(5);
            spi(wr);
            spi(skt0);
            spi(cmd);
            spi(0x10); // Close
            __delay_us(5);
            CS = 1;
            break;
    } 
}

void send_keep() {
     CS = 0;
    __delay_us(5);
    spi(wr);
    spi(skt0);
    spi(cmd);
    spi(0x22); // issue SEND_KEEP command
    __delay_us(5);
    CS = 1;
}