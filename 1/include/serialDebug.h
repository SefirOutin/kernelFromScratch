#ifndef SERIALDEBUG_H
#define SERIALDEBUG_H

#include "lib.h"

#define COM1 0x3F8

void		serialInit(void);
int			serialCanTx(void);
void		serialPutc(char c);
void		serialWrite(const char* s);

#endif