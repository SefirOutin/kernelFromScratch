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

#define KERNELSTACKSIZE 4096
#define USERSTACKSIZE 4096

struct vga_console vga;
struct ps2_driver ps2;

k_uint8_t user_stack[USERSTACKSIZE];
k_uint8_t kernel_stack[KERNELSTACKSIZE];

extern void switch_to_user_mode(void *user_stack_ptr, void *user_entry);

void kinit(struct gdt_entry *gdt, struct tss_entry *tss, struct ps2_driver *ps2)
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
	putstr("Hello from user mode!\n");
	while (1);
}

//  Limited to width of screen for now
void	readline(char *buffer)
{
	char	character;
	int		i = 0;

	while (i < VGA_WIDTH)
	{
		character = ps2.keyboard.handle_scancode(&ps2.keyboard, &ps2, ps2.read_byte(&ps2));

		if (character == '\b')
			{ vga.delchar(&vga); i--; }
		else if (character == '\n')
			{ vga.putchar(&vga, character); vga.set_cursor(&vga, vga.row, vga.col); break; }
		else if (character)
		{
			vga.putchar(&vga, character);
			buffer[i++] = character;
		}
		vga.set_cursor(&vga, vga.row, vga.col);

	}
	buffer[i] = '\0';
}

void	microshell()
{
	char buffer[VGA_WIDTH + 1];

	while (1)
	{
		readline(buffer);
	}
}

void kernel(unsigned long magic, unsigned long addr)
{
	static struct tss_entry tss;
	// GDTable is located at 0x800
	static struct gdt_entry gdt[6] __attribute__((section(".gdt")));

	(void)addr;

	if (magic != 0x36d76289) // magic value given by GRUB indicating it was
		return;				 // loaded by a Multiboot2-compliant bootloader

	kinit(gdt, &tss, &ps2);

	// top of the user stack
	// switch_to_user_mode(user_stack + USERSTACKSIZE - 4, userHello);

	putstr("Welcome to minishell\n");
	vga.set_cursor(&vga, vga.row, vga.col);

	microshell();
}
