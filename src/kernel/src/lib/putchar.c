#include "lib.h"
#include "vga_console.h"

extern struct vga_console vga;

void putchar(int c)
{
	vga.putchar(&vga, c);
	vga.set_cursor(&vga, vga.row, vga.col);
}