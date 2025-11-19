#ifndef _LIB_H_
#define _LIB_H_

#include <stdarg.h>

#include "type.h"
#include "serial.h"

#define BIT(x) (1U << (x))

bool	isalpha(char c);
int     isprint(int c);


size_t	strlen(const char *str);
char	*strchr(const char *s, int c);
int		strncmp(const char *s1, const char *s2, size_t n);
int		strcmp(const char *s1, const char *s2);


void		outb(k_uint16_t port, k_uint8_t val);
k_uint8_t	inb(k_uint16_t port);
k_uint16_t 	get_tr();

void	*memset(void *p, int v, size_t size);
void	*memcpy(void *dest, const void *src, size_t size);

void	putchar(int c);
void	putstr(const char *str);

int     printf(const char *str, ...);
int		vprintf(const char *str, va_list args);

#endif