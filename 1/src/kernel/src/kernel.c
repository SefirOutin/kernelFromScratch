#include "multiboot2.h"
#include "vga_console.h"
#include "type.h"

void kernel(unsigned long magic, unsigned long addr)
{
  (void)magic;
  (void)addr;

  struct vga_console vga;
  vga_console_init(&vga);

  vga.putchar(&vga, '4');
  vga.putchar(&vga, '\n');
  vga.putchar(&vga, 'O');
  vga.putchar(&vga, 'K');
  vga.set_cursor(&vga, vga.row, vga.col);
}
