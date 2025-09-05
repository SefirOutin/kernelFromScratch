#include "kernel.h"

void putnbr(uint16_t n, size_t baseLen)
{
    if (n >= baseLen)
        putnbr(n / baseLen, baseLen);
    serialPutc(BASE[n % baseLen]);
}

int kernel_main(uint32_t *s)
{
	serialInit();
    uint32_t structLenght = *s;
    // putnbr(1820, 16);
    // putnbr(*s, DECBASE);
    // serial_write("\n");
    // serial_write("---fixed part---\n");
    // putnbr(*s, DECBASE);
    // serial_putc('\n');
    // putnbr(*(s + 1), DECBASE);
    // serial_putc('\n');
    // serial_write("--------------\n");
    
    // uint32_t i = 8;
    // while (1)
    // {
    //     uint32_t type = *(s + i);
    //     uint32_t tagSize = *(s + i + 4);
        
    // }
    uint8_t *tmp = (uint8_t *)s;
    for (uint32_t i = 0; i < structLenght; i++)
    {
        putnbr(*(tmp + i), 16);
        serialWrite(" ");
        if (!(i % 10) && i > 5)
            serialPutc('\n');
    }
    while (1);
}
