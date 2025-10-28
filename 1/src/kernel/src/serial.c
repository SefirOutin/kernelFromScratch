#include "serial.h"

void serialInit(void)
{
    outb(COM1 + 1, 0x00);      // IER: disable interrupts
    outb(COM1 + 3, 0x80);      // LCR: enable DLAB
    outb(COM1 + 0, 0x01);      // DLL: divisor low (115200 / 1 = 115200)
    outb(COM1 + 1, 0x00);      // DLM: divisor high
    outb(COM1 + 3, 0x03);      // LCR: 8 bits, no parity, 1 stop, DLAB=0
    outb(COM1 + 2, 0xC7);      // FCR: enable FIFO, clear, 14-byte threshold
    outb(COM1 + 4, 0x0B);      // MCR: OUT2|RTS|DTR (OUT2 enables IRQ on PC hw; harmless here)
}

int serialCanTx(void)
{
    // LSR bit 5 (0x20) = THR empty
    return inb(COM1 + 5) & 0x20;
}

k_uint8_t serialPutc(char c)
{
    while (!serialCanTx()) { /* spin */ }
    outb(COM1 + 0, (unsigned char)c);
    return (1);
}

unsigned int serialWrite(const char* s, unsigned int len)
{
    size_t i = 0;
    for (; *s; ++s, i++)
    {
        if (*s == '\n') serialPutc('\r'); // CRLF for comfy terminals
        serialPutc(*s);
    }
    return (i);
}
