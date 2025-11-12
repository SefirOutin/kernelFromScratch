#ifndef _LIB_H_
#define _LIB_H_

#include <stdarg.h>

#include "type.h"
#include "serial.h"

#define BIT(x) (1U << (x))

size_t	strlen(const char *str);
char	*strchr(const char *s, int c);
bool	isalpha(char c);
int     isprint(int c);

void            outb(unsigned short port, unsigned char val);
unsigned char   inb(unsigned short port);

void	*memset(void *p, int v, size_t size);
void	*memcpy(void *dest, const void *src, size_t size);


int     printf(const char *str, ...);

#endif