#include "tss.h"
#include "lib.h"

#define KERNELSTACKSIZE 4096
extern k_uint8_t kernel_stack[];

extern void dump_stack(void);

void	init_tss(struct tss_entry *entry)
{
	memset(entry, 0, sizeof(*entry));

	// Top of kernel stack for this user task
	entry->esp0 = (k_uint32_t)(kernel_stack + KERNELSTACKSIZE - 4);
	// selector for kernel data segment
	entry->ss0 = 2 << 3;
}