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
#include "buddy.h"

#include "microshell.h"

#define KERNELSTACKSIZE 4096
#define USERSTACKSIZE 4096

struct vga_console vga;
struct ps2_driver ps2;

k_uint8_t user_stack[USERSTACKSIZE];
k_uint8_t kernel_stack[KERNELSTACKSIZE];

extern void switch_to_user_mode(void *user_stack_ptr, void *user_entry);

void	STUM(void *user_stack_ptr, void *user_entry)
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

void	parse_boot_struct(k_uint32_t *boot_info, struct multiboot_tag_mmap *mmap);


void kernel(k_uint32_t magic, k_uint32_t *addr)
{
	struct multiboot_tag_mmap	mmap;
	static struct tss_entry 	tss;
	static page_allocator_t		page_manager;
	// GDTable is located at 0x800 (see linker)
	static struct gdt_entry gdt[6] __attribute__((section(".gdt")));

	if (magic != 0x36d76289) // magic value given by GRUB indicating it was
		return;				 // loaded by a Multiboot2-compliant bootloader
		
	kinit(gdt, &tss, &ps2);
	parse_boot_struct(addr, &mmap);
	
	buddy_constructor(&page_manager, &mmap.entries[3]);
	
	k_uintptr_t		*alloc[256];

	printf("BEFORE:\n");
	page_manager.print_free_lists(&page_manager);

	alloc[0] = page_manager.alloc_pages(&page_manager, 4);
	alloc[1] = page_manager.alloc_pages(&page_manager, 4);
	alloc[2] = page_manager.alloc_pages(&page_manager, 4);
	alloc[3] = page_manager.alloc_pages(&page_manager, 0);
	alloc[4] = page_manager.alloc_pages(&page_manager, 0);
	alloc[5] = page_manager.alloc_pages(&page_manager, 0);
	alloc[6] = page_manager.alloc_pages(&page_manager, 7);

	// page_manager.free_pages(&page_manager, alloc2);
	// page_manager.free_pages(&page_manager, alloc1);
	// page_manager.free_pages(&page_manager, alloc);
	
	printf("AFTER:\n");
	page_manager.print_free_lists(&page_manager);
	for (int i = 0; i <= 6; i++)
	{
		printf("alloc[%d]: %p\n", i, alloc[i]);
		page_manager.free_pages(&page_manager, alloc[i]);
	}

	printf("AFTER AFTER:\n");
	page_manager.print_free_lists(&page_manager);
	putstr("Welcome to OS\n");
	microshell();
}
