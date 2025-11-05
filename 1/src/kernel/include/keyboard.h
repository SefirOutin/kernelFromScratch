#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "lib.h"

#define KEYRELEASED 0xF0

#define LEFTSHIFT 0x12
#define RIGHTSHIFT 0x59
#define SPACEBAR 0x29
#define BACKSPACE 0x66
#define CAPSLOCK 0x58
#define LEFTALT 0x11

#define KEYPAD1 0x69
#define KEYPAD2 0x72
#define KEYPAD3 0x7A

#define EXTENDEDKEY 0xE0

#define CURSORLEFT 0x6B
#define CURSORDOWN 0x72
#define CURSORRIGHT 0x74
#define CURSORUP 0x75


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