#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "lib.h"

static inline void	sendChar(uint8_t c);
void	initPs2();
uint8_t	getChar();


#endif