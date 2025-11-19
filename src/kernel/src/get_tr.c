#include "type.h"

//	get task register (selector)
k_uint16_t get_tr()
{
	k_uint16_t tr;
	asm volatile("str %0" : "=r"(tr));
	return tr;
}