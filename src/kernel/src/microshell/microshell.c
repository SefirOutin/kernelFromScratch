#include "microshell.h"
#include "vga_console.h"
#include "ps2_driver.h"

extern struct vga_console	vga;
extern struct ps2_driver	ps2;

extern k_uint8_t user_stack[];

extern void	STOM(void *user_stack_ptr, void *user_entry);
extern void	userHello();

//  Limited to width of screen for now
void	readline(char *buffer)
{
	char	character;
	int		i = 0;

	while (i < VGA_WIDTH - 2)
	{
		character = ps2.keyboard.handle_scancode(&ps2.keyboard, &ps2, ps2.read_byte(&ps2));

		if (character == '\b')
			{ vga.delchar(&vga); i--; }
		else if (character == '\n')
			{ putchar(character); break; }
		else if (character)
		{
			putchar(character);
			buffer[i++] = character;
		}
	}
	buffer[i] = '\0';
}


void	(*builtins[64])() = {
	dump_stack_builtin,
	reboot,
	halt,
	clear,
	STOM,

};

int	decode_cmd(char *buffer)
{
	char	save;
	int 	i = 0, ret = -1;
	
	// Trim to keep 1 word
	while (buffer[i] && buffer[i] != ' ')
		i++;
	save = buffer[i];
	buffer[i] = 0;

	// Find builtin
	if (!strcmp("dump_stack", buffer))
		ret = 0;
	else if (!strcmp("reboot", buffer))
		ret = 1;
	else if (!strcmp("halte", buffer))
		ret = 2;
	else if (!strcmp("clear", buffer))
		ret = 3;
	else if (!strcmp("usermode", buffer))
		ret = 4;

	// Restore deleted char
	buffer[i] = save;
	return (ret);
}

#define USERSTACKSIZE 4096

void	microshell()
{
	int		ret;
	char	buffer[VGA_WIDTH - 2 + 1];

	while (1)
	{
		putstr("> ");
		readline(buffer);

		ret = decode_cmd(buffer);
		if (ret < 0)
			putstr("error: cmd not found\n");

		if (ret < 0 || ret > 4)
			continue;

		// top of the user stack
		if (ret == 4)
			 STOM(user_stack + USERSTACKSIZE - 4, userHello);
		else
			builtins[ret]();
	}
}