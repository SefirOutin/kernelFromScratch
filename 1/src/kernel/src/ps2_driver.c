#include "ps2_driver.h"
#include "type.h"
#include "lib.h"


static k_uint8_t	read_byte_ps2(struct ps2_driver *self);
static bool 		has_data_ps2(struct ps2_driver *self);
static void 		send_byte_ps2(struct ps2_driver *self, k_uint8_t c);
void    			init_ps2_controller(struct ps2_driver *self);


void ps2_driver_constructor(struct ps2_driver *self)
{
    self->data_port = PS2Data;
    self->status_port = PS2StatusCmd;

    self->init = init_ps2_controller;
    self->read_byte = read_byte_ps2;
    self->has_data = has_data_ps2;
    self->send_byte = send_byte_ps2;
}

static bool has_data_ps2(struct ps2_driver *self)
{
    return (inb(self->status_port) & bit(0));
}

static void send_byte_ps2(struct ps2_driver *self, k_uint8_t c)
{
    while (inb(self->status_port) & bit(1)); 
    
    outb(self->data_port, c);                
}

static k_uint8_t read_byte_ps2(struct ps2_driver *self)
{
    while (!(inb(self->status_port) & bit(0)));

    return inb(self->data_port);
}

void    init_ps2_controller(struct ps2_driver *self)
{
	bool				dualChannel = false;
	volatile k_uint8_t	controllerConf;
	
	outb(PS2StatusCmd, 0xAD);				// Disable 1st device
	outb(PS2StatusCmd, 0xA7);				// Disable 2nd device if it exists
	inb(PS2Data);							// Flush controller output buffer 

	outb(PS2StatusCmd, 0x20);				    // Tell to read controller config
	controllerConf = self->read_byte(self);		// read controller config
	controllerConf &= ~(bit(6) | bit(4) | bit(0));	// 1st port: disable int, translation, enable port clock
	outb(PS2StatusCmd, 0x60);				    // Tell to write controller config
	self->send_byte(self, controllerConf);		// Send new config

	outb(PS2StatusCmd, 0xAA);					// Perform controller self test
	printf("controller self-test: %s\n", self->read_byte(self) == 0x55 ? "pass" : "fail");	// check result
	
	outb(PS2StatusCmd, 0xA8);					// Test dual channel
	outb(PS2StatusCmd, 0x20);					// Tell to read controller config
	if (!(self->read_byte(self) & bit(5)))		// bit 5 must be clear
	{
		dualChannel = true;
		outb(PS2StatusCmd, 0xA7);			    // Disable 2nd device if it exists
		controllerConf &= ~(bit(5) | bit(1));	// 2nd port: disable int, enable port clock
		outb(PS2StatusCmd, 0x60);				// Tell to write controller config
		self->send_byte(self, controllerConf);
	}
	printf("dual test: %s\n", dualChannel ? "dual" : "mono");
	
	outb(PS2StatusCmd, 0xAB);				// Test PS/2 1st port
	if (self->read_byte(self) == 0)
	{
		outb(PS2StatusCmd, 0xAE);			// Enable PS/2 1st port
		self->send_byte(self, 0xFF);						// reset device
		for (int i = 0; i < 2; i++)
			printf("1st getChar: %X ", self->read_byte(self));
		printf("\n");
		self->send_byte(self, 0xF5);							// disable scan 1st dev
		printf("ACK disable 1st dev: %X\n", self->read_byte(self));
	}
	if (dualChannel)
	{
		outb(PS2StatusCmd, 0xA9);				// Test PS/2 2nd port
		if (self->read_byte(self) == 0)
		{
			outb(PS2StatusCmd, 0xA8);			// Enable PS/2 2nd port
			outb(PS2StatusCmd, 0xD4);			// Enable write to 2nd port
			self->send_byte(self, 0xFF);						// reset device
			for (int i = 0; i < 2; i++)
				printf("2nd getChar: %X ", self->read_byte(self));
			printf("\n");
		}
		outb(PS2StatusCmd, 0xD4);				
		self->send_byte(self, 0xF5);							// Disable scan 2nd dev	
		printf("ACK disable 2nd dev: %X\n", self->read_byte(self));
		
	}

	// First Device ID
	self->send_byte(self, 0xF2);				// identify yourself
	printf("ACK identity 1st dev: %X\n", self->read_byte(self));
	k_uint8_t c = self->read_byte(self);
	if (c >= 0xAB)
		printf("dev1 ID: %X %X\n", c, self->read_byte(self));
	else
		printf("dev1 ID: %X\n", c);
	keyboard_constructor(&self->keyboard, 2);

	// Second Device ID
	outb(PS2StatusCmd, 0xD4);					// talk to 2nd dev
	self->send_byte(self, 0xF2);				// identify yourself
	printf("ACK identity 2nd dev: %X\n", self->read_byte(self));
	k_uint8_t c1 = self->read_byte(self);
	if (c1 >= 0xAB)
		printf("dev2 ID: %X %X\n", c1, self->read_byte(self));
	else
		printf("dev2 ID: %X\n", c1);
	
	
	self->send_byte(self, 0xF4);				// enable scan 1st dev
	outb(PS2StatusCmd, 0xD4);
	self->send_byte(self, 0xF4);				// enable scan 2nd dev

}