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
	struct free_block_s	*prev;
	struct free_block_s	*next;
} free_block_t;

typedef struct free_area
{
	free_block_t	*head;
	unsigned long	*map;
} free_area_t;



typedef struct page_allocator
{
	// Attributs
	page_descriptor_t	*page_info;
	size_t				size_page_info;
	free_area_t			free_area[MAX_ORDER + 1];
	size_t				first_free_page, last_free_page;
	unsigned int		total_pages;

	// Methods
	void	*(*alloc_pages)(struct page_allocator *self, unsigned int order);
	void	(*free_pages)(struct page_allocator *self, void *addr);
	void	(*add_block)(struct page_allocator *self, int page_idx, int order);
	void	(*print_free_lists)(struct page_allocator *self);


} page_allocator_t;

int buddy_constructor(page_allocator_t *self, struct multiboot_mmap_entry *mmap_entry);

void			*list_new_node(void *addr);
void			list_add_head(free_block_t **head, free_block_t *new);
void			list_add_tail(free_block_t **head, free_block_t *new);
void			list_remove(free_block_t **head, free_block_t *to_remove);

free_block_t	*list_last(free_block_t *head);

#endif