#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "lib.h"

#define LEFTSHIFT 0x12
#define RIGHTSHIFT 0x59
#define SPACEBAR 0x29
#define BACKSPACE 0x66
#define CAPSLOCK 0x58
#define LEFTALT 0x11
#define KEYRELEASED0 0xE0
#define KEYRELEASED1 0xF0

struct ps2_driver;

struct keyboard
{
	// Attributs
	int 			_set;
	const k_uint8_t	*_set_table;
	const k_uint8_t	*_shifted_set_table;

	bool			_is_left_shift_pressed, _is_right_shift_pressed;
	bool			_is_caps_lock_on;
	bool			_is_alt_pressed;

	// Methods
	char	(*handle_scancode)(struct keyboard *self, struct ps2_driver *ps2, k_uint8_t scancode);
	char	(*translate)(struct keyboard *self, k_uint8_t scancode);

};

void	keyboard_constructor(struct keyboard *self, int set);


#endif