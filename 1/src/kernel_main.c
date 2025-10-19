#include "kernel.h"

#define VgaWidth 80

#define ATKBD_KEYMAP_SIZE       512

static const unsigned short atkbd_set2_keycode[ATKBD_KEYMAP_SIZE] = {
        0, 67, 65, 63, 61, 59, 60, 88,  0, 68, 66, 64, 62, 15, 41,117,
        0, 56, 42, 93, 29, 16,  2,  0,  0,  0, 44, 31, 30, 17,  3,  0,
        0, 46, 45, 32, 18,  5,  4, 95,  0, 57, 47, 33, 20, 19,  6,183,
        0, 49, 48, 35, 34, 21,  7,184,  0,  0, 50, 36, 22,  8,  9,185,
        0, 51, 37, 23, 24, 11, 10,  0,  0, 52, 53, 38, 39, 25, 12,  0,
        0, 89, 40,  0, 26, 13,  0,  0, 58, 54, 28, 27,  0, 43,  0, 85,
        0, 86, 91, 90, 92,  0, 14, 94,  0, 79,124, 75, 71,121,  0,  0,
       82, 83, 80, 76, 77, 72,  1, 69, 87, 78, 81, 74, 55, 73, 70, 99,

        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
      217,100,255,  0, 97,165,  0,  0,156,  0,  0,  0,  0,  0,  0,125,
      173,114,  0,113,  0,  0,  0,126,128,  0,  0,140,  0,  0,  0,127,
      159,  0,115,  0,164,  0,  0,116,158,  0,172,166,  0,  0,  0,142,
      157,  0,  0,  0,  0,  0,  0,  0,155,  0, 98,  0,  0,163,  0,  0,
       226,  0,  0,  0,  0,  0,  0,  0,  0,255, 96,  0,  0,  0,143,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,107,  0,105,102,  0,  0,112,
       110,111,108,112,106,103,  0,119,  0,118,109,  0, 99,104,119,  0,
 
        0,  0,  0, 65, 99,
};

volatile uint16_t *vga = (uint16_t *)0xB8000;


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

// 01110001
// 00100000

int kernel_main(uint32_t *s)
{
    tty term;
    initTerm(&term);
	initPs2();
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
	// inb(PS2Data);
	int i = 0;
	// while (i < 512)
	// {
	// 	printf("0x%X ",  atkbd_set2_keycode[i]);
	// 	if (!(i++ % 15))
	// 		printf("\n");

	// }
	uint8_t chart = getChar();
	printf("char: 0x%X\n", chart);
	term.write(&term, &chart, 1);
    while (1);
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
