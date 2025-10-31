#include "vga_buffer.h"
#include "lib.h"

void	dup(struct vga_buffer *self, struct vga_buffer *src);
void	set_cursor(struct vga_buffer *self, int row, int col);
void	set_byte(struct vga_buffer *self, size_t index, unsigned char byte);
k_uint16_t		get_cursor(struct vga_buffer *self);

void	vga_buffer_constructor(struct vga_buffer *self)
{
	memset(self, 0, sizeof(struct vga_buffer));
	
	self->dup = dup;
	self->get_cursor = get_cursor;
	// self->get_col = get_col;
	// self->get_row = get_row;
	self->set_cursor = set_cursor;
	self->set_byte = set_byte;
}

// size_t	get_size(struct vga_buffer *self)
// {
// 	return (self->_size);
// }

void	set_byte(struct vga_buffer *self, size_t index, unsigned char byte)
{
	self->buffer[index] = byte;
}

void	set_cursor(struct vga_buffer *self, int row, int col)
{
	self->row = row;
	self->col = col;
}

k_uint16_t		get_cursor(struct vga_buffer *self)
{
	return (self->row | self->col << 8);
}

// int		get_row(struct vga_buffer *self)
// {
// 	return (self->row);
// }

// int		get_col(struct vga_buffer *self)
// {
// 	return (self->col);	
// }

void	dup(struct vga_buffer *self, struct vga_buffer *src)
{
	memcpy(self, src, sizeof(struct vga_buffer));

}
