char msjCALIBRATE[16] = "CALIBRATE      ";
char msjZERO[16] = "ZERO           ";
char msjDIVISIONS[16] = "DIVISIONS      ";
char msjPATTERN[16] = "PATTERN        ";
char msjFILTER[16] = "FILTER         ";
char msjSTEP[16] = "STEP           ";

char msjSERIAL[16] = "SERIAL         ";
char msjBAUD[16] = "BAUD           ";

char msjETHERNET[16] = "ETHERNET       ";
char msjGATEWAY[16] = "GATEWAY        ";
char msjSUBNETMASK[16] = "SUBNET MASK    ";
char msjMAC[16] = "MAC            ";
char msjIP[16] = "IP             ";
char msjPORT[16] = "PORT           ";

char msjPASSWORD[16] = "PASSWORD       ";

char msjRESET[16] = "RESET          ";

char msjFINISH[16] = "FINISH         ";
char msjCONFIRM[16] = "CONFIRM        ";
char msjABORT[16] = "ABORT          ";

char msjONOFF[16] = "ON OFF         ";
char msjON[16] = "ON             ";
char msjOFF[16] = "OFF            ";

const unsigned int digitos [11][22] = {
	{0x07E0,0x0FF0,0x1008,0x300C,0x300C,0x300C,0x300C,0x300C,0x300C,0x300C,0x300C,0x300C,0x300C,0x300C,0x300C,0x300C,0x300C,0x300C,0x300C,0x1008,0x0FF0,0x07E0},	// 0
	{0x0780,0x0780,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x0180,0x1FF8,0x1FF8},	// 1
	{0x07E0,0x0FF0,0x1008,0x000C,0x0006,0x0006,0x0006,0x000C,0x001C,0x0038,0x0070,0x00E0,0x01C0,0x0380,0x0700,0x0E00,0x1C00,0x1800,0x3000,0x3000,0x3FFE,0x3FFE},	// 2
	{0x1FF0,0x3FF8,0x601C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x0018,0x07F0,0x07F0,0x0018,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x601C,0x3FF8,0x1FF0},	// 3
	{0x0000,0x000C,0x001C,0x003C,0x006C,0x00CC,0x018C,0x030C,0x060C,0x0C0C,0x180C,0x300C,0x3FFE,0x3FFE,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C},	// 4
	{0x7FF8,0x7FF8,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x7FF0,0x7FF8,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x000C,0x7FF8,0x7FF0},	// 5
	{0x0FF0,0x1FF0,0x3000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6000,0x6FF0,0x7FF8,0x700C,0x600C,0x600C,0x600C,0x600C,0x600C,0x600C,0x600C,0x3FF8,0x1FF0},	// 6
	{0x3FFC,0x3FFC,0x000C,0x000C,0x0018,0x0018,0x0030,0x0030,0x0060,0x0060,0x00C0,0x00C0,0x0180,0x0180,0x0300,0x0300,0x0600,0x0600,0x0C00,0x0C00,0x1800,0x1800},	// 7
	{0x07E0,0x0FF0,0x1008,0x300C,0x300C,0x300C,0x300C,0x300C,0x300C,0x1818,0x0FF0,0x0FF0,0x1818,0x300C,0x300C,0x300C,0x300C,0x300C,0x300C,0x1008,0x0FF0,0x07E0},	// 8
	{0x07F8,0x1FFC,0x180C,0x3006,0x3006,0x3006,0x3006,0x3006,0x3006,0x180E,0x1FFE,0x07F6,0x0006,0x0006,0x0006,0x0006,0x0006,0x0006,0x000C,0x0018,0x0FF0,0x0FE0},	// 9
	{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}	// blank
};

const char texto[43][8] = {
	{0x00, 0x7E, 0xA1, 0x91, 0x89, 0x85, 0x7E, 0x00},  // Code for char 0
	{0x00, 0x00, 0x82, 0xFF, 0x80, 0x00, 0x00, 0x00},  // Code for char 1
	{0x00, 0xC2, 0xA1, 0x91, 0x89, 0x86, 0x00, 0x00},  // Code for char 2
	{0x00, 0x00, 0x81, 0x81, 0x89, 0x95, 0x63, 0x00},  // Code for char 3
	{0x00, 0x00, 0x18, 0x14, 0x12, 0xFF, 0x10, 0x00},  // Code for char 4
	{0x00, 0x8F, 0x89, 0x89, 0x89, 0x89, 0x70, 0x00},  // Code for char 5
	{0x00, 0x7E, 0x89, 0x89, 0x89, 0x89, 0x70, 0x00},  // Code for char 6
	{0x00, 0x01, 0xC1, 0x31, 0x09, 0x05, 0x03, 0x00},  // Code for char 7
	{0x00, 0x76, 0x89, 0x89, 0x89, 0x76, 0x00, 0x00},  // Code for char 8
	{0x00, 0x0E, 0x91, 0x91, 0x91, 0x7E, 0x00, 0x00},  // Code for char 9
	{0x00, 0x00, 0x00, 0xCC, 0xCC, 0x00, 0x00, 0x00},  // Code for char :	10
	{0x00, 0x00, 0x48, 0x48, 0x48, 0x48, 0x00, 0x00},  // Code for char =	11
	{0x00, 0xFC, 0x12, 0x11, 0x12, 0xFC, 0x00, 0x00},  // Code for char A	12
	{0x00, 0xFF, 0x89, 0x89, 0x89, 0x76, 0x00, 0x00},  // Code for char B	13
	{0x00, 0x7E, 0x81, 0x81, 0x81, 0x81, 0x00, 0x00},  // Code for char C	14
	{0x00, 0xFF, 0x81, 0x81, 0x81, 0x81, 0x7E, 0x00},  // Code for char D	15
	{0x00, 0xFF, 0x89, 0x89, 0x89, 0x81, 0x00, 0x00},  // Code for char E	16
	{0x00, 0xFF, 0x09, 0x09, 0x09, 0x01, 0x00, 0x00},  // Code for char F	17
	{0x00, 0x7E, 0x81, 0x91, 0x91, 0xF0, 0x00, 0x00},  // Code for char G	18
	{0x00, 0xFF, 0x10, 0x10, 0x10, 0xFF, 0x00, 0x00},  // Code for char H	19
	{0x00, 0x00, 0x81, 0xFF, 0x81, 0x00, 0x00, 0x00},  // Code for char I	20
	{0x00, 0x41, 0x81, 0x7F, 0x01, 0x00, 0x00, 0x00},  // Code for char J	21
	{0x00, 0xFF, 0x18, 0x24, 0x42, 0x80, 0x00, 0x00},  // Code for char K	22
	{0x00, 0xFF, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00},  // Code for char L	23
	{0xFF, 0x02, 0x04, 0x08, 0x04, 0x02, 0xFF, 0x00},  // Code for char M	24
	{0x00, 0xFF, 0x06, 0x08, 0x10, 0x60, 0xFF, 0x00},  // Code for char N	25
	{0x00, 0x7E, 0x81, 0x81, 0x81, 0x81, 0x7E, 0x00},  // Code for char O	26
	{0x00, 0xFF, 0x11, 0x11, 0x11, 0x0E, 0x00, 0x00},  // Code for char P	27
	{0x00, 0x7E, 0x81, 0xA1, 0xC1, 0x7E, 0x00, 0x00},  // Code for char Q	28
	{0x00, 0xFF, 0x31, 0x31, 0x51, 0x8E, 0x00, 0x00},  // Code for char R	29
	{0x00, 0x86, 0x89, 0x89, 0x89, 0x71, 0x00, 0x00},  // Code for char S	30
	{0x01, 0x01, 0x01, 0xFF, 0x01, 0x01, 0x01, 0x00},  // Code for char T	31
	{0x00, 0x7F, 0x80, 0x80, 0x80, 0x80, 0x7F, 0x00},  // Code for char U	32
	{0x00, 0x3F, 0x40, 0x80, 0x80, 0x40, 0x3F, 0x00},  // Code for char V	33
	{0x1E, 0x60, 0xC0, 0x30, 0x30, 0xC0, 0x60, 0x1E},  // Code for char W	34
	{0x00, 0x84, 0x48, 0x30, 0x30, 0x48, 0x84, 0x00},  // Code for char X	35
	{0x01, 0x02, 0x04, 0xF8, 0x04, 0x02, 0x01, 0x00},  // Code for char Y	36
	{0x00, 0xC1, 0xA1, 0x91, 0x89, 0x85, 0x83, 0x00},	// Code for char Z	37
	{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}	// Code for space	38
};