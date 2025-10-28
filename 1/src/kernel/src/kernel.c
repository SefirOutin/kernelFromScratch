#include "multiboot2.h"
#include "vga_console.h"
#include "serial.h"
#include "ps2_driver.h"
#include "keyboard.h"
#include "lib.h"
#include "type.h"


void kernel(unsigned long magic, unsigned long addr)
{
	(void)magic;
	(void)addr;

	struct ps2_driver   ps2;
	struct vga_console  vga;

	vga_console_constructor(&vga);
	serialInit();
	ps2_driver_constructor(&ps2);
	ps2.init(&ps2);

	vga.putchar(&vga, '4');
	vga.putchar(&vga, '2');
	vga.putchar(&vga, '\n');

	vga.set_cursor(&vga, vga.row, vga.col);
	unsigned char chart;
	while (1)
	{
	    chart = ps2.keyboard.handle_scancode(&ps2.keyboard, &ps2, ps2.read_byte(&ps2));
	    printf("trans char: 0x%X\n", chart);
		if (chart)
			vga.putchar(&vga, chart);
		vga.set_cursor(&vga, vga.row, vga.col);
	}
}
