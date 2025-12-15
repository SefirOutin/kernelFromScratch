#include "buddy.h"
#include "lib.h"

extern k_uint32_t _kernel_end; // defined in linker script
void	*get_page(buddy_allocator_t *self);
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

void	update_pages(buddy_allocator_t *self, size_t page_idx, int order)
{
	int	block_size = 1 << order;		// max 1024 pages

	printf("order: %d\n", order);
	for (int i = 0; i < block_size; i++)
		self->page_info[page_idx - self->first_free_page + i].state = (k_uint8_t)order << 2;	// set order bits
	
	self->page_info[page_idx - self->first_free_page].state |= 1 << 1;			// set head of block bit
}

void	*new_block(buddy_allocator_t *self, size_t page_idx, int order)
{
	free_block_t	*new;

	if (page_idx < self->first_free_page || page_idx > self->last_free_page)
		return (NULL);

	new = (free_block_t *)page_idx_to_addr(page_idx);
	new->next = NULL;

	update_pages(self, page_idx, order);

	return (new);
}

void	add_block(buddy_allocator_t *self, size_t page_idx, int order)
{
	free_block_t	*new, *tmp;
	
	new = new_block(self, page_idx, order);
	if (!new)
		return ;

	if (!self->order[order])
		self->order[order] = new;

	else
	{
		tmp = self->order[order];
		while (tmp->next != NULL)
			tmp = tmp->next;
	
		tmp->next = new;
	}
}

#define USE_BIT (1)
#define HEAD_OF_BLOCK_BIT (1 << 1)
#define ORDER_BITS(order) (order << 2)
#define BLOCK_SIZE(order) (1 << order)

bool	enough_remaining_space(buddy_allocator_t *self, size_t page_idx, int order)
{
	int need = 1 << BLOCK_SIZE(order);
	for (int i = 0; i < need; i++)
		if (self->page_info[page_idx - self->first_free_page + i].state & HEAD_OF_BLOCK_BIT)
			return (1);
	return (0);
}
/*
	max 4MB blocks;

*/
int	init_buddy_allocator(buddy_allocator_t *self, struct multiboot_mmap_entry *mmap_entry)
{
	size_t	page_states_end;
	size_t	current_page, next_page;
	int		order;
	
	self->page_info = (page_descriptor_t *)aligned_addr(&_kernel_end, 4096);
	self->size_page_info = mmap_entry->len / 4096 * sizeof(page_descriptor_t);
	page_states_end = ((size_t)self->page_info + self->size_page_info) / PAGE_SIZE;
	self->first_free_page = mmap_entry->addr / PAGE_SIZE;
	self->last_free_page = (mmap_entry->addr + mmap_entry->len) / PAGE_SIZE;

	memset(self->page_info, 0, self->size_page_info);
	memset(self->order, 0, sizeof(free_block_t *) * MAX_ORDER);

	current_page = self->first_free_page;
	while (current_page < page_states_end)								// mark kernel memory as used
	{
		self->page_info[current_page++ - self->first_free_page].state = 1;
	}
	order = MAX_ORDER;
	while (current_page < self->last_free_page)
	{
		if (self->page_info[current_page - self->first_free_page].state & 1)		// check use/free bit to skip kernel
			{ current_page++; continue; }

		printf("current page: %u\n", current_page);
		if (is_aligned(current_page, order)
			&& current_page + BLOCK_SIZE(order) <= self->last_free_page)
		{
			add_block(self, current_page, order);
			current_page += BLOCK_SIZE(order);
			order = MAX_ORDER;
		}
		else
			order--;
	}

	print_buddy_metadata(self);

	printf("start usable RAM: %X\n", mmap_entry->addr);
	printf("end of kernel: %p\n", &_kernel_end);
	printf("start page struct: %p\n", self->page_info);
	// printf("end of   bitmap  : %p\n", page_states_end);
	printf("size of pds:  %u\n", self->size_page_info);
	printf("nb of pages: %u\n", mmap_entry->len / 4096);
	printf("len of usable ram: %u\n", mmap_entry->len);
	printf("total len really to use: %u\n", mmap_entry->len - (page_states_end - mmap_entry->addr));
	printf("available pages are: p%u-p", self->first_free_page);
	printf("%u\n", self->last_free_page);
	printf("order10 bock size: %u\n", BLOCK_SIZE(MAX_ORDER));
	return (0);
}

int	buddy_constructor(buddy_allocator_t *self, struct multiboot_mmap_entry *mmap_entry)
{
	memset(self, 0, sizeof(buddy_allocator_t));

	self->get_page = get_page;

	init_buddy_allocator(self, mmap_entry);
	return (0);
}

void	*get_page(buddy_allocator_t *self)
{
	(void)self;
	return (NULL);
}

void	print_buddy_metadata(buddy_allocator_t *self)
{
	free_block_t	*tmp;

	printf("BUDDY ALLOCATOR\n");
	for (int i = MAX_ORDER; i; i--)
	{
		tmp = self->order[i];
		printf("ORDER%d:\n", i);
		while (tmp)
		{
			printf("%X, size: %u | ", tmp, BLOCK_SIZE((self->page_info[((k_uint32_t)tmp / PAGE_SIZE) - self->first_free_page].state) >> 2));
			tmp = tmp->next;
		}
		printf("\n");
	}

}