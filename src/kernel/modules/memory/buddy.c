#include "type.h"

#define MAX_ORDER 10

typedef struct page_list
{
	k_uint32_t			addr;
	k_uint8_t			infos;
	struct page_list	*next;
} page_list_t;

typedef struct buddy_free_lists
{
	page_list_t	*head;

} buddy_free_lists_t;

static inline void *aligned_addr(void *addr, k_uint32_t align)
{
	return ((void *)(((k_uint32_t)addr + align - 1) & ~(align - 1)));
}

extern k_uint32_t _kernel_end; // defined in linker script

	// printf("kernel end: %p\n", aligned_addr(&_kernel_end, 4K));

void	init_page_allocator(k_uint32_t start_usable, k_uint32_t len)
{
	k_uint32_t	page_states_start, page_states_end;

	page_states_start = aligned_addr(&_kernel_end, 4096);

	
}