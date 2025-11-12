#include "vga_console.h"
#include "lib.h"
#include "vga_buffer.h"

#define CURR_BUFFER self->buffer[self->current_buffer]

static void vga_putchar(struct vga_console *self, char c);
static void vga_clear(struct vga_console *self);
static void vga_set_color(struct vga_console *self, k_uint8_t color);
static void vga_set_cursor(struct vga_console *self, k_uint8_t row, k_uint8_t col);
static void vga_scroll(struct vga_console *self);
static void vga_set_buffer(struct vga_console *self, int new_active_screen);
static void	vga_flush(struct vga_console *self);
static void vga_delchar(struct vga_console *self);

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
    self->delchar = vga_delchar;

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
        return;
    }

    const int index = self->row * VGA_WIDTH + self->col;
    self->hdw_buf[index] = vga_entry(c, self->color);
	CURR_BUFFER->set_byte(CURR_BUFFER, index, c);

    self->col++;
    if (self->col >= VGA_WIDTH)
    {
        self->col = 0;
        self->row++;
        if (self->row >= VGA_HEIGHT)
            self->scroll(self);
    }
}

static void vga_delchar(struct vga_console *self)
{
    size_t index;
    
    if (!self->col)
    {
		if (!self->row)
			return;
		self->col = VGA_WIDTH - 1;
		self->row--;
		index = self->row * VGA_WIDTH + self->col;
        while (index && (!isprint(CURR_BUFFER->buffer[index - 1])))
    		index--;
        self->col = index % VGA_WIDTH;
        self->row = index / VGA_WIDTH;
        return;
    }

    index = self->row * VGA_WIDTH + self->col;
    self->col--;
	CURR_BUFFER->buffer[index - 1] = 0;
    self->hdw_buf[index - 1] = vga_entry(' ', self->color);
}

static void vga_clear(struct vga_console *self)
{
    size_t limit = VGA_HEIGHT * VGA_WIDTH;

    for (size_t i = 0; i < limit; i++)
        self->hdw_buf[i] = vga_entry(0, self->color);
    
    self->row = 0;
    self->col = 0;
}

static void vga_set_color(struct vga_console *self, k_uint8_t color)
{
    self->color = color;
	self->flush(self);
}

static void vga_set_cursor(struct vga_console *self, k_uint8_t row, k_uint8_t col)
{
    self->row = row;
    self->col = col;

    CURR_BUFFER->set_cursor(CURR_BUFFER, self->row, self->col);

    k_uint16_t pos = self->row * VGA_WIDTH + self->col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (k_uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (k_uint8_t)((pos >> 8) & 0xFF));
}

static void vga_scroll(struct vga_console *self)
{
    for (k_uint16_t i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++)
        CURR_BUFFER->buffer[i] = CURR_BUFFER->buffer[i + VGA_WIDTH];

    for (k_uint16_t x = 0; x < VGA_WIDTH; x++)
    	CURR_BUFFER->buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', self->color);

    self->row = VGA_HEIGHT - 1;
    self->col = 0;
	self->flush(self);
}

static void	vga_flush(struct vga_console *self)
{
	unsigned char	c;
	size_t			limit = VGA_HEIGHT * VGA_WIDTH;

	for (size_t i = 0; i < limit; i++)
    {
		c = CURR_BUFFER->buffer[i];
		self->hdw_buf[i] = vga_entry(c ? c : ' ', self->color);
    }
}

static void    vga_set_buffer(struct vga_console *self, int new_active_screen)
{
	k_uint16_t	cursor;

    if (new_active_screen < 0 || new_active_screen > 1)
        { printf("Error changing buffer screen\n"); return; }

    memset((void *)self->hdw_buf, 0, VGA_HEIGHT * VGA_WIDTH * 2);

	self->current_buffer = new_active_screen;
	cursor = CURR_BUFFER->get_cursor(CURR_BUFFER);
    self->set_cursor(self, (k_uint8_t)cursor, (k_uint8_t)(cursor >> 8));
	
    self->flush(self);
}
