#include "lib.h"

inline k_uint8_t inb(k_uint16_t port)
{
    k_uint8_t ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
