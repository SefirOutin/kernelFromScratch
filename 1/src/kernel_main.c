#include "kernel.h"

#define VgaWidth 80

volatile uint16_t *vga = (char *)0xB8000;

typedef struct  terminal
{
    void    (*updateCursor) (struct terminal *term, int x, int y);
    void    (*updatePos) (struct terminal *term, int x, int y);
    int     (*write) (struct terminal *term, const char *str, uint32_t len);

    int     xPos;
    int     yPos;
}                   tty;


void enableCursor(tty *term, uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void updateCursor(tty *term, int x, int y)
{
    term->xPos = x;
    term->yPos = y;

	uint16_t pos = y * VgaWidth + x;
	
    outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}

int write(tty *term, const char *str, uint32_t len)
{
    uint32_t j = 0;
    uint32_t cursor = term->yPos * VgaWidth + term->xPos;

    while(j < len)
    {
        vga[cursor + j] = str[j] | 0xa << 8;
        j++;
    }
    cursor += j;
    term->updateCursor(term, cursor % VgaWidth, cursor / VgaWidth);
    return (j);
}

void    initTerm(tty *term)
{
    memset(term, 0, sizeof(term));
    // term->enableCursor = enableCursor;
    term->updateCursor = updateCursor;
    term->write = write;
}

int kernel_main(uint32_t *s)
{
    tty term;
    initTerm(&term);
    printf("x: %d y: %d\n", term.xPos, term.yPos);
    term.write(&term, "oui", 3);
    term.write(&term, "oui", 3);
    term.write(&term, "oui", 3);
    term.write(&term, "oui", 3);
    term.write(&term, "oui", 3);
    term.write(&term, "oui", 3);
    term.write(&term, "oui", 3);
    term.write(&term, "oui", 3);
    term.write(&term, "oui", 3);
    term.write(&term, "oui", 3);
    term.write(&term, "oui", 3);
    term.write(&term, "oui", 3);
    term.write(&term, "oui", 3);
    // term.updateCursor(&term, 1, 0);
    // char *vga = (char *)0xB8000;  // adresse mémoire VGA texte
    // vga[0] = '4';                 // premier caractère
    // vga[1] = 0x07;                // couleur gris clair sur fond noir
    // vga[2] = '2';                 // second caractère
    // vga[3] = 0x07;                // même couleur
    // outb(0x3D4, 0x0A);
    // outb(0x3D5, 0x20);
    // enable_cursor(14, 15);
    // update_cursor(50, 1);
    while (1);                     // boucle infinie pour garder l’écran affiché
        
}
