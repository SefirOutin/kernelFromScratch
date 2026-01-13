#include "buddy.h"
#include "lib.h"

// #define VIRT_IDX(a) (a - self->first_free_page)
#define GET_FLAG_PAGE_INFO(index) (self->page_info[index].state)
#define USED_BIT 1
#define HEAD_OF_BLOCK_BIT (1 << 1)
#define ORDER_BITS(order) ((order & 0x0F) << 2)
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

static inline k_uint32_t	page_idx_to_addr(buddy_allocator_t *self, size_t page_idx)
{
	return (((page_idx + self->first_free_page) << PAGE_SHIFT));
}

static inline size_t	addr_to_idx(buddy_allocator_t *self, void *addr)
{
	return (((k_uint32_t)addr >> PAGE_SHIFT) - self->first_free_page);
} 

static inline bool	is_head(unsigned char flag)
{
	return (flag & HEAD_OF_BLOCK_BIT);
}

static inline bool	is_used(unsigned char flag)
{
	return (flag & USED_BIT);
}

static inline bool	same_order(unsigned char current_flag, unsigned char buddy_flag)
{
	return (((current_flag >> 2) & 0x0F) == ((buddy_flag >> 2) & 0x0F));
}

static inline unsigned int	parse_order(unsigned char flag)
{
	return ((flag >> 2) & 0x0F);
}

static inline void update_page_info(buddy_allocator_t *self, size_t index, k_uint8_t flag, int order)
{
	size_t	block_size = BLOCK_SIZE(order) - 1;

	if (flag != USED_BIT)
		flag = 0;

	self->page_info[index].state = flag | HEAD_OF_BLOCK_BIT | ORDER_BITS(order);

	while (block_size)
		self->page_info[index + block_size--].state = flag;
}


void	*new_block(buddy_allocator_t *self, size_t page_idx, int order)
{
	free_block_t	*new;

	if (page_idx < self->first_free_page || page_idx > self->last_free_page) // check boundaries
		return (NULL);

	new = (free_block_t *)page_idx_to_addr(self, page_idx);	// the node lives at that addr (1st page of block)
	new->next = NULL;

	update_page_info(self, page_idx, 0, order);

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

	if (!self->free_area[order].head)	// if no element in list
		self->free_area[order].head = new;

	else
	{
		head = self->free_area[order].head;
		last_block(head)->next = new;
	}
}
// returns first usable page index
size_t	pages_maps_init(buddy_allocator_t *self, struct multiboot_mmap_entry *mmap_entry)
{
	void	*curr_ptr;
	size_t	current_page, page_info_end;

	self->page_info = (page_descriptor_t *)aligned_addr(&_kernel_end, PAGE_SIZE);
	self->total_pages = mmap_entry->len >> PAGE_SHIFT;
	self->size_page_info =  self->total_pages * sizeof(page_descriptor_t);
	curr_ptr = aligned_addr((size_t)self->page_info + self->size_page_info, PAGE_SIZE);
	self->first_free_page = mmap_entry->addr >> PAGE_SHIFT;
	self->last_free_page = (mmap_entry->addr + mmap_entry->len) >> PAGE_SHIFT;
	
	size_t size_map_bits, size_map;
	for (int order = 0; order <= MAX_ORDER; order++)
	{
		size_map_bits = (self->total_pages + (BLOCK_SIZE(order + 1) - 1)) / BLOCK_SIZE(order + 1);
		size_map = (size_map_bits + ((sizeof(*self->free_area->map) * 8) - 1)) / (sizeof(*self->free_area->map) * 8);
		printf("size map[%d]: %d | %dbits | ", order, size_map, size_map_bits);
		self->free_area[order].map = curr_ptr;
		printf("head: %p\n", curr_ptr);
		curr_ptr = aligned_addr(curr_ptr + size_map, sizeof(*self->free_area->map));
	}
	
	memset(self->page_info, 0, self->size_page_info);
	
	

	return (page_info_end);
}

/*
	max 4MB blocks;
*/
int	init_buddy_allocator(buddy_allocator_t *self, struct multiboot_mmap_entry *mmap_entry)
{
	size_t	current_page, page_info_end;
	int		order;
	
	page_info_end = pages_maps_init(self, mmap_entry);

	current_page = self->first_free_page;						// begin at start of usable ram
	while (current_page <= page_info_end)						// mark kernel memory as used and skip it
		self->page_info[current_page++ - self->first_free_page].state = USED_BIT;
	
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

	// print_buddy_metadata(self);

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
	if (self->free_area[order].head == NULL)
		self->free_area[order].head = new;
	else
	{
		new->next = self->free_area[order].head;
		self->free_area[order].head = new;
	}
}

int	split_block(buddy_allocator_t *self, unsigned int target_order)
{
	free_block_t	*to_split, *new_buddy;
	size_t			idx_to_split;
	unsigned int	available_order;

	if (target_order > MAX_ORDER)
		return (1);

	available_order = target_order;
	while (available_order <= MAX_ORDER && self->free_area[available_order].head == NULL)
		available_order++;
	
	if (available_order == MAX_ORDER + 1)
		return (1);		// NO order-PAGE AVAILABLE
	
	to_split = self->free_area[available_order].head;
	self->free_area[available_order].head = to_split->next;
	idx_to_split = addr_to_idx(self, to_split);
	while (available_order-- > target_order)
	{
		new_buddy = new_block(self, idx_to_split ^ BLOCK_SIZE(available_order), available_order);
		if (!new_buddy)
			return (1);
	
		add_head(self, new_buddy, available_order);
	}
	
	self->page_info[addr_to_idx(self, to_split)].state = HEAD_OF_BLOCK_BIT | ORDER_BITS(target_order);
	add_head(self, to_split, target_order);
	return (0);
}

void	*alloc_pages(buddy_allocator_t *self, unsigned int order)
{
	free_block_t	*new_alloc;

	if (order > MAX_ORDER)
		return (NULL);

	if (self->free_area[order].head == NULL)
		split_block(self, order);

	new_alloc = self->free_area[order].head;
	update_page_info(self, addr_to_idx(self, new_alloc), USED_BIT, order);
	memset(new_alloc, 0, sizeof(free_block_t));		// clear data at start of the block REALLY NEEDED ?

	return (new_alloc);
}

free_block_t	*find_block(buddy_allocator_t *self, free_block_t *head, size_t index)
{
	k_uintptr_t	addr = page_idx_to_addr(self, index);

	if (!head)
		return (NULL);
	
	while (head)
	{
		if ((k_uintptr_t)head == addr)
			return (head);
		head = head->next;
	}
	return (NULL);
}

static inline bool	buddy_avail(unsigned char curr_flag, unsigned char buddy_flag)
{
	return ((is_used(buddy_flag) == false)
			& (is_head(buddy_flag) == true)
			& (same_order(curr_flag, buddy_flag) == true));
}

void	merge_buddies(buddy_allocator_t *self, void *to_free, size_t current_idx)
{
	free_block_t	*new, *buddy;
	unsigned char	flag;
	unsigned int	order;

	
	// buddy = (free_block_t *)page_idx_to_addr(self, current_idx ^ BLOCK_SIZE(order));
	
	// while ()
	// {
		
	// }
	// if (freed > buddy)
	// 	GET_FLAG_PAGE_INFO(addr_to_idx(freed)) = FREE_BIT;
	// else
	// 	GET_FLAG_PAGE_INFO(addr_to_idx(buddy)) = FREE_BIT;
	
	
}

void	free_pages(buddy_allocator_t *self, void *to_free)
{
	size_t			current_idx = (addr_to_idx(self, to_free));
	unsigned char	flag = GET_FLAG_PAGE_INFO(current_idx);

	if (!is_head(flag) && !is_used(flag))
		return ;

	merge_buddies(self, to_free, current_idx);

}

void	print_buddy_metadata(buddy_allocator_t *self)
{
	free_block_t	*tmp;
	k_uintptr_t		*alloc;

	alloc = alloc_pages(self, 0);
	// alloc = alloc_pages(self, 4);
	// free_pages(self, alloc);
	printf("BUDDY ALLOCATOR\n");
	for (int i = MAX_ORDER; i >= 0; i--)
	{
		tmp = self->free_area[i].head;
		printf("ORDER%d:\n", i);
		while (tmp)
		{
			printf("%p, size: %u | ", tmp, parse_order(self->page_info[(addr_to_idx(self, tmp))].state));
			tmp = tmp->next;
		}
		printf("\n");
	}
	
	// printf("alloc addr:%p\n", alloc);
	// printf("%d\n", (42 - 1) % 40);
	// for (size_t i = 0; i < self->size_page_info; i++)
	// {
	// 	if (!i || (!((i - 1) % 10) && i != 1))
	// 		printf("%p: ", page_idx_to_addr(self, i));
	// 	printf("%X ", self->page_info[i].state);
	// 	if (!(i % 10) && i)
	// 		printf("\n");
	// }
}
