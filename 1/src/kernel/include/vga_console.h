#ifndef _VGA_CONSOLE_H
#define _VGA_CONSOLE_H

#include "type.h"

#define VGA_ADDR 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

struct vga_console
{
    // Attributs
    volatile k_uint16_t	*hdw_buf;
    struct vga_buffer	*buffer[5];

    k_uint8_t			color;
    unsigned char		row;
    unsigned char		col;
	int					current_buffer;


    // Methods
    void    (*putchar)(struct vga_console *self, char c);
    void    (*clear)(struct vga_console *self);
    void    (*set_color)(struct vga_console *self, k_uint8_t color);
    void    (*set_cursor)(struct vga_console *self, k_uint8_t row, k_uint8_t col);
    void    (*scroll)(struct vga_console *self);
    void    (*set_buffer)(struct vga_console *self, int new_active_screen);
	void	(*flush)(struct vga_console *self);
    void    (*delchar)(struct vga_console *self);
};

void vga_console_constructor(struct vga_console *self);

#endif /* _VGA_CONSOLE_H */
