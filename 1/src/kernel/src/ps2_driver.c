#include "ps2_driver.h"
#include "type.h"
#include "lib.h"


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
    // self->has_data = has_data_ps2;
    self->send_byte = send_byte_ps2;
}

static inline bool has_data_ps2(struct ps2_driver *self)
{
    return (inb(self->status_port) & bit(0));
}

static inline void send_byte_ps2(struct ps2_driver *self, k_uint8_t c)
{
    while (inb(self->status_port) & bit(1)); 
    
    outb(self->data_port, c);                
}

static inline k_uint8_t read_byte_ps2(struct ps2_driver *self)
{
    while (!has_data_ps2(self));

    return inb(self->data_port);
}

static inline k_uint8_t read_byte_ps2_timeout(struct ps2_driver *self)
{
    while (!has_data_ps2(self))
	{

	}

    return inb(self->data_port);
}

int    init_ps2_controller(struct ps2_driver *self)
{
	int					dualChannel = 0, active_channels = 0;
	volatile k_uint8_t	controllerConf;
	k_uint8_t c;	
	
	outb(PS2StatusCmd, 0xAD);				// Disable 1st device
	outb(PS2StatusCmd, 0xA7);				// Disable 2nd device if it exists
	inb(PS2Data);							// Flush controller output buffer 

	outb(PS2StatusCmd, 0x20);				    	// Tell to read controller config
	controllerConf = self->read_byte(self);			// read controller config
	controllerConf &= ~(bit(6) | bit(4) | bit(0));	// 1st port: disable int, translation; enable port clock
	outb(PS2StatusCmd, 0x60);				    	// Tell to write controller config
	self->send_byte(self, controllerConf);			// Send new config

	outb(PS2StatusCmd, 0xAA);					// Perform controller self test
	if (self->read_byte(self) != 0x55)
	{
		outb(PS2StatusCmd, 0x60);				    	// Tell to write controller config
		self->send_byte(self, controllerConf);			// Send new config
		printf("PS/2 controller self-test failed!\n");
		return 1;
	}
	printf("PS/2 controller self-test: pass\n");	// check result
	
	outb(PS2StatusCmd, 0xA8);					// Test dual channel
	outb(PS2StatusCmd, 0x20);					// Tell to read controller config
	controllerConf = self->read_byte(self);		// read controller config
	if (!(controllerConf & bit(5)))				// bit 5 must be clear
	{
		dualChannel = true;
		outb(PS2StatusCmd, 0xA7);			    // Disable 2nd device if it exists
		controllerConf &= ~(bit(5) | bit(1));	// 2nd port: disable int; enable port clock
		outb(PS2StatusCmd, 0x60);				// Tell to write controller config
		self->send_byte(self, controllerConf);
	}
	printf("channel: %s\n", dualChannel ? "dual" : "mono");
	
	outb(PS2StatusCmd, 0xAB);				// Test PS/2 1st port
	if (self->read_byte(self) == 0)
	{
		active_channels++;
		outb(PS2StatusCmd, 0xAE);			// Enable PS/2 1st port
		self->send_byte(self, 0xFF);		// reset device
		for (int i = 0; i < 2; i++)
			printf("1st port test: %X ", self->read_byte(self));
		printf("\n");
		self->send_byte(self, 0xF5);		// disable scan 1st dev
		printf("ACK disable 1st dev: %X\n", self->read_byte(self));
	}
	if (dualChannel)
	{
		outb(PS2StatusCmd, 0xA9);				// Test PS/2 2nd port
		if (self->read_byte(self) == 0)
		{
			active_channels++;
			outb(PS2StatusCmd, 0xA8);			// Enable PS/2 2nd port
			outb(PS2StatusCmd, 0xD4);			// Enable write to 2nd port
			self->send_byte(self, 0xFF);		// reset device
			for (int i = 0; i < 2; i++)
				printf("2nd port test: %X ", self->read_byte(self));
			printf("\n");
		}
		outb(PS2StatusCmd, 0xD4);				
		self->send_byte(self, 0xF5);			// Disable scan 2nd dev	
		printf("ACK disable 2nd dev: %X\n", self->read_byte(self));
		
	}
	if (!active_channels) { printf("No PS/2 device detected!\n"); return (1); }

	for (int i = 0; i <= dualChannel; i++)
	{
		if (i)
			outb(PS2StatusCmd, 0xD4);			// talk to 2nd dev
		self->send_byte(self, 0xF2);			// identify yourself
		do
			c = self->read_byte(self);
		while (c == 0xFA); 						// ACK
		if (c >= 0xAB && self->read_byte(self) == 0x83) {
            printf("keyboard detected on port %d\n", i ? 2 : 1);
            self->keyboard_port = i ? 2 : 1;
		}
		else if (c == 0) {
			printf("mouse detected on port %d\n", i ? 2 : 1);
			// mouse ???
		}
		else
			printf("unsupported device%d ID: %X\n", i, c);

	}

	if (self->keyboard_port)
		keyboard_constructor(&self->keyboard, 2);

	
	self->send_byte(self, 0xF4);				// enable scan 1st dev
	outb(PS2StatusCmd, 0xD4);					// talk to 2nd dev
	self->send_byte(self, 0xF4);				// enable scan 2nd dev
	return 0;
}
