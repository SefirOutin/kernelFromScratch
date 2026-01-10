#include "buddy.h"
#include "lib.h"

#define VIRT_IDX(a) (a - self->first_free_page)
#define USE_BIT (1)
#define HEAD_OF_BLOCK_BIT (1 << 1)
#define ORDER_BITS(order) (order << 2)
#define BLOCK_SIZE(order) (1 << order)

extern k_uint32_t _kernel_end; // defined in linker script

void	*alloc_pages(buddy_allocator_t *self, unsigned int nb_request);
void	print_buddy_metadata(buddy_allocator_t *self);

void printmem(void *ptr, size_t len)
{
	k_uint8_t	*tmp = ptr;
	k_uint32_t i = 0;
	printf("\n\nPRINT MEM:\n");
	while (i < len)
	{
		printf("%X ", tmp[i]);
		i++;
		if (!(i % 40))
		printf("\n");
	}
}

static inline void *aligned_addr(void *addr, k_uint32_t align)
{
	return ((void *)(((k_uint32_t)addr + align - 1) & ~(align - 1)));
}

static inline int aligned_idx(size_t page_idx, int order)
{
	return (((page_idx + (1 << order) - 1) & ~((1 << order) - 1)));
}

static inline bool	is_aligned(size_t page_idx, int order)
{
	return ((page_idx & ((1 << order) - 1)) == 0);
}

static inline k_uint32_t	page_idx_to_addr(size_t page_idx)
{
	return (page_idx * PAGE_SIZE);
}

static inline unsigned int	addr_to_idx(k_uint32_t addr)
{
	return (addr / PAGE_SIZE);
} 


void	update_pages(buddy_allocator_t *self, size_t page_idx, int order)
{
	int	block_size = 1 << order;		// max 1024 pages

	for (int i = 0; i < block_size; i++)
		self->page_info[VIRT_IDX(page_idx + i)].state = (k_uint8_t)order << 2;	// set order bits
	
	self->page_info[VIRT_IDX(page_idx)].state |= 1 << 1;			// set head of block bit for first page
}

void	*new_block(buddy_allocator_t *self, size_t page_idx, int order)
{
	free_block_t	*new;

	if (page_idx < self->first_free_page || page_idx > self->last_free_page) // check boundaries
		return (NULL);

	new = (free_block_t *)page_idx_to_addr(page_idx);	// the node lives at that addr (1st page of block)
	new->next = NULL;

	update_pages(self, page_idx, order);

	return (new);
}

free_block_t	*last_block(free_block_t *head)
{
	while (head->next)
		head = head->next;
	return (head);
}


void	add_block(buddy_allocator_t *self, size_t page_idx, int order)
{
	free_block_t	*new, *tmp;
	
	new = new_block(self, page_idx, order);
	if (!new)
		return ;

	if (!self->order[order])	// if no element in list
		self->order[order] = new;

	else
	{
		tmp = self->order[order];
		last_block(tmp)->next = new;
	}
}

/*
	max 4MB blocks;

*/
int	init_buddy_allocator(buddy_allocator_t *self, struct multiboot_mmap_entry *mmap_entry)
{
	size_t	current_page, page_info_end;
	int		order;
	
	self->page_info = (page_descriptor_t *)aligned_addr(&_kernel_end, 4096);
	self->size_page_info = mmap_entry->len / 4096 * sizeof(page_descriptor_t);
	page_info_end = addr_to_idx((size_t)self->page_info + self->size_page_info);
	self->first_free_page = addr_to_idx(mmap_entry->addr);
	self->last_free_page = addr_to_idx(mmap_entry->addr + mmap_entry->len);

	memset(self->page_info, 0, self->size_page_info);

	current_page = self->first_free_page;						// begin at start of usable ram
	while (current_page <= page_info_end)						// mark kernel memory as used and skip it
		self->page_info[VIRT_IDX(current_page++)].state = 1;
	
	order = MAX_ORDER;
	while (current_page < self->last_free_page)
	{
		// printf("last page: %u current page: %u aligned: %s blockSize: %u blockEnd: %u\n", self->last_free_page, current_page, is_aligned(current_page, order) ? "true" : "false", BLOCK_SIZE(order), current_page + BLOCK_SIZE(order));
		if (is_aligned(current_page, order)									// check remaining space and
			&& current_page + BLOCK_SIZE(order) <= self->last_free_page)	// page alignement
		{
			add_block(self, current_page, order);
			current_page += BLOCK_SIZE(order);
			order = MAX_ORDER;
		}
		else
			order--;
	}

	print_buddy_metadata(self);

	return (0);
}
// printf("start usable RAM: %X\n", mmap_entry->addr);
// printf("end of kernel: %p\n", &_kernel_end);
// printf("start page struct: %p\n", self->page_info);
// // printf("end of   bitmap  : %p\n", page_states_end);
// printf("size of pds:  %u\n", self->size_page_info);
// printf("nb of pages: %u\n", mmap_entry->len / 4096);
// printf("len of usable ram: %u\n", mmap_entry->len);
// printf("total len really to use: %u\n", mmap_entry->len - (page_states_end - mmap_entry->addr));
// printf("available pages are: p%u-p", self->first_free_page);
// printf("%u\n", self->last_free_page);
// printf("order10 bock size: %u\n", BLOCK_SIZE(MAX_ORDER));

int	buddy_constructor(buddy_allocator_t *self, struct multiboot_mmap_entry *mmap_entry)
{
	memset(self, 0, sizeof(buddy_allocator_t));

	self->alloc_pages = alloc_pages;

	init_buddy_allocator(self, mmap_entry);
	return (0);
}

void	add_head(buddy_allocator_t *self, free_block_t *new, unsigned int order)
{
	if (self->order[order] == NULL)
		self->order[order] = new;
	else
	{
		new->next = self->order[order];
		self->order[order] = new;
	}
}

void	split_block(buddy_allocator_t *self, unsigned int order)
{
	free_block_t	*tmp;
	size_t			index;

	if (order < 1 || order > MAX_ORDER)
		return ;

	tmp = self->order[order];
	self->order[order] = tmp->next;

	order--;
	index = addr_to_idx(tmp);
	tmp = new_block(self, index + (1 << (order)), order);
	add_head(self, tmp, order);
	tmp = new_block(self, index, order);
	add_head(self, tmp, order);
}

void	*alloc_pages(buddy_allocator_t *self, unsigned int order)
{
	unsigned int target_order = order;

	if (order > MAX_ORDER)
		return (NULL);
	while (target_order <= MAX_ORDER && self->order[target_order] == NULL)
		target_order++;
	
	// if (target_order > order)
	// 	split_block
	return (NULL);
}

void	print_buddy_metadata(buddy_allocator_t *self)
{
	free_block_t	*tmp;

	split_block(self, 4);
	printf("BUDDY ALLOCATOR\n");
	for (int i = MAX_ORDER; i >= 0; i--)
	{
		tmp = self->order[i];
		printf("ORDER%d:\n", i);
		while (tmp)
		{
			printf("%p, size: %u | ", tmp, self->page_info[VIRT_IDX((k_uint32_t)tmp / PAGE_SIZE)].state >> 2);
			tmp = tmp->next;
		}
		printf("\n");
	}

}
