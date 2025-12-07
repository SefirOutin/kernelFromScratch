#include "lib.h"

inline void outb(k_uint16_t port, k_uint8_t val)
{
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}
