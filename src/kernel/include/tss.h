#ifndef _TSS_H_
#define _TSS_H_

#include "type.h"

struct tss_entry {
    k_uint32_t prev_tss;   // Previous TSS (if hardware task switching is used)
    k_uint32_t esp0;       // Stack pointer for ring 0
    k_uint32_t ss0;        // Stack segment for ring 0
    k_uint32_t esp1;       // Stack pointer for ring 1
    k_uint32_t ss1;        // Stack segment for ring 1
    k_uint32_t esp2;       // Stack pointer for ring 2
    k_uint32_t ss2;        // Stack segment for ring 2
    k_uint32_t cr3;        // Page directory base register
    k_uint32_t eip;        // Instruction pointer
    k_uint32_t eflags;     // Flags register
    k_uint32_t eax;        // General purpose registers
    k_uint32_t ecx;
    k_uint32_t edx;
    k_uint32_t ebx;
    k_uint32_t esp;        // Stack pointer
    k_uint32_t ebp;        // Base pointer
    k_uint32_t esi;        // Source index
    k_uint32_t edi;        // Destination index
    k_uint32_t es;         // Segment selectors
    k_uint32_t cs;
    k_uint32_t ss;
    k_uint32_t ds;
    k_uint32_t fs;
    k_uint32_t gs;
    k_uint32_t ldt;        // Local descriptor table selector
    k_uint16_t trap;       // Trap on task switch
    k_uint16_t iomap_base; // I/O map base address
} __attribute__((packed));

void	init_tss(struct tss_entry *entry);


#endif