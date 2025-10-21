#include "lib.h"

#define PS2Data 0x60
#define PS2StatusCmd 0x64
#define bit(x) (1U << (x))

#define ATKBD_KEYMAP_SIZE       512

static const unsigned short atkbd_set2_keycode[ATKBD_KEYMAP_SIZE] = {
        0, 67, 65, 63, 61, 59, 60, 88,  0, 68, 66, 64, 62, 15, 41,117,
        0, 56, 42, 93, 29, 16,  2,  0,  0,  0, 44, 31, 30, 17,  3,  0,
        0, 46, 45, 32, 18,  5,  4, 95,  0, 57, 47, 33, 20, 19,  6,183,
        0, 49, 48, 35, 34, 21,  7,184,  0,  0, 50, 36, 22,  8,  9,185,
        0, 51, 37, 23, 24, 11, 10,  0,  0, 52, 53, 38, 39, 25, 12,  0,
        0, 89, 40,  0, 26, 13,  0,  0, 58, 54, 28, 27,  0, 43,  0, 85,
        0, 86, 91, 90, 92,  0, 14, 94,  0, 79,124, 75, 71,121,  0,  0,
       82, 83, 80, 76, 77, 72,  1, 69, 87, 78, 81, 74, 55, 73, 70, 99,

        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      217,100,255,  0, 97,165,  0,  0,156,  0,  0,  0,  0,  0,  0,125,
      173,114,  0,113,  0,  0,  0,126,128,  0,  0,140,  0,  0,  0,127,
      159,  0,115,  0,164,  0,  0,116,158,  0,172,166,  0,  0,  0,142,
      157,  0,  0,  0,  0,  0,  0,  0,155,  0, 98,  0,  0,163,  0,  0,
       226,  0,  0,  0,  0,  0,  0,  0,  0,255, 96,  0,  0,  0,143,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,107,  0,105,102,  0,  0,112,
       110,111,108,112,106,103,  0,119,  0,118,109,  0, 99,104,119,  0,
 
        0,  0,  0, 65, 99,
};

uint8_t	getChar()
{
	uint8_t data;
	while (!(inb(PS2StatusCmd) & bit(0)));
		// printf("hmm: %X\n", data);
	data = inb(PS2Data);

	return (data);
}

static inline void	sendChar(uint8_t c)
{
	while (inb(PS2StatusCmd) & bit(1));		// Wait until input buffer ready
	outb(PS2Data, c);
}

void	initPs2()
{
	bool				dualChannel = false;
	volatile uint8_t	controllerConf;
	
	outb(PS2StatusCmd, 0xAD);				// Disable 1st device
	outb(PS2StatusCmd, 0xA7);				// Disable 2nd device if it exists
	inb(PS2Data);							// Flush controller output buffer 

	outb(PS2StatusCmd, 0x20);				// Tell to read controller config
	controllerConf = getChar();					// read controller config
	controllerConf &= ~(bit(6) | bit(4) | bit(0));	// 1st port: disable int, translation, enable port clock
	outb(PS2StatusCmd, 0x60);				// Tell to write controller config
	sendChar(controllerConf);				// Send new config
	
	// outb(PS2StatusCmd, 0x20);				// Tell to read controller config
	// printf("conf byte1: %b\n", getChar());

	outb(PS2StatusCmd, 0xAA);				// Perform controller self test
	printf("con test: %s\n", getChar() == 0x55 ? "pass" : "fail");	// check result
	
	outb(PS2StatusCmd, 0xA8);				// Test dual channel
	outb(PS2StatusCmd, 0x20);				// Tell to read controller config
	if (!(getChar() & bit(5)))				// bit 5 must be clear
	{
		printf("dual test: %s\n", dualChannel ? "dual" : "mono");
		dualChannel = true;
		outb(PS2StatusCmd, 0xA7);			// Disable 2nd device if it exists
		controllerConf &= ~(bit(5) | bit(1));		// 2nd port: disable int, enable port clock
		outb(PS2StatusCmd, 0x60);				// Tell to write controller config
		sendChar(controllerConf);
	}
	
	outb(PS2StatusCmd, 0xAB);				// Test PS/2 1st port
	if (getChar() == 0)
	{
		outb(PS2StatusCmd, 0xAE);			// Enable PS/2 1st port
		sendChar(0xFF);						// reset device
		int i = 0;
		while (i++ < 3)
			printf("1st getChar: %X ", getChar());
		printf("\n");
		sendChar(0xF5);							// disable scan 1st dev
		getChar();								// ACK
	}
	if (dualChannel)
	{
		outb(PS2StatusCmd, 0xA9);				// Test PS/2 2nd port
		if (getChar() == 0)
		{
			outb(PS2StatusCmd, 0xA8);			// Enable PS/2 2nd port
			outb(PS2StatusCmd, 0xD4);			// Enable write to 2nd port
			sendChar(0xFF);						// reset device
			int i = 0;
			while (i++ < 3)
				printf("2nd getChar: %X ", getChar());
			printf("\n");
		}
		outb(PS2StatusCmd, 0xD4);				
		sendChar(0xF5);							// Disable scan 2nd dev	
		getChar();								// ACK
		
	}
	// sendChar(0xF0);				// get-set scancode set CMD
	// printf("ACK scancodeSet: %X\n", getChar());
	// sendChar(0x00);				// get scan code set
	// printf("ACK: %X\n", getChar());
	// printf("code set: %X\n", getChar());

	// First Device ID

	sendChar(0xF2);				// identify yourself
	printf("ACK identity: %X\n", getChar());
	uint8_t c = getChar();
	if (c >= 0xAB)
		printf("dev1 ID: %X %X\n", c, getChar());
	else
		printf("dev1 ID: %X\n", c);
	sendChar(0xF4);				// enable scanning

	// Second Device ID

	outb(PS2StatusCmd, 0xD4);
	sendChar(0xF2);				// identify yourself
	printf("ACK identity: %X\n", getChar());
	uint8_t c1 = getChar();
	if (c1 >= 0xAB)
		printf("dev2 ID: %X %X\n", c1, getChar());
	else
		printf("dev2 ID: %X\n", c1);
	
	
	sendChar(0xF4);				// enable scan 1st dev
	outb(PS2StatusCmd, 0xD4);
	sendChar(0xF4);				// enable scan 2nd dev

}