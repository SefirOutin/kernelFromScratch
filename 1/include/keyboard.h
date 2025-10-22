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


#define PS2Data 0x60
#define PS2StatusCmd 0x64
#define bit(x) (1U << (x))

static inline void	sendChar(uint8_t c);
void	initPs2();
uint8_t	getChar();

char handle_keyboard_scancode(unsigned char scancode);

#endif