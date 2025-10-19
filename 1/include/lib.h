#ifndef LIB_H
#define LIB_H

#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "serialDebug.h"

#define bool unsigned int
#define true 1
#define false 0
#define BASE "0123456789ABCDEF"

// i386 port I/O
static inline void outb(unsigned short port, unsigned char val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline unsigned char inb(unsigned short port)
{
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
size_t	strlen(const char *str);
char	*strchr(const char *s, int c);
void	*memset(void *p, int v, size_t size);


int	printf(const char *str, ...);


#endif