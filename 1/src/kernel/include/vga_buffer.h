#ifndef _VGA_BUFFER_H 
#define _VGA_BUFFER_H

#include "type.h"
#include "vga_console.h"

#define BUFFERSIZE 9600

struct vga_buffer
{
	// Attributs
	unsigned char	row, col;
	/*
		_buffer is embedded in the struct
		Instances of this sctruct will be large
	*/
	unsigned char	buffer[VGA_WIDTH * VGA_HEIGHT];
	

	// Methods
	void	(*dup)(struct vga_buffer *self, struct vga_buffer *src);
	size_t	(*get_size)(struct vga_buffer *self);
	void	(*set_cursor)(struct vga_buffer *self, int row, int col);
	void	(*set_byte)(struct vga_buffer *self, size_t index, unsigned char byte);
	int		(*get_cursor)(struct vga_buffer *self);
};

void	vga_buffer_constructor(struct vga_buffer *self);

#endif