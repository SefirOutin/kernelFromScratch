#include "video.h"

int	vgaInit()
{
	// inb(0x3DA);							// attribute reg, flipflop reset
	
	// 
	// uint8_t input = inb(VGA_GC_INDEX);
	// outb(VGA_GC_INDEX, 6);
	// uint8_t tmp = inb(VGA_GC_DATA);
	// outb(VGA_GC_DATA, 0x1);
	// outb(VGA_GC_INDEX, input);
	return (0);
}