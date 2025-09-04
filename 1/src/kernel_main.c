// #define COM1 0x3F8 + 0x00

// typedef struct s_struct
// {

// }			t_struct;

#include <stdint.h>

// i386 port I/O
static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
// (*(volatile uint8_t *)(mem_addr))
#define COM1 0x3F8

void serial_init(void) {
    outb(COM1 + 1, 0x00);      // IER: disable interrupts
    outb(COM1 + 3, 0x80);      // LCR: enable DLAB
    outb(COM1 + 0, 0x01);      // DLL: divisor low (115200 / 1 = 115200)
    outb(COM1 + 1, 0x00);      // DLM: divisor high
    outb(COM1 + 3, 0x03);      // LCR: 8 bits, no parity, 1 stop, DLAB=0
    outb(COM1 + 2, 0xC7);      // FCR: enable FIFO, clear, 14-byte threshold
    outb(COM1 + 4, 0x0B);      // MCR: OUT2|RTS|DTR (OUT2 enables IRQ on PC hw; harmless here)
}

static int serial_can_tx(void) {
    // LSR bit 5 (0x20) = THR empty
    return inb(COM1 + 5) & 0x20;
}

void serial_putc(char c) {
    while (!serial_can_tx()) { /* spin */ }
    outb(COM1 + 0, (unsigned char)c);
}

void serial_write(const char* s) {
    for (; *s; ++s) {
        if (*s == '\n') serial_putc('\r'); // CRLF for comfy terminals
        serial_putc(*s);
    }
}


void putnbr(uint32_t n)
{
    if (n > 9)
        putnbr(n / 10);
    serial_putc(n % 10 + '0');
}


int kernel_main(void *s)
{
	serial_init();
    uint32_t structLenght = *(uint32_t *)s;
    
    serial_write("---fixed part---\n");
    putnbr(*(uint32_t *)s);
    serial_putc('\n');
    putnbr(*(uint32_t *)(s + 4));
    serial_putc('\n');
    serial_write("--------------\n");
    
    uint32_t i = 8;
    while (1)
    {
        uint32_t type = *(uint32_t *)s + i;
        uint32_t tagSize = *(uint32_t *)s + i + 4;
        
    }


}
