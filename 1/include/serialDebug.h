#ifndef SERIALDEBUG_H
#define SERIALDEBUG_H

#include "lib.h"

#define COM1 0x3F8

void		serialInit(void);
int			serialCanTx(void);
uint8_t		serialPutc(char c);
uint32_t	serialWrite(const char* s, uint32_t len);

#endif