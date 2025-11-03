#include "multiboot2.h"
#include "vga_console.h"
#include "serial.h"
#include "ps2_driver.h"
#include "keyboard.h"
#include "lib.h"
#include "type.h"
#include "vga_buffer.h"

struct vga_console  vga;

void kernel(unsigned long magic, unsigned long addr)
{
	(void)magic;
	(void)addr;

	struct ps2_driver   ps2;
	struct vga_buffer	buffer0, buffer1;

	serial_init();
	ps2_driver_constructor(&ps2);				// here we assume ps2 controller always exists
	vga_buffer_constructor(&buffer0);
	vga_buffer_constructor(&buffer1);
	vga_console_constructor(&vga);
	// vga.set_buffer(&vga, &buffer0);
	ps2.init(&ps2);
	vga.buffer[0] = &buffer0;	
	vga.buffer[1] = &buffer1;	
	
	vga.putchar(&vga, '4');
	vga.putchar(&vga, '2');
	vga.putchar(&vga, '\n');
	
	vga.set_cursor(&vga, vga.row, vga.col);
	char chart;
	while (1)
	{
	    chart = ps2.keyboard.handle_scancode(&ps2.keyboard, &ps2, ps2.read_byte(&ps2));
		printf("chart: %d\n", chart);
		if (chart < 0)
			vga.set_buffer(&vga, -chart - 1);
		if (chart)
		{
			vga.putchar(&vga, chart);
			vga.set_cursor(&vga, vga.row, vga.col);
		}
	}
}
