#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "lib.h"

#define LEFTSHIFT 0x12
#define RIGHTSHIFT 0x59
#define SPACEBAR 0x29
#define BACKSPACE 0x66
#define CAPSLOCK 0x58
#define KEYRELEASED0 0xE0
#define KEYRELEASED1 0xF0

struct ps2_driver;

struct keyboard
{
	bool			_isLeftShiftPressed, _isRightShiftPressed, _isCapsLockOn;
	int 			_set;
	const k_uint8_t	*_set_table;
	const k_uint8_t	*_shifted_set_table;

	char	(*handle_scancode)(struct keyboard *self, struct ps2_driver *ps2, k_uint8_t scancode);
	char	(*translate)(struct keyboard *self, k_uint8_t scancode);

};

void	keyboard_constructor(struct keyboard *self, int set);


#endif