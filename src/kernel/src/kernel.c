#include "multiboot2.h"
#include "vga_console.h"
#include "serial.h"
#include "ps2_driver.h"
#include "keyboard.h"
#include "lib.h"
#include "type.h"
#include "vga_buffer.h"
#include "gdt.h"
#include "tss.h"
#include "printk.h"

#include "microshell.h"

#define KERNELSTACKSIZE 4096
#define USERSTACKSIZE 4096

struct vga_console vga;
struct ps2_driver ps2;

k_uint8_t user_stack[USERSTACKSIZE];
k_uint8_t kernel_stack[KERNELSTACKSIZE];

extern void switch_to_user_mode(void *user_stack_ptr, void *user_entry);

void	STOM(void *user_stack_ptr, void *user_entry)
{
	putstr("Switching to user mode...\n");
	switch_to_user_mode(user_stack_ptr, user_entry);
}

void	kinit(struct gdt_entry *gdt, struct tss_entry *tss, struct ps2_driver *ps2)
{
	// Fill and load GDT with segments and TSS
	setup_gdt(gdt, tss);

	// Serial port communication
	serial_init();

	// Basic PS/2 controller driver for keyboard
	ps2_driver_constructor(ps2); // here we assume ps2 controller always exists
	ps2->init(ps2);

	// Basic interface to print to the VGA text buffer
	vga_console_constructor(&vga);
}

void userHello()
{
	// putstr("Hello from user mode!\n");
	while (1);
}

void	parse_mmap(k_uint32_t *boot_info, struct multiboot_tag_mmap *mmap)
{
	size_t i = 0;
	struct multiboot_mmap_entry *entry;
	k_uint32_t	limit_addr;
	
	printf("parsing mmap...\n");

	mmap->size = *(boot_info + 1);
	mmap->entry_size = *(boot_info + 2);
	mmap->entry_version = *(boot_info + 3);
	limit_addr = ((k_uint32_t)boot_info + mmap->size);
	boot_info += 4;				// skip infos just fetched

	printf("loop...\n");
	printf("1: %u | 2: %u\n", boot_info, limit_addr);
	while (boot_info < (k_uint32_t *)limit_addr)
	{
		printf("1: %u | 2: %u ", boot_info, limit_addr);
		printf("phys addr: \n", *boot_info);
		entry = &mmap->entries[i];
		entry->addr = *(k_uint64_t *)boot_info;
		entry->len = *((k_uint64_t *)boot_info + 1);
		entry->type = *(boot_info + 4);
		entry->zero = *(boot_info + 5);
		boot_info = (k_uint32_t *)((k_uint32_t)boot_info + mmap->entry_size);
	}
}

void	print_mmap(struct multiboot_tag_mmap *mmap)
{
	int	i = 0, nb_entries = (mmap->size - 16) / mmap->entry_size;
	printf("mmap struct\nsize: %u, entry_size: %u, entry_version: %u\n", mmap->size, mmap->entry_size, mmap->entry_version);
	printf("nb entries: %u\n", nb_entries);

	while (i < nb_entries)
	{
		printf("entry_%d: addr: %X, len: %u, type: %d\n", i, mmap->entries[i].addr, mmap->entries[i].len, mmap->entries[i].type);
		i++;
	}
}

void	parse_boot_struct(k_uint32_t *boot_info, struct multiboot_tag_mmap *mmap)
{
	k_uint32_t size, type;
	int i = 0;
	unsigned char str[400];
	k_uint8_t *tmp = (k_uint8_t *)boot_info;
	while (i < 1280 / 4)
	{
		printf("%u ", tmp[i]);
		i++;
		if (!(i % 20))
			printf("\n");

	}
	boot_info += 2;								// skip first tag, still aligned
	while (*boot_info != 6 && *boot_info != 0)	// search for memory map
	{
		size = boot_info[1];					// size of tag is 2nd member of struct
		boot_info = (k_uint32_t *)((k_uint32_t)(boot_info) + size);	// bypass uint32 pointer arithmetic
		boot_info = (k_uint32_t *)(((k_uint32_t)(boot_info) + 7) & ~7);	// skip to next 8-bytes aligned addr
	}
	if (*boot_info == 6)
	{
		parse_mmap(boot_info, mmap);
		print_mmap(mmap);
	}
	// printf("mmap: size: %u, entry_size: %u, e_version: %u", *(boot_info + 1), *(boot_info + 2), *(boot_info + 3));
}

void kernel(k_uint32_t magic, k_uint32_t *addr)
{
	struct multiboot_tag_mmap mmap;
	static struct tss_entry tss;
	// GDTable is located at 0x800 (see linker)
	static struct gdt_entry gdt[6] __attribute__((section(".gdt")));

	if (magic != 0x36d76289) // magic value given by GRUB indicating it was
		return;				 // loaded by a Multiboot2-compliant bootloader
		
	kinit(gdt, &tss, &ps2);
		
	printf("kernel addr: %p\n", kernel);
	parse_boot_struct(addr, &mmap);
	
	putstr("Welcome to minishell\n");
	microshell();
}
