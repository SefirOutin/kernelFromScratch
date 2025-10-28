#ifndef SERIAL_H
#define SERIAL_H

#include "lib.h"

#define COM1 0x3F8

void			serialInit(void);
int				serialCanTx(void);
k_uint8_t		serialPutc(char c);
unsigned int	serialWrite(const char* s, unsigned int len);

#endif