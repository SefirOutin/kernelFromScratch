#ifndef _SERIAL_H
#define _SERIAL_H

#include "lib.h"

#define COM1 0x3F8

void			serial_init(void);
int				serial_can_tx(void);
k_uint8_t		serial_putchar(char c);
unsigned int	serial_write(const char* s, unsigned int len);

#endif