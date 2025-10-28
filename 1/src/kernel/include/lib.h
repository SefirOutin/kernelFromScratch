#ifndef LIB_H
#define LIB_H

#include <stdarg.h>

#include "type.h"
#include "serial.h"

#define bit(x) (1U << (x))

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
bool	is_alpha(char c);


int     printf(const char *str, ...);

#endif