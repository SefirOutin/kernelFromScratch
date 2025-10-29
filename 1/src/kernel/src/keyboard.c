#include "keyboard.h"
#include "ps2_driver.h"

char translate(struct keyboard *self, k_uint8_t scancode);
char handle_keyboard_scancode(struct keyboard *self, struct ps2_driver *ps2_driver, k_uint8_t scancode);


#define PS2_SET2_TABLE_SIZE 132
static const k_uint8_t ps2_set2_to_ascii[PS2_SET2_TABLE_SIZE] = {
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

static const k_uint8_t ps2_set2_to_ascii_shift[PS2_SET2_TABLE_SIZE] = {
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


void	keyboard_constructor(struct keyboard *self, int set)
{
	memset(self, 0, sizeof(struct keyboard));

	self->_set = set;
	self->_set_table = ps2_set2_to_ascii;
	self->_shifted_set_table = ps2_set2_to_ascii_shift;

	self->translate = translate;
	self->handle_scancode = handle_keyboard_scancode;
}


char translate(struct keyboard *self, k_uint8_t scancode)
{
	char character;

	if (scancode > 131 || !scancode)
		return (0);

	if (self->_isLeftShiftPressed | self->_isRightShiftPressed)
		character = self->_shifted_set_table[scancode];
	else
		character = self->_set_table[scancode];

	if (self->_isCapsLockOn && is_alpha(character))
		return (character ^ bit(5));
	return (character);
}

char handle_keyboard_scancode(struct keyboard *self, struct ps2_driver *ps2_driver, k_uint8_t scancode)
{
	bool isReleased = false;

	if (scancode == KEYRELEASED0) {
		isReleased = true;
		scancode = ps2_driver->read_byte(ps2_driver);
	}
	if (scancode ==  KEYRELEASED1) {
		isReleased = true;
		scancode = ps2_driver->read_byte(ps2_driver);
	}
	switch (scancode)
	{
		case LEFTSHIFT:
			self->_isLeftShiftPressed = isReleased ? false : true;
			return (0);
		case RIGHTSHIFT:
			self->_isRightShiftPressed = isReleased ? false : true;
			return (0);
		case SPACEBAR:
			return (' ');
		case BACKSPACE:
			return (isReleased ? 0 : '\b');
		case CAPSLOCK:
			self->_isCapsLockOn = isReleased ? self->_isCapsLockOn : self->_isCapsLockOn ^ bit(0);

	}
	if (isReleased)
		return (0);
	return (self->translate(self, scancode));
}