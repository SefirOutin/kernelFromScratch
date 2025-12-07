#include "builtins.h"

void halt(void)
{
	__asm__ volatile ("hlt");
}