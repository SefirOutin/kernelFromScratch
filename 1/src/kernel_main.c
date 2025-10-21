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
    int i = 0;
    unsigned int cursor = term->yPos * VgaWidth + term->xPos;

    while(i < len)
    {
        vga[cursor + i] = str[i] | 0xa << 8;
        i++;
    }
    cursor += i;
    term->updateCursor(term, cursor % VgaWidth, cursor / VgaWidth);
    return (i);
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

#define PS2_SET2_TABLE_SIZE 256
const uint8_t ps2_set2_to_ascii[PS2_SET2_TABLE_SIZE] = {
    // 0x00-0x0F
    [0x00] = 0,     [0x01] = 0,     [0x03] = 0,     // F9, unused, F5
    [0x04] = 0,     [0x05] = 0,     [0x06] = 0,     // F3, F1, F2
    [0x07] = 0,     [0x09] = 0,     [0x0A] = 0,     // F12, F10, F8
    [0x0B] = 0,     [0x0C] = 0,     [0x0D] = '\t',  // F6, F4, Tab
    [0x0E] = '`',   [0x0F] = 0,                     // `

    // 0x10-0x1F
    [0x11] = 0,     [0x12] = 0,     [0x14] = 0,     // Alt, L Shift, L Ctrl
    [0x15] = 'q',   [0x16] = '1',   [0x1A] = 'z',   // q, 1, z
    [0x1B] = 's',   [0x1C] = 'a',   [0x1D] = 'w',   // s, a, w
    [0x1E] = '2',   [0x1F] = 0,                     // 2

    // 0x20-0x2F
    [0x21] = 'c',   [0x22] = 'x',   [0x23] = 'd',   // c, x, d
    [0x24] = 'e',   [0x25] = '4',   [0x26] = '3',   // e, 4, 3
    [0x27] = 0,     [0x2A] = 'v',   [0x2B] = 'f',   // unused, v, f
    [0x2C] = 't',   [0x2D] = 'r',   [0x2E] = '5',   // t, r, 5

    // 0x30-0x3F
    [0x31] = 'n',   [0x32] = 'b',   [0x33] = 'h',   // n, b, h
    [0x34] = 'g',   [0x35] = 'y',   [0x36] = '6',   // g, y, 6
    [0x3A] = 'm',   [0x3B] = 'j',   [0x3C] = 'u',   // m, j, u
    [0x3D] = '7',   [0x3E] = '8',   [0x3F] = 0,     // 7, 8

    // 0x40-0x4F
    [0x41] = ',',   [0x42] = 'k',   [0x43] = 'i',   // ,, k, i
    [0x44] = 'o',   [0x45] = '0',   [0x46] = '9',   // o, 0, 9
    [0x49] = '.',   [0x4A] = '/',   [0x4B] = 'l',   // ., /, l
    [0x4C] = ';',   [0x4D] = 'p',   [0x4E] = '-',   // ;, p, -
    [0x4F] = 0,                                 // unused

    // 0x50-0x5F
    [0x52] = '\'',  [0x54] = '[',   [0x55] = '=',   // ', [, =
    [0x5A] = '\n',  [0x5B] = ']',   [0x5D] = '\\',  // Enter, ], \
    [0x5F] = 0,                                 // unused

    // 0x60-0x6F
    [0x66] = '\b',  [0x69] = 0,     [0x6B] = 0,     // Backspace, End, Left
    [0x6C] = 0,                                 // Home
    [0x6F] = 0,                                 // unused

    // 0x70-0x7F
    [0x70] = 0,     [0x71] = 0,     [0x72] = 0,     // Ins, Del, Down
    [0x74] = 0,     [0x75] = 0,     [0x76] = 27,    // Right, Up, Esc (ASCII 27)
    [0x77] = 0,     [0x79] = 0,     [0x7A] = 0,     // Num Lock, +, PgDn
    [0x7B] = 0,     [0x7C] = 0,     [0x7D] = 0,     // -, *, PgUp
    [0x7E] = 0,     [0x7F] = 0,                 // Scroll Lock, unused

    // 0x80-0xFF: All unused
    // ...
};
    tty term;
    initTerm(&term);
	initPs2();

    term.write(&term, "KERNEL", 6);
    term.updateCursor(&term, 0, 1);
    unsigned char chart;
    while (1)
    {
        chart = getChar();
        printf("char: %X ", chart);
        if (chart == 0xF0)
            { getChar(); printf("\n"); continue;}
        if (chart > 0x84)
            {printf("\n"); continue;}
        chart = ps2_set2_to_ascii[chart];
        printf("trans char: 0x%X\n", chart);
        if (chart)
            term.write(&term, &chart, 1);
    }
}
