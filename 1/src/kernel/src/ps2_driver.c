#include "ps2_driver.h"
#include "type.h"
#include "lib.h"
static k_uint8_t read_byte_ps2(struct ps2_driver *self);
static bool has_data_ps2(struct ps2_driver *self);
static void send_byte_ps2(struct ps2_driver *self, k_uint8_t c);

void ps2_driver_init(struct ps2_driver *self)
{
    self->data_port = PS2Data;
    self->status_port = PS2StatusCmd;

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
    while (!has_data_ps2(self))
        ;
    return inb(self->data_port);
}
