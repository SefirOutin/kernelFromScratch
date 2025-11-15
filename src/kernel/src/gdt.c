#include "gdt.h"
#include "lib.h"
#include "tss.h"

void		gdt_set_gate(struct gdt_entry *entry, struct gdt_raw_entry *raw);
extern void	load_gdt(k_uint16_t limit, k_uint32_t base);

void setup_gdt(struct gdt_entry *gdt, struct tss_entry *tss)
{
	struct gdt_raw_entry	raw;

	// Null Segment
	// raw = (struct gdt_raw_entry){ 0, 0, 0, 0 };
	// gdt_set_gate(&gdt[0], &raw);
	memset(gdt, 0, (6 * sizeof(struct gdt_entry)));
	// Kernel Code Segment
	raw = (struct gdt_raw_entry){ 0, 0xFFFFFFFF, 0x9A, 0xCF };
	gdt_set_gate(&gdt[1], &raw);

	// Kernel Data Segment
	raw = (struct gdt_raw_entry){ 0, 0xFFFFFFFF, 0x92, 0xCF };
	gdt_set_gate(&gdt[2], &raw);

	// User Code Segment
	raw = (struct gdt_raw_entry){ 0, 0xFFFFFFFF, 0xFA, 0xCF };
	gdt_set_gate(&gdt[3], &raw);
	
	// User Data Segment
	raw = (struct gdt_raw_entry){ 0, 0xFFFFFFFF, 0xF2, 0xCF };
	gdt_set_gate(&gdt[4], &raw);

	// TSS
	init_tss(tss);
	raw = (struct gdt_raw_entry){ (k_uint32_t)tss, sizeof(*tss) - 1, 0x89, 0 };
	gdt_set_gate(&gdt[5], &raw);

	// struct gdt_entry *h = &gdt[5];
	load_gdt((k_uint16_t)(6 * sizeof(struct gdt_entry) - 1), (k_uint32_t)gdt);
}

void gdt_set_gate(struct gdt_entry *entry, struct gdt_raw_entry *raw)
{
    /* Setup the descriptor base address */
    entry->base_low = (raw->base & 0xFFFF);
    entry->base_middle = (raw->base >> 16) & 0xFF;
    entry->base_high = (raw->base >> 24) & 0xFF;

    /* Setup the descriptor limits */
    entry->limit_low = (raw->limit & 0xFFFF);
    entry->granularity = ((raw->limit >> 16) & 0x0F);

    /* Finally, set up the granularity and access flags */
    entry->granularity |= (raw->granularity & 0xF0);
    entry->access = raw->access;
}