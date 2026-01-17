#ifndef _BUDDY_H_
#define _BUDDY_H_

#include "type.h"
#include "multiboot2.h"

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12
#define MAX_ORDER 10

/*	bit[0] -> used/free
	bit[1] -> head of block/or not
	bit[2-5] -> buddy order
	bit[6-7] -> free for futur use
*/
typedef struct page_descriptor_s
{
	k_uint8_t	state;

} page_descriptor_t;

typedef struct free_block_s
{
	struct free_block	*prev;
	struct free_block	*next;
} free_block_t;

typedef struct free_area
{
	free_block_t	*head;
	unsigned long	*map;
} free_area_t;



typedef struct buddy_allocator
{
	// Attributs
	page_descriptor_t	*page_info;
	size_t				size_page_info;
	free_area_t			free_area[MAX_ORDER + 1];
	size_t				first_free_page, last_free_page;
	unsigned int		total_pages;

	// Methods
	void	*(*alloc_pages)(struct buddy_allocator *self, unsigned int nb_request);
	void	(*add_block)(struct buddy_allocator *self, int page_idx, int order);


} buddy_allocator_t;

int buddy_constructor(buddy_allocator_t *self, struct multiboot_mmap_entry *mmap_entry);

#endif