#include "buddy.h"
#include "lib.h"

// #define VIRT_IDX(a) (a - self->first_free_page)
#define GET_FLAG_PAGE_INFO(index) (self->page_info[index].state)
#define USED_BIT 1
#define HEAD_OF_BLOCK_BIT (1 << 1)
#define ORDER_BITS(order) ((order & 0x0F) << 2)
#define BLOCK_SIZE(order) (1 << order)
#define BITS_PER_LONG  sizeof(unsigned long) * 8
#define SHIFT_FOR_LONG 5

void __change_bit(long nr, volatile unsigned long *addr)
{
	addr[nr >> SHIFT_FOR_LONG] ^= ((unsigned long)1 << (nr % BITS_PER_LONG));
}

#define MARK_USED(index, order, area) \
        __change_bit((index) >> (1 + (order)), (area)->map)

#define MARK_FREE(index, order, area) \
        __change_bit((index) >> (1 + (order)), (area)->map)


extern k_uint32_t _kernel_end; // defined in linker script

void	*alloc_pages(buddy_allocator_t *self, unsigned int nb_request);
void	print_buddy_metadata(buddy_allocator_t *self);
void	free_pages(buddy_allocator_t *self, void *to_free);


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

static inline void *aligned_up(void *addr, size_t align)
{
	return ((void *)(((size_t)addr + align - 1) & ~(align - 1)));
}

static inline size_t aligned_idx(size_t page_idx, int order)
{
	return (((page_idx + (1 << order) - 1) & ~((1 << order) - 1)));
}

static inline bool	is_aligned(size_t page_idx, int order)
{
	return ((page_idx & ((1 << order) - 1)) == 0);
}

static inline size_t	page_idx_to_addr(buddy_allocator_t *self, size_t page_idx)
{
	return (((page_idx + self->first_free_page) << PAGE_SHIFT));
}

static inline size_t	addr_to_idx(buddy_allocator_t *self, void *addr)
{
	return (((size_t)addr >> PAGE_SHIFT) - self->first_free_page);
} 

static inline bool	is_head(unsigned char flag)
{
	return ((flag >> 1) & 1);
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

static inline bool	read_buddy_bit(long nr, unsigned long *addr)
{
	return ((addr[nr >> SHIFT_FOR_LONG] >> (nr % BITS_PER_LONG)) & 1);
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

void	list_remove(free_block_t **head, free_block_t *to_remove)
{
	free_block_t	*prev;

	if (!to_remove)
		return ;

	if (*head == to_remove)
	{
		*head = to_remove->next;
		(*head)->prev = NULL;
	}
	else
	{
		prev = to_remove->prev;
		prev->next = to_remove->next;	
	}
}

void	list_add_head(buddy_allocator_t *self, free_block_t *new, unsigned int order)
{
	if (self->free_area[order].head == NULL)
		self->free_area[order].head = new;
	else
	{
		self->free_area[order].head->prev = new;
		new->next = self->free_area[order].head;
		self->free_area[order].head = new;
	}
}

void	*list_new_node(buddy_allocator_t *self, size_t page_idx, int order)
{
	free_block_t	*new;

	if (page_idx < self->first_free_page || page_idx > self->last_free_page) // check boundaries
		return (NULL);

	new = (free_block_t *)page_idx_to_addr(self, page_idx);	// the node lives at that addr (1st page of block)
	new->next = NULL;
	new->prev = NULL;

	update_page_info(self, page_idx, 0, order);

	return (new);
}

free_block_t	*list_last(free_block_t *head)
{
	while (head->next)
		head = head->next;
	return (head);
}


void	list_add_tail(buddy_allocator_t *self, size_t page_idx, int order)
{
	free_block_t	*new, *head, *last;
	
	new = list_new_node(self, page_idx, order);
	if (!new)
	return ;
	
	if (!self->free_area[order].head)	// if no element in list
		self->free_area[order].head = new;
	
	else
	{
		head = self->free_area[order].head;
		last = list_last(head);
		last->next = new;
		new->prev = last;
	}
}
/*
	returns first usable page index

	reserve space for all the maps used by buddy allocator
*/
size_t	pages_maps_init(buddy_allocator_t *self, struct multiboot_mmap_entry *mmap_entry)
{
	void	*curr_ptr;
	size_t	current_page, maps_end;

	self->page_info = (page_descriptor_t *)aligned_up(&_kernel_end, PAGE_SIZE);
	self->total_pages = mmap_entry->len >> PAGE_SHIFT;
	self->size_page_info =  self->total_pages * sizeof(page_descriptor_t);
	curr_ptr = aligned_up((size_t)self->page_info + self->size_page_info, PAGE_SIZE);
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
		curr_ptr = aligned_up(curr_ptr + size_map, sizeof(*self->free_area->map));
	}
	curr_ptr = aligned_up(curr_ptr, PAGE_SIZE);
	memset(self->page_info, 0, ((k_uintptr_t)curr_ptr - (k_uintptr_t)self->page_info));
	
	printf("curr ptr addr: %p | idx: %d\n", curr_ptr, addr_to_idx(self, aligned_up(curr_ptr, PAGE_SIZE)));
	
	maps_end = addr_to_idx(self, curr_ptr);
	current_page = 0;							// begin at start of usable ram

	while (current_page < maps_end)				// mark kernel memory as used and skip it
		self->page_info[current_page++].state = USED_BIT;
	
	return (current_page);
}

/*
	max 4MB blocks;
*/
int	init_buddy_allocator(buddy_allocator_t *self, struct multiboot_mmap_entry *mmap_entry)
{
	size_t	current_page;
	int		order;
	
	current_page = pages_maps_init(self, mmap_entry);

	order = MAX_ORDER;
	while (current_page < self->last_free_page)
	{
		// printf("last page: %u current page: %u aligned: %s blockSize: %u blockEnd: %u\n", self->last_free_page, current_page, is_aligned(current_page, order) ? "true" : "false", BLOCK_SIZE(order), current_page + BLOCK_SIZE(order));
		if (is_aligned(current_page, order)									// check remaining space and
			&& current_page + BLOCK_SIZE(order) <= self->last_free_page)	// page alignement
		{
			list_add_tail(self, current_page, order);
			current_page += BLOCK_SIZE(order);
			order = MAX_ORDER;
		}
		else
			order--;
	}

	k_uintptr_t		*alloc, *alloc1 ;

	alloc = alloc_pages(self, 0);
	
	alloc1 = alloc_pages(self, 4);
	// free_pages(self, alloc);
	// free_pages(self, alloc1);
	// size_t idx = addr_to_idx(self, alloc);
	
	print_buddy_metadata(self);
	// printf("alloc addr:%p\n", alloc);

	return (0);
}

int	buddy_constructor(buddy_allocator_t *self, struct multiboot_mmap_entry *mmap_entry)
{
	memset(self, 0, sizeof(buddy_allocator_t));

	self->alloc_pages = alloc_pages;

	init_buddy_allocator(self, mmap_entry);
	return (0);
}

int	split_block(buddy_allocator_t *self, unsigned int target_order)
{
	free_block_t	*to_split, *new_buddy;
	size_t			idx_to_split;
	unsigned int	available_order;

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
		new_buddy = list_new_node(self, idx_to_split ^ BLOCK_SIZE(available_order), available_order);
		if (!new_buddy)
			return (1);
	
		list_add_head(self, new_buddy, available_order);
	}
	
	self->page_info[addr_to_idx(self, to_split)].state = HEAD_OF_BLOCK_BIT | ORDER_BITS(target_order);
	list_add_head(self, to_split, target_order);
	return (0);
}

void	*alloc_pages(buddy_allocator_t *self, unsigned int order)
{
	free_block_t	*new_alloc;
	size_t			new_alloc_index;

	if (order > MAX_ORDER)
		return (NULL);

	if (self->free_area[order].head == NULL)
		if (split_block(self, order))
			return (NULL);

	new_alloc = self->free_area[order].head;
	new_alloc_index = addr_to_idx(self, new_alloc);
	list_remove(&self->free_area[order].head, (free_block_t *)new_alloc);
	update_page_info(self, new_alloc_index, USED_BIT, order);
	memset(new_alloc, 0, sizeof(free_block_t));		// clear data at start of the block | REALLY NEEDED ?
	
	MARK_USED(new_alloc_index, order, &self->free_area[order]);

	return (new_alloc);
}

static inline bool	buddy_avail(unsigned char curr_flag, unsigned char buddy_flag)
{
	return ((is_used(buddy_flag) == false)
			&& (is_head(buddy_flag) == true)
			&& (same_order(curr_flag, buddy_flag) == true));
}

#define CHECK_BUDDY_STATE(index, order, area) \
	read_buddy_bit((index) >> (1 + (order)), (area)->map)

static inline void	swap_idx(size_t *a, size_t *b)
{
	*a = *b ^ *a;
	*b = *a ^ *b;
	*a = *b ^ *a;
}

void	merge_buddies(buddy_allocator_t *self, size_t current_idx)
{
	free_block_t	*new, *buddy;
	k_uint8_t		*flag;
	unsigned int	order;
	size_t			buddy_idx;

	flag = &self->page_info[current_idx].state;
	order = parse_order(*flag);
	buddy_idx = current_idx ^ BLOCK_SIZE(order);
	printf("flag: %X\n", *flag);
	printf("flag: %X\n", self->page_info[buddy_idx].state);
	printf("ooo: %d\n", CHECK_BUDDY_STATE(current_idx, order, &self->free_area[order]));
	printf("uuuu: %d\n", buddy_avail(*flag, self->page_info[buddy_idx].state) == 1);
		
	while (order <= MAX_ORDER
		&& CHECK_BUDDY_STATE(current_idx, order, &self->free_area[order]) == 0
		&& buddy_avail(*flag, self->page_info[buddy_idx].state) == 1)
	{
		printf("flag: %X\n", *flag);
		printf("flag: %X\n", self->page_info[buddy_idx].state);
		printf("ooo: %d\n", CHECK_BUDDY_STATE(current_idx, order, &self->free_area[order]));
		printf("uuuu: %d\n", buddy_avail(*flag, self->page_info[buddy_idx].state) == 1);
		if (current_idx > buddy_idx)
			swap_idx(&current_idx, &buddy_idx);
		list_remove(&self->free_area[order].head, (free_block_t *)page_idx_to_addr(self, buddy_idx));
		order++;
		self->page_info[buddy_idx].state = 0;		 // remove head
		flag = &self->page_info[current_idx].state;
		*flag |= ORDER_BITS(order);
		buddy_idx = current_idx ^ BLOCK_SIZE(order);
		printf("------------------------------\n");
		printf("flag: %X\n", *flag);
		printf("buddy flag: %X\n", self->page_info[buddy_idx].state);
		printf("ooo: %d\n", CHECK_BUDDY_STATE(current_idx, order, &self->free_area[order]));
		printf("uuuu: %d\n", buddy_avail(*flag, self->page_info[buddy_idx].state));
	}
	new = list_new_node(self, current_idx, order);
	list_add_head(self, new, order);	
}

void	free_pages(buddy_allocator_t *self, void *to_free)
{
	size_t			idx_to_free;
	unsigned char	flag;

	idx_to_free = addr_to_idx(self, to_free);
	flag = self->page_info[idx_to_free].state;

	if (!is_head(flag) && !is_used(flag))
		return ;

	MARK_FREE(idx_to_free, parse_order(flag), &self->free_area[parse_order(flag)]);

	update_page_info(self, idx_to_free, 0, parse_order(flag));

	merge_buddies(self, idx_to_free);

}

void	print_buddy_metadata(buddy_allocator_t *self)
{
	free_block_t	*tmp;
	// printf("state buddy: %d\n", (self->free_area[0].map[(idx >> (1+(0))) >> 5] << ((idx >> (1+(0))) % 32)) & 1);
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
