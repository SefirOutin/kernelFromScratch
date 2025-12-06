#include "type.h"
#include "multiboot2.h"
#include "printk.h"
#include "lib.h"

//         +-------------------+
// u32     | type = 6          |
// u32     | size              |
// u32     | entry_size        |
// u32     | entry_version     |
// varies  | entries           |
//         +-------------------+

// entry:
//         +-------------------+
// u64     | base_addr         |
// u64     | length            |
// u32     | type              |
// u32     | reserved          |
//         +-------------------+


void	print_mmap(struct multiboot_tag_mmap *mmap);

static inline void *aligned_jmp(void *ptr, int len_jmp)
{
	return ((void *)(((k_uint32_t)ptr + len_jmp + 7) & ~7));
}

void	parse_mmap(k_uint32_t *boot_info, struct multiboot_tag_mmap *mmap)
{
	size_t i = 0;
	struct multiboot_mmap_entry *entry, *tmp;
	k_uint8_t	*ptr, *end;

	mmap->size = boot_info[1];
	mmap->entry_size = boot_info[2];
	mmap->entry_version = boot_info[3];

	ptr = (k_uint8_t *)boot_info;	// avoid ptr arithmetic
	end = ptr + mmap->size;			// end of mmap tag
	ptr += 16;						// skip data just fetched, still aligned

	while (ptr < end)
	{
		// use of a temp struct to easily fill entries
		tmp = (struct multiboot_mmap_entry *)ptr;
		
		entry = &mmap->entries[i++];
		entry->addr = tmp->addr;
		entry->len = tmp->len;
		entry->type = tmp->type;
		entry->zero = tmp->zero;

		ptr += mmap->entry_size;	// next entry
	}
	print_mmap(mmap);
}

void	parse_boot_struct(k_uint32_t *boot_info, struct multiboot_tag_mmap *mmap)
{
	k_uint32_t	size, type;
	k_uint8_t	flag = 0;

	boot_info += 2;								// skip first tag, still aligned
	while (*boot_info != 0)						// search until end tag
	{
		type = boot_info[0];					// type of tag is 1st member of struct
		size = boot_info[1];					// size of tag is 2nd member of struct
		switch (type)
		{
			case MULTIBOOT_TAG_TYPE_MMAP:
				parse_mmap(boot_info, mmap);
				flag = 1;
				break;

		}
		boot_info = (k_uint32_t *)aligned_jmp(boot_info, size);	// skip to next 8-bytes aligned tag
	}
	if (!flag)
		printk(LOG_ERROR, "corrupted mmap\n");
}

void	print_mmap(struct multiboot_tag_mmap *mmap)
{
	int	i = 0, nb_entries, discarded_tag_data = 16;

	nb_entries = (mmap->size - discarded_tag_data) / mmap->entry_size;
	
	printf("mmap struct\n");
	printf("size: %u, entry_size: %u, entry_version: %u\n", mmap->size, mmap->entry_size, mmap->entry_version);
	printf("nb entries: %u\n", nb_entries);

	while (i < nb_entries)
	{
		printf("entry_%d: addr: %X, len: %u, type: %u\n",
			i, (k_uint32_t)mmap->entries[i].addr, (k_uint32_t)mmap->entries[i].len, mmap->entries[i].type);
		i++;
	}
}