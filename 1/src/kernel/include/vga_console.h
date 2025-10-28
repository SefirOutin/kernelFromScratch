#ifndef _VGA_CONSOLE_H
#define _VGA_CONSOLE_H

#include "type.h"

#define VGA_ADDR 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

struct vga_console
{
    /* Attributs */
    volatile k_uint16_t *buf;
    k_uint8_t           color;
    char                row;
    char                col;

    /* Méthodes "internes" (pointeurs de fonctions) */
    void (*putchar)(struct vga_console *self, char c);
    void (*clear)(struct vga_console *self);
    void (*set_color)(struct vga_console *self, k_uint8_t color);
    void (*set_cursor)(struct vga_console *self, k_uint8_t row, k_uint8_t col);
    void (*scroll)(struct vga_console *self);
};

/* Fonctions d'initialisation/destruction */
void vga_console_constructor(struct vga_console *self);

#endif /* _VGA_CONSOLE_H */
