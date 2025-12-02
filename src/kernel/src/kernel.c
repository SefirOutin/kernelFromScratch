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

void parse_boot_struct(k_uint32_t *boot_info)
{
	int i = 0;
	// k_uint8_t *tmp = boot_info;
	while (i < 1280 / 4)
	{
		printf("%X ", boot_info[i]);
		if (!(i % 10))
			printf("\n");
		i++;
	}
	return ;
	printf("s[0] %d | addr: %d\n", *boot_info, boot_info);
	boot_info += 2;								// skip first tag
	printf("s[0] %d | addr: %d\n", *boot_info, boot_info);
	boot_info = (k_uint32_t *)(((k_uint32_t)boot_info + 7) & ~7);
	printf("s[0] %d | addr: %d\n", *boot_info, boot_info);
	while (*boot_info != 4 && *boot_info != 0)
	{
		printf("type: %d | size: %d | base: %X\n", *boot_info, *(boot_info + 1), *(boot_info + 2));
		boot_info += (*(boot_info + 1)) / 4;	// skip tag size bytes
		boot_info = (k_uint32_t *)(((k_uint32_t)boot_info + 7) & ~7);	// skip to next 8-bytes aligned addr
	}
	printk(LOG_INFO, "type: %d\nsize: %d\nlower: %X\nupper:%X\n", *boot_info, *(boot_info + 1), *(boot_info + 2), *(boot_info + 3));
}

void kernel(k_uint32_t magic, k_uint32_t *addr)
{
	static struct tss_entry tss;
	// GDTable is located at 0x800 (see linker)
	static struct gdt_entry gdt[6] __attribute__((section(".gdt")));

	if (magic != 0x36d76289) // magic value given by GRUB indicating it was
		return;				 // loaded by a Multiboot2-compliant bootloader

	kinit(gdt, &tss, &ps2);

	printf("kernel addr: %p\n", kernel);
	parse_boot_struct(addr);
	
	putstr("Welcome to minishell\n");
	microshell();
}
