#include "ps2_driver.h"
#include "type.h"
#include "lib.h"
#include "printk.h"

#define TIMEOUT 10000

static k_uint8_t	read_byte_ps2(struct ps2_driver *self);
static bool 		has_data_ps2(struct ps2_driver *self);
static void 		send_byte_ps2(struct ps2_driver *self, k_uint8_t c);
int	    			init_ps2_controller(struct ps2_driver *self);

void ps2_driver_constructor(struct ps2_driver *self)
{
	memset(self, 0, sizeof(struct ps2_driver));
    self->data_port = PS2Data;
    self->status_port = PS2StatusCmd;

    self->init = init_ps2_controller;
    self->read_byte = read_byte_ps2;
    self->has_data = has_data_ps2;
    self->send_byte = send_byte_ps2;
}

static inline bool has_data_ps2(struct ps2_driver *self)
{
    return (inb(self->status_port) & BIT(0));
}

static inline void send_byte_ps2(struct ps2_driver *self, k_uint8_t c)
{
    while (inb(self->status_port) & BIT(1)); 
    
    outb(self->data_port, c);                
}

static inline k_uint8_t read_byte_ps2(struct ps2_driver *self)
{
    while (!self->has_data(self));

    return inb(self->data_port);
}

static int read_byte_ps2_timeout(struct ps2_driver *self)
{
    k_uint32_t timeout = TIMEOUT;

    while (!self->has_data(self) && timeout--);

    if (timeout == 0)
        return (-1);

    return (inb(self->data_port));
}

/*
	Made by following OSDev wiki part for 
	Initialising the PS/2 Controller (Intel 8042)
*/
static inline void write_ps2_config(struct ps2_driver *self, k_uint8_t byte)
{
	outb(PS2StatusCmd, 0x60);				// Tell to write controller config
	self->send_byte(self, byte);			// Send new config
}

static inline int test_ps2_port(struct ps2_driver *self, int port)
{
	k_uint8_t response_byte0, response_byte1;

	outb(PS2StatusCmd, port == 1 ? 0xAB : 0xA9);	// Test PS/2 1st/2nd port

	if (read_byte_ps2_timeout(self) != 0)
		return 0;

	outb(PS2StatusCmd, port == 1 ? 0xAE : 0xA8);	// Enable PS/2 1st/2nd port
	if (port == 2) outb(PS2StatusCmd, 0xD4);		// Enable write to 2nd port
	self->send_byte(self, 0xFF);					// reset device
	
	response_byte0 = self->read_byte(self);
	response_byte1 = self->read_byte(self);
	if (response_byte0 != 0xFA && response_byte1 != 0xAA)
	{
		printk(LOG_ERROR, "PS/2 controller: port%d test failed\n", port);
		return (0);
	}

	if (port == 2) outb(PS2StatusCmd, 0xD4);
	self->send_byte(self, 0xF5);			// Disable scan dev	
	printk(LOG_INFO, "PS/2 controller: ACK disable dev port %d: %X\n", port, self->read_byte(self));
	return (BIT(port - 1));
}

static inline void identify_ps2_device(struct ps2_driver *self, int port, int active_channels)
{
	int ret;

	if (!(active_channels & (1 << port - 1)))		// skip inactive ports
		return ;

	if (port == 2)
		outb(PS2StatusCmd, 0xD4);			// talk to 2nd dev
	self->send_byte(self, 0xF2);			// identify yourself
	do
		ret = read_byte_ps2_timeout(self);
	while (ret == 0xFA); 						// ACK
	
	if (ret >= 0xAB && read_byte_ps2_timeout(self) == 0x83) {
        printk(LOG_INFO, "PS/2 controller: keyboard detected on port %d\n", port);
        self->keyboard_port = port;
	}
	else if (ret == 0) {
		printk(LOG_INFO, "PS/2 controller: mouse detected on port %d\n", port);
		// mouse ???
	}
	else
		printk(LOG_WARN, "PS/2 controller: unsupported device or timeout\n");
}

int    init_ps2_controller(struct ps2_driver *self)
{
	int					dualChannel = 0, active_channels = 0;
	volatile int		ret;
	volatile k_uint8_t	byte;
	
	printk(LOG_INFO, "\n*** Starting PS/2 controller intialisation ***\n\n");

	outb(PS2StatusCmd, 0xAD);				// Disable 1st device
	outb(PS2StatusCmd, 0xA7);				// Disable 2nd device if it exists
	inb(PS2Data);							// Flush controller output buffer 

	
	outb(PS2StatusCmd, 0x20);				// Tell to read controller config
	ret = read_byte_ps2_timeout(self);			// read controller config
	if (ret < 0) { printk(LOG_ERROR, "PS/2 controller: error\n"); return (1); }
	
	byte = (k_uint8_t)ret & ~(BIT(6) | BIT(4) | BIT(0));	// 1st port: disable int, translation; enable port clock
	write_ps2_config(self, byte);			// Send new config

	outb(PS2StatusCmd, 0xAA);				// Perform controller self test
	if (read_byte_ps2_timeout(self) != 0x55) {
		write_ps2_config(self, byte);			// Send new config
		printk(LOG_ERROR, "PS/2 controller: self-test failed!\n");
		return (1);
	}
	printk(LOG_INFO, "PS/2 controller: self-test: pass\n");	// check result
	
	outb(PS2StatusCmd, 0xA8);			// Test dual channel
	outb(PS2StatusCmd, 0x20);			// Tell to read controller config
	byte = self->read_byte(self);		// read controller config
	if (!(byte & BIT(5))) {				// bit 5 must be clear 
		dualChannel = true;
		outb(PS2StatusCmd, 0xA7);			// Disable 2nd device if it exists
		byte &= ~(BIT(5) | BIT(1));			// 2nd port: disable int; enable port clock
		write_ps2_config(self, byte);		// Send new config
	}
	printk(LOG_INFO, "PS/2 controller: channel: %s\n", dualChannel ? "dual" : "mono");

	active_channels |= test_ps2_port(self, 1);
	if (dualChannel)
		active_channels |= test_ps2_port(self, 2);

	if (!active_channels) 
		{ printk(LOG_INFO, "PS/2 controller: No device detected!\n"); return (1); }

	identify_ps2_device(self, 1, active_channels);
	identify_ps2_device(self, 2, active_channels);

	if (self->keyboard_port)
		keyboard_constructor(&self->keyboard, 2);

	self->send_byte(self, 0xF4);				// enable scan 1st dev
	if (dualChannel && self->keyboard_port == 2) {
		outb(PS2StatusCmd, 0xD4);				// talk to 2nd dev
		self->send_byte(self, 0xF4);			// enable scan 2nd dev
	}

	printk(LOG_INFO, "\n*** PS/2 controller intialisation ended ***\n\n");

	return (0);
}

