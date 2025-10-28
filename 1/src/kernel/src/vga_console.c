#include "vga_console.h"
#include "lib.h"

static void vga_putchar(struct vga_console *self, char c);
static void vga_clear(struct vga_console *self);
static void vga_set_color(struct vga_console *self, k_uint8_t color);
static void vga_set_cursor(struct vga_console *self, k_uint8_t row, k_uint8_t col);
static void vga_scroll(struct vga_console *self);

void vga_console_constructor(struct vga_console *self)
{
    self->buf = (volatile k_uint16_t *)VGA_ADDR;
    self->color = 0x07;
    self->row = 0;
    self->col = 0;

    self->putchar = vga_putchar;
    self->clear = vga_clear;
    self->set_color = vga_set_color;
    self->set_cursor = vga_set_cursor;
    self->scroll = vga_scroll;

    self->clear(self);
}

// fonction utilitaire (inline)
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
    self->buf[index] = vga_entry(c, self->color);

    self->col++;
    if (self->col >= VGA_WIDTH)
    {
        self->col = 0;
        self->row++;
        if (self->row >= VGA_HEIGHT)
            self->scroll(self);
    }
}

static void vga_clear(struct vga_console *self)
{
    for (int y = 0; y < VGA_HEIGHT; y++)
        for (int x = 0; x < VGA_WIDTH; x++)
            self->buf[y * VGA_WIDTH + x] = vga_entry(' ', self->color);

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

    k_uint16_t pos = self->row * VGA_WIDTH + self->col;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (k_uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (k_uint8_t)((pos >> 8) & 0xFF));
}

static void vga_scroll(struct vga_console *self)
{
    for (k_uint16_t i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++)
        self->buf[i] = self->buf[i + VGA_WIDTH];

    for (k_uint16_t x = 0; x < VGA_WIDTH; x++)
        self->buf[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', self->color);

    self->row = VGA_HEIGHT - 1;
    self->col = 0;
}
