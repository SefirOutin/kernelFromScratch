#ifndef _GDT_H_
#define _GDT_H_

#include "type.h"

struct gdt_raw_entry
{
	k_uint32_t	base;
	k_uint32_t	limit;
	k_uint8_t	access;
	k_uint8_t	granularity;
};

struct	gdt_entry
{
    k_uint16_t limit_low;
    k_uint16_t base_low;
    k_uint8_t base_middle;
    k_uint8_t access;
    k_uint8_t granularity;
    k_uint8_t base_high;
}__attribute__((packed));


void setup_gdt(struct gdt_entry *gdt);

#endif