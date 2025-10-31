#include "vga_console.h"
#include "lib.h"
#include "vga_buffer.h"

static void vga_putchar(struct vga_console *self, char c);
static void vga_clear(struct vga_console *self);
static void vga_set_color(struct vga_console *self, k_uint8_t color);
static void vga_set_cursor(struct vga_console *self, k_uint8_t row, k_uint8_t col);
static void vga_scroll(struct vga_console *self);
static void vga_set_buffer(struct vga_console *self, int new_active_screen);
static void	vga_flush(struct vga_console *self);

void vga_console_constructor(struct vga_console *self)
{
    memset(self, 0, sizeof(struct vga_console));
    self->hdw_buf = (volatile k_uint16_t *)VGA_ADDR;
    self->color = 0x07;

    self->putchar = vga_putchar;
    self->clear = vga_clear;
    self->set_color = vga_set_color;
    self->set_cursor = vga_set_cursor;
    self->scroll = vga_scroll;
    self->set_buffer = vga_set_buffer;
	self->flush = vga_flush;
    self->clear(self);
}


static inline k_uint16_t vga_entry(char c, k_uint8_t color)
{
    return (k_uint16_t)c | ((k_uint16_t)color << 8);
}

static void vga_putchar(struct vga_console *self, char c)
{
    if (c == '\n')
    {
        self->col = 0;
        self->row++;
        if (self->row >= VGA_HEIGHT)
        self->scroll(self);
        self->buffer[self->current_buffer]->set_cursor(self->buffer[self->current_buffer], self->row, self->col);
        return;
    }

    const int index = self->row * VGA_WIDTH + self->col;
    self->hdw_buf[index] = vga_entry(c, self->color);
	self->buffer[self->current_buffer]->set_byte(self->buffer[self->current_buffer], index, c);

    self->col++;
    if (self->col >= VGA_WIDTH)
    {
        self->col = 0;
        self->row++;
        if (self->row >= VGA_HEIGHT)
            self->scroll(self);
    }
    self->buffer[self->current_buffer]->set_cursor(self->buffer[self->current_buffer], self->row, self->col);

}

static void vga_clear(struct vga_console *self)
{
    for (int y = 0; y < VGA_HEIGHT; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
            self->hdw_buf[y * VGA_WIDTH + x] = vga_entry(' ', self->color);

    self->row = 0;
    self->col = 0;
}

static void vga_set_color(struct vga_console *self, k_uint8_t color)
{
    self->color = color;
}

static void vga_set_cursor(struct vga_console *self, k_uint8_t row, k_uint8_t col)
{
    self->row = row;
    self->col = col;

    self->buffer[self->current_buffer]->set_cursor(self->buffer[self->current_buffer], self->row, self->col);


    k_uint16_t pos = self->row * VGA_WIDTH + self->col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (k_uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (k_uint8_t)((pos >> 8) & 0xFF));
}

static void vga_scroll(struct vga_console *self)
{
    for (k_uint16_t i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++)
        self->hdw_buf[i] = self->hdw_buf[i + VGA_WIDTH];

    for (k_uint16_t x = 0; x < VGA_WIDTH; x++)
    	self->hdw_buf[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', self->color);

    self->row = VGA_HEIGHT - 1;
    self->col = 0;
    self->buffer[self->current_buffer]->set_cursor(self->buffer[self->current_buffer], self->row, self->col);


}

static void	vga_flush(struct vga_console *self)
{
	size_t	limit = VGA_HEIGHT * VGA_HEIGHT;

	for (size_t i = 0; i < limit; i++)
		self->hdw_buf[i] = vga_entry(self->buffer[self->current_buffer]->buffer[i], self->color);
	
}

static void    vga_set_buffer(struct vga_console *self, int new_active_screen)
{
	k_uint16_t	cursor;

    memset((void *)self->hdw_buf, 0, VGA_HEIGHT * VGA_WIDTH);
	cursor = self->buffer[self->current_buffer]->get_cursor(self->buffer[self->current_buffer]);
	printf("cursor x: %d y: %d\n", (k_uint8_t)cursor, (k_uint8_t)cursor >> 8);
	self->current_buffer = new_active_screen;
	self->flush(self);
	self->set_cursor(self, cursor, cursor >> 8);
}