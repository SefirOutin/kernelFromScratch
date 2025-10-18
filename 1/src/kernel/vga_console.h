#ifndef _VGA_CONSOLE_H
#define _VGA_CONSOLE_H

#include "console.h"
#include "type.h"   /* pour uintX_t */

#define VGA_ADDR    0xB8000
#define VGA_WIDTH   80
#define VGA_HEIGHT  25

/*
 * struct vga_console - implémentation de la console texte VGA.
 * 
 * Cette structure représente une console basée sur le framebuffer VGA
 * en mode texte. Le premier membre 'base' permet le polymorphisme avec
 * struct console. Le champ 'buf' pointe directement vers la mémoire
 * mappée à l'adresse 0xB8000 (hardware).
 */
struct vga_console {
	struct console base;         /* interface commune (héritage) */
	volatile uint16_t *buf;      /* framebuffer matériel (16 bits exacts) */
	uint8_t color;               /* attribut couleur VGA (4 bits fg/bg) */
	uint8_t row;                 /* ligne courante (0–24) */
	uint8_t col;                 /* colonne courante (0–79) */
};

/*
 * Fonctions publiques spécifiques à l'implémentation VGA.
 */

struct vga_console *vga_console_create(void);
void vga_console_destroy(struct vga_console *self);
void vga_console_set_color(struct vga_console *self, uint8_t color);
#endif /* _VGA_CONSOLE_H */
