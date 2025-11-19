#include "builtins.h"
#include "vga_console.h"

extern struct vga_console vga;

void	clear(void)
{
	vga.clear(&vga);
}