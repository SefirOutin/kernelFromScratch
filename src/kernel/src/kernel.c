#include "multiboot2.h"
#include "vga_console.h"
#include "serial.h"
#include "ps2_driver.h"
#include "keyboard.h"
#include "lib.h"
#include "type.h"
#include "vga_buffer.h"
#include "gdt.h"

struct vga_console  vga;

void kernel(unsigned long magic, unsigned long addr)
{
	(void)addr;
	
	if (magic != 0x36d76289)		// magic value given by GRUB indicating it was
	return;						// loaded by a Multiboot2-compliant bootloader
	
	static struct gdt_entry	gdt[5];
	struct ps2_driver   ps2;
	struct vga_buffer	buffer0, buffer1;

	serial_init();
	
	ps2_driver_constructor(&ps2);	// here we assume ps2 controller always exists
	ps2.init(&ps2);
	
	setup_gdt(gdt);
	vga_buffer_constructor(&buffer0);
	vga_buffer_constructor(&buffer1);
	vga_console_constructor(&vga);
	vga.buffer[0] = &buffer0;	
	vga.buffer[1] = &buffer1;	
	
	vga.putchar(&vga, '4');
	vga.putchar(&vga, '2');
	vga.putchar(&vga, '\n');
	
	vga.set_cursor(&vga, vga.row, vga.col);
	char character;
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
