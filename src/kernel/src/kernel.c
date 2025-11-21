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


void kernel(unsigned long magic, unsigned long addr)
{
	static struct tss_entry tss;
	// GDTable is located at 0x800 (see linker)
	static struct gdt_entry gdt[6] __attribute__((section(".gdt")));

	(void)addr;

	if (magic != 0x36d76289) // magic value given by GRUB indicating it was
		return;				 // loaded by a Multiboot2-compliant bootloader

	kinit(gdt, &tss, &ps2);

	putstr("Welcome to minishell\n");

	microshell();
}
