#include "buddy.h"

void	list_remove(free_block_t **head, free_block_t *to_remove)
{
	free_block_t	*prev;

	if (!to_remove)
		return ;

	if (*head == to_remove)
	{
		*head = to_remove->next;
		if (*head)
			(*head)->prev = NULL;
	}
	else
	{
		prev = to_remove->prev;
		prev->next = to_remove->next;	
		if (prev->next)
			(prev->next)->prev = prev;
	}
	to_remove->next = NULL;
	to_remove->prev = NULL;
}

void	list_add_head(free_block_t **head, free_block_t *new)
{
	if (*head == NULL)
		*head = new;
	else
	{
		(*head)->prev = new;
		new->next = *head;
		*head = new;
	}
}

void	*list_new_node(void *addr)
{
	free_block_t	*new;


	new = (free_block_t *)addr;	// the node lives at that addr (1st page of block)
	new->next = NULL;
	new->prev = NULL;

	return (new);
}

free_block_t	*list_last(free_block_t *head)
{
	while (head->next)
		head = head->next;
	return (head);
}


void	list_add_tail(free_block_t **head, free_block_t *new)
{
	free_block_t	*last;
	
	if (!new)
		return ;
	
	if (!(*head))	// if no element in list
		*head = new;
	else
	{
		last = list_last(*head);
		last->next = new;
		new->prev = last;
	}
}