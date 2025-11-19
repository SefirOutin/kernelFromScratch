#include "lib.h"
#include "vga_console.h"

extern struct vga_console vga;

void putstr(const char *str)
{
	while (*str)
		vga.putchar(&vga, *str++);
}
