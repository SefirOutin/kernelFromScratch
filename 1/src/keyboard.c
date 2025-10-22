#include "lib.h"
#include <keyboard.h>

static bool	isLeftShiftPressed = false, isRightShiftPressed = false, isCapsLockOn = false;

#define PS2_SET2_TABLE_SIZE 132
const uint8_t ps2_set2_to_ascii[PS2_SET2_TABLE_SIZE] = {
    [0x0D] = '\t',  [0x0E] = '`',
    [0x15] = 'q',   [0x16] = '1',
    [0x1A] = 'z',   [0x1B] = 's',   [0x1C] = 'a',   [0x1D] = 'w',   [0x1E] = '2',
    [0x21] = 'c',   [0x22] = 'x',   [0x23] = 'd',   [0x24] = 'e',   [0x25] = '4',   [0x26] = '3',
    [0x2A] = 'v',   [0x2B] = 'f',   [0x2C] = 't',   [0x2D] = 'r',   [0x2E] = '5',
    [0x31] = 'n',   [0x32] = 'b',   [0x33] = 'h',   [0x34] = 'g',   [0x35] = 'y',   [0x36] = '6',
    [0x3A] = 'm',   [0x3B] = 'j',   [0x3C] = 'u',   [0x3D] = '7',   [0x3E] = '8',
    [0x41] = ',',   [0x42] = 'k',   [0x43] = 'i',   [0x44] = 'o',   [0x45] = '0',   [0x46] = '9',
    [0x49] = '.',   [0x4A] = '/',   [0x4B] = 'l',   [0x4C] = ';',   [0x4D] = 'p',   [0x4E] = '-',
    [0x52] = '\'',  [0x54] = '[',   [0x55] = '=',   [0x5A] = '\n',  [0x5B] = ']',   [0x5D] = '\\',
    [0x66] = '\b',  [0x76] = 27 // ESC
};

const uint8_t ps2_set2_to_ascii_shift[PS2_SET2_TABLE_SIZE] = {
    [0x0E] = '~',
    [0x15] = 'Q',   [0x16] = '!',
    [0x1A] = 'Z',   [0x1B] = 'S',   [0x1C] = 'A',   [0x1D] = 'W',   [0x1E] = '@',
    [0x21] = 'C',   [0x22] = 'X',   [0x23] = 'D',   [0x24] = 'E',   [0x25] = '$',   [0x26] = '#',
    [0x2A] = 'V',   [0x2B] = 'F',   [0x2C] = 'T',   [0x2D] = 'R',   [0x2E] = '%',
    [0x31] = 'N',   [0x32] = 'B',   [0x33] = 'H',   [0x34] = 'G',   [0x35] = 'Y',   [0x36] = '^',
    [0x3A] = 'M',   [0x3B] = 'J',   [0x3C] = 'U',   [0x3D] = '&',   [0x3E] = '*',
    [0x41] = '<',   [0x42] = 'K',   [0x43] = 'I',   [0x44] = 'O',   [0x45] = ')',   [0x46] = '(',
    [0x49] = '>',   [0x4A] = '?',   [0x4B] = 'L',   [0x4C] = ':',   [0x4D] = 'P',   [0x4E] = '_',
    [0x52] = '"',   [0x54] = '{',   [0x55] = '+',   [0x5A] = '\n',  [0x5B] = '}',   [0x5D] = '|',
    [0x66] = '\b',  [0x76] = 27
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
		while (i++ < 2)
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
			while (i++ < 2)
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

char translate(unsigned char scancode, bool uppercase, bool capsLock)
{
	char character;

	if (scancode > 131 || !scancode)
		return (0);

	if (uppercase)
		character = ps2_set2_to_ascii_shift[scancode];
	else
		character = ps2_set2_to_ascii[scancode];

	if (capsLock && is_alpha(character))
		return (character ^ bit(5));
	return (character);
}

char handle_keyboard_scancode(unsigned char scancode)
{
	bool isReleased = false;

	if (scancode == KEYRELEASED0) {
		isReleased = true;
		scancode = getChar();
	}
	if (scancode ==  KEYRELEASED1) {
		isReleased = true;
		scancode = getChar();
	}
	switch (scancode)
	{
		case LEFTSHIFT:
			isLeftShiftPressed = isReleased ? false : true;
			return (0);
		case RIGHTSHIFT:
			isRightShiftPressed = isReleased ? false : true;
			return (0);
		case SPACEBAR:
			return (' ');
		case BACKSPACE:
			return (isReleased ? 0 : '\b');
		case CAPSLOCK:
			isCapsLockOn = isReleased ? isCapsLockOn : isCapsLockOn ^ bit(0);

	}
	if (isReleased)
		return (0);
	return (translate(scancode, isLeftShiftPressed | isRightShiftPressed, isCapsLockOn));
}