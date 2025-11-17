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
#include "symbols.h"

#define KERNELSTACKSIZE 4096
#define USERSTACKSIZE 4096

struct vga_console vga;
k_uint8_t user_stack[USERSTACKSIZE];
k_uint8_t kernel_stack[KERNELSTACKSIZE];

extern void switch_to_user_mode(void *user_stack_ptr, void *user_entry);
void dump_stack(void);

//	get task register (selector)
unsigned short get_tr()
{
	unsigned short tr;
	asm volatile("str %0" : "=r"(tr));
	return tr;
}

void kinit(struct gdt_entry *gdt, struct tss_entry *tss, struct ps2_driver *ps2)
{
	setup_gdt(gdt, tss);
	serial_init();
	ps2_driver_constructor(ps2); // here we assume ps2 controller always exists
	ps2->init(ps2);
	vga_console_constructor(&vga);
}

void putstr(const char *str)
{
	while (*str)
		vga.putchar(&vga, *str++);
	// vga.set_cursor(&vga, vga.row, vga.col);
}

void userHello()
{
	// vga = (struct vga_console){0};
	putstr("Hello from user mode!\n");
	while (1)
		;
}

void kernel(unsigned long magic, unsigned long addr)
{
	char character;
	struct ps2_driver ps2;
	static struct gdt_entry gdt[6] __attribute__((section(".gdt")));
	static struct tss_entry tss;

	(void)addr;

	if (magic != 0x36d76289) // magic value given by GRUB indicating it was
		return;				 // loaded by a Multiboot2-compliant bootloader

	kinit(gdt, &tss, &ps2);


	switch_to_user_mode(user_stack + USERSTACKSIZE - 4, userHello);

	vga.putchar(&vga, '4');
	vga.putchar(&vga, '2');
	vga.putchar(&vga, '\n');
	vga.set_cursor(&vga, vga.row, vga.col);

	while (1)
	{
		character = ps2.keyboard.handle_scancode(&ps2.keyboard, &ps2, ps2.read_byte(&ps2));
		if (character < 0)
		{
			vga.set_buffer(&vga, -character - 1);
			continue;
		}
		else if (character == '\b')
			vga.delchar(&vga);
		else if (character)
			vga.putchar(&vga, character);
		vga.set_cursor(&vga, vga.row, vga.col);
	}
}

void dump_stack(void)
{
    unsigned int *ebp;
    unsigned int *esp;
    unsigned int eip;
    
    asm volatile("movl %%ebp, %0" : "=r"(ebp));
    asm volatile("movl %%esp, %0" : "=r"(esp));
    
    printk(LOG_INFO, "=== KERNEL STACK DUMP ===\n");
    printk(LOG_INFO, "Current ESP: 0x%x\n", esp);
    printk(LOG_INFO, "Current EBP: 0x%x\n", ebp);
    printk(LOG_INFO, "Call trace:\n");
    
    int frame = 0;
    while (ebp != 0 && frame < 10)
    {
        eip = ebp[1];
        const char *symbol = resolve_symbol(eip);
        printk(LOG_INFO, " [%d] 0x%x <%s>\n", frame, eip, symbol);
        
        ebp = (unsigned int *)ebp[0];
        frame++;
    }
    
    printk(LOG_INFO, "=========================\n");
}