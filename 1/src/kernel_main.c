#include "kernel.h"

#define VgaWidth 80

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
    tty term;
    initTerm(&term);
	initPs2();

    printf("isalpha: %d\n", is_alpha('A'));
    term.write(&term, "KERNEL", 6);
    term.updateCursor(&term, 0, 1);
    unsigned char chart;
    while (1)
    {
        // chart = getChar();
        // printf("char: %X ", chart);
        // if (chart == 0xF0)
        //     { getChar(); printf("\n"); continue;}
        // if (chart > 0x84)
        //     {printf("\n"); continue;}
        chart = handle_keyboard_scancode(getChar());
        // printf("trans char: 0x%X\n", chart);
        
        if (chart == '\b') {
            term.updateCursor(&term, term.xPos - 1, term.yPos);
            chart = 0;
            term.write(&term, &chart, 1);
            term.updateCursor(&term, term.xPos - 1, term.yPos);
        }
        if (chart)
            term.write(&term, &chart, 1);
    }
}
