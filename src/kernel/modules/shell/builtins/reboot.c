#include "builtins.h"
#include "type.h"
#include "lib.h"
#include "ps2_driver.h"

extern void send_cmd_ps2(k_uint8_t cmd);

void reboot(void)
{
	// // Use of 8042 controller with CPU reset line
	send_cmd_ps2(0xFE);
	__asm__ volatile("hlt");
}