#include "buddy.h"
#include "lib.h"

#define VIRT_IDX(a) (a - self->first_free_page)
#define GET_FLAG_PAGE_INFO(index) (self->page_info[index].state)
#define FREE_BIT 0
#define USED_BIT 1
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

static inline size_t aligned_idx(size_t page_idx, int order)
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

static inline size_t	addr_to_idx(k_uint32_t addr)
{
	return (addr >> PAGE_SHIFT);
} 

static inline bool	is_head(unsigned char flag)
{
	return (flag & HEAD_OF_BLOCK_BIT);
}

static inline bool	is_used(unsigned char flag)
{
	return (flag & USED_BIT);
}

static inline bool	same_order(unsigned char flag, unsigned int order)
{
	return (((flag >> 2) & 0xF0) == order);
}

static inline unsigned int	parse_order(unsigned char flag)
{
	return ((flag >> 2) & 0xF0);
}

static inline void update_page_info(buddy_allocator_t *self, size_t index, k_uint8_t flag, int order)
{
	size_t	block_size = BLOCK_SIZE(order);

	self->page_info[index].state = flag | HEAD_OF_BLOCK_BIT | ORDER_BITS(order);

	while (block_size)
		GET_FLAG_PAGE_INFO(index + block_size--) = flag;
}


void	*new_block(buddy_allocator_t *self, size_t page_idx, int order)
{
	free_block_t	*new;

	if (page_idx < self->first_free_page || page_idx > self->last_free_page) // check boundaries
		return (NULL);

	new = (free_block_t *)page_idx_to_addr(page_idx);	// the node lives at that addr (1st page of block)
	new->next = NULL;

	update_page_info(self, page_idx, FREE_BIT, order);

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
	free_block_t	*new, *head;
	
	new = new_block(self, page_idx, order);
	if (!new)
		return ;

	if (!self->order[order])	// if no element in list
		self->order[order] = new;

	else
	{
		head = self->order[order];
		last_block(head)->next = new;
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
		self->page_info[VIRT_IDX(current_page++)].state = USED_BIT;
	
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
	index = addr_to_idx((k_uint32_t)tmp);
	tmp = new_block(self, index + BLOCK_SIZE(order), order);
	add_head(self, tmp, order);
	tmp = new_block(self, index, order);
	add_head(self, tmp, order);
}

void	*alloc_pages(buddy_allocator_t *self, unsigned int order)
{
	free_block_t	*new_alloc;
	size_t			index;
	unsigned int 	target_order = order;

	if (order > MAX_ORDER)
		return (NULL);

	// find next available free block
	while (target_order < MAX_ORDER && self->order[target_order] == NULL)
		target_order++;
	
	if (target_order == MAX_ORDER && self->order[target_order] == NULL)
		return (NULL);		// OUT OF MEMORY

	// if suitable block bigger than requested, split until it matches
	while (target_order > order)
		split_block(self, target_order--);

	new_alloc = self->order[order];
	index = VIRT_IDX(addr_to_idx(new_alloc));
	
	self->order[order] = new_alloc->next;		//	remove from free list
	
	update_page_info(self, index, USED_BIT, order);

	memset(new_alloc, 0, sizeof(free_block_t));		// clear data at start of the block REALLY NEEDED ?
	return (new_alloc);
}

free_block_t	*find_block(free_block_t *head, size_t index)
{
	k_uintptr_t	addr = page_idx_to_addr(index);

	if (!head)
		return (NULL);
	
	while (head)
	{
		if (head == addr)
			return (head);
		head = head->next;
	}
	return (NULL);
}

static inline is

void	merge_buddies(buddy_allocator_t *self, free_block_t *freed, unsigned int order)
{
	free_block_t	*new, *buddy;

	buddy = (free_block_t *)page_idx_to_addr(addr_to_idx(freed) ^ BLOCK_SIZE(order));

	if (freed > buddy)
		GET_FLAG_PAGE_INFO(addr_to_idx(freed)) = FREE_BIT;
	else
		GET_FLAG_PAGE_INFO(addr_to_idx(buddy)) = FREE_BIT;
	
	
}

void	free_pages(buddy_allocator_t *self, k_uintptr_t addr)
{
	size_t			buddy_idx, current_idx = VIRT_IDX(addr_to_idx(addr));
	unsigned char	flag = GET_FLAG_PAGE_INFO(current_idx);
	unsigned int	depth = 0, order = parse_order(flag);
	free_block_t	*freed;

	if (!is_head(flag) && !is_used(flag))
		return ;
	

	freed = new_block(self, current_idx, order);
	add_head(self, freed, order);

	buddy_idx = VIRT_IDX(current_idx ^ BLOCK_SIZE(order));
	while (!is_used(GET_FLAG_PAGE_INFO(buddy_idx)) && order <= MAX_ORDER)
	{
		merge_buddies(self, freed, BLOCK_SIZE(order++));
		buddy_idx = VIRT_IDX(current_idx ^ BLOCK_SIZE(order));
	}
	


}

void	print_buddy_metadata(buddy_allocator_t *self)
{
	free_block_t	*tmp;
	k_uintptr_t		*alloc;

	alloc = alloc_pages(self, 0);
	// free_pages(self, alloc);
	printf("BUDDY ALLOCATOR\n");
	for (int i = MAX_ORDER; i >= 0; i--)
	{
		tmp = self->order[i];
		printf("ORDER%d:\n", i);
		while (tmp)
		{
			printf("%p, size: %u | ", tmp, self->page_info[VIRT_IDX(addr_to_idx(tmp))].state >> 2);
			tmp = tmp->next;
		}
		printf("\n");
	}
	
	printf("alloc addr:%p\n", alloc);
	// printf("%d\n", (42 - 1) % 40);
	for (size_t i = 0; i < self->size_page_info; i++)
	{
		if (!i || (!((i - 1) % 10) && i != 1))
			printf("%p: ", page_idx_to_addr(i + self->first_free_page));
		printf("%X ", self->page_info[i].state);
		if (!(i % 10) && i)
			printf("\n");
	}
}
