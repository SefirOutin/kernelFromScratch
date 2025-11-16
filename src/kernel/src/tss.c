#include "tss.h"
#include "lib.h"

#define KERNELSTACKSIZE 4096

extern k_uint8_t kernel_stack[];

void	init_tss(struct tss_entry *entry)
{
	memset(entry, 0, sizeof(*entry));

	entry->esp0 = (k_uint32_t)(kernel_stack + KERNELSTACKSIZE - 4);
	entry->ss0 = 2 << 3;
}