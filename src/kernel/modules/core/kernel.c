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

extern char _kernel_end; // defined in linker script
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

static inline void *aligned_addr(void *addr, k_uint32_t align)
{
	return ((void *)(((k_uint32_t)addr + align - 1) & ~(align - 1)));
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
	printf("kernel end: %p\n", aligned_addr(&_kernel_end, 4096));
	parse_boot_struct(addr, &mmap);

	putstr("Welcome to microshell\n");
	microshell();
}
