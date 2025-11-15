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

struct vga_console  vga;
k_uint8_t user_stack[USERSTACKSIZE];
k_uint8_t kernel_stack[KERNELSTACKSIZE];

extern void switch_to_user_mode(void *user_stack_ptr, void *user_entry);

//	get task register (selector)
unsigned short get_tr() {
    unsigned short tr;
    asm volatile("str %0" : "=r"(tr));
    return tr;
}

void	kinit(struct gdt_entry *gdt, struct tss_entry *tss, struct ps2_driver *ps2)
{
	setup_gdt(gdt, tss);
	serial_init();
	ps2_driver_constructor(ps2);	// here we assume ps2 controller always exists
	ps2->init(ps2);
	vga_console_constructor(&vga);

}

void	userHello()
{
	vga.putchar(&vga, 'C');
	for ( ;; );
}

void kernel(unsigned long magic, unsigned long addr)
{
	char character;
	struct ps2_driver   ps2;
	static struct gdt_entry gdt[6] __attribute__((section(".gdt")));
	static struct tss_entry	tss;

	(void)addr;
	
	if (magic != 0x36d76289)		// magic value given by GRUB indicating it was
	return;							// loaded by a Multiboot2-compliant bootloader
	

	kinit(gdt, &tss, &ps2);

	// switch_to_user_mode(&user_stack, &userHello);

	vga.putchar(&vga, '4');
	vga.putchar(&vga, '2');
	vga.putchar(&vga, '\n');
	vga.set_cursor(&vga, vga.row, vga.col);
	
	while (1)
	{
	    character = ps2.keyboard.handle_scancode(&ps2.keyboard, &ps2, ps2.read_byte(&ps2));
		if (character < 0)
			{ vga.set_buffer(&vga, -character - 1); continue; }
		else if (character == '\b')
			vga.delchar(&vga);
		else if (character)
			vga.putchar(&vga, character);
		vga.set_cursor(&vga, vga.row, vga.col);
	}
}
