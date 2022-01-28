/* EEPROM start address of parameters */
#define gateway_ad  0x0100
#define subnet_ad   0x0104
#define MAC_ad      0x0108
#define IP_ad       0x010E
#define port_ad     0x0112

/* Socket states */
typedef enum socket_states {
    sck_disabled,
    sck_open,
    sck_listen,
    sck_established,
    sck_data_ready,
    sck_post_send,
    sck_close,
} sock_state;

/* Write/Read opcodes */
#define wr 0xF0
#define rd 0x0F

/* Common registers */
#define gateway 0x01
#define subnet  0x05
#define s_mac   0x09
#define s_ip    0x0F
#define imr     0x16
#define com_ir  0x15
#define rx_mem  0x1A

/* Socket register address banks */
#define skt0 0x04
#define skt1 0x05
#define skt2 0x06
#define skt3 0x07

/* Socket n registers */
/* Tx/Rx mask 0x07FF */
#define mode    0x00    //mode
#define cmd     0x01    //command
#define s_int   0x02    //interrupt
#define stat    0x03    //status
#define s_port  0x04    //source port (1st of 2)
#define d_mac   0x06    //destination mac (1st of 6)
#define d_ip    0x0C    //destination ip (1st of 4)
#define d_port  0x10    //destination port (1st of 2)
#define max_seg 0x12    //maximum segment size --> 0x05B4 = 1460 default
#define tx_free 0x20    //Tx free size (2B)
#define tx_rp   0x22    //Tx read pointer (2B)
#define tx_wr   0x24    //Tx write pointer (2B)
#define rx_rcv  0x26    //Rx received size (2B)
#define rx_rp   0x28    //Rx read pointer (2B)
#define mask    0x07FF  //address related definitions
#define tx_base 0x4000
#define tx_top  0x4800
#define rx_base 0x6000
#define rx_top  0x6800

/* Function prototypes */

#ifndef byte
    #define byte unsigned char
#endif

#ifndef word
    #define word unsigned int
#endif

/**
 * @Summary
 *      Initialize module with saved MAC, IP, gateway and port
 */
void boot_ethernet();

/**
 * @Summary
 *      Sets the MAC address, it takes effect on next boot
 * @param ptr   Pointer to 6 byte long array
 */
void set_mac (byte *ptr);

/**
 * @Summary
 *      Sets the gateway IP, it takes effect on next boot
 * @param ptr   Pointer to 4 byte long array
 */
void set_gateway (byte *ptr);

/**
 * @Summary
 *      Sets the subnet mask, it takes effect on next boot
 * @param ptr   Pointer to 4 byte long array
 */
void set_subnet_mask (byte *ptr);

/**
 * @Summary
 *      Sets the device IP, it takes effect on next boot
 * @param ptr   Pointer to 4 byte long array
 */
void set_ip (byte *ptr);

/**
 * @Summary
 *      Gets the device current IP
 * @param ptr   Pointer to 4 byte long array
 */
void get_ip (byte *ptr);

/**
 * @Summary
 *      Sets the TCP port, it takes effect on next boot
 * @param ptr   Pointer to 2 byte long array
 */
void set_tcp_port (byte *ptr);

/**
 * @Summary
 *      Sends 'length' bytes through TCP to the specified port 
 * @param ptr       Pointer to data
 * @param length    Amount of bytes to send
 */
byte send_eth_data (char *ptr, byte length);

/**
 * @Summary
 *      Gets the data received
 * @param ptr   Pointer to the buffer where data must be copied to
* @return       The amount of data received
 */
word get_eth_data (byte *ptr);

/**
 * @Summary
 *      Returns the interrupt vector status
 * @return  Interrupt vector
 */
byte check_int ();

/**
 * @Summary
 *      This FSM manage the socket behavior
 * @param socket_status Pointer to status global variable
 */
void socket_fsm (sock_state *socket_status);

/**
 * @Summary
 *      Reads the status byte 
 * @return The status vector
 */
byte get_status();

/**
 * @Summary
 *      This function manages the socket life cycle.
 *      It must be implemented in the main file
 * @param socket_status Pointer to status global variable
 */
void socket (sock_state *socket_status);

/**
 * @Summary
 *      This function serves as a simple test for the module, it should return
 *      0x55 that is the default configuration, otherwise, something is wrong
 * @return  The content of the memory distribution register
 */
byte memoria ();

/**
 * @Summary
 *      Checks the connection, if timeout occurs after sending this command then
 *      the close command must be issued
 */
void send_keep();