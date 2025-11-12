#ifndef _PS2_DRIVER_H_
#define _PS2_DRIVER_H_

#include "type.h"
#include "keyboard.h"

#define PS2Data 0x60
#define PS2StatusCmd 0x64

struct ps2_driver
{
    // Attributs
    k_uint16_t      data_port;
    k_uint16_t      status_port;
    struct keyboard keyboard;
    int             keyboard_port;

    // Methods
    int         (*init)(struct ps2_driver *self);

    k_uint8_t   (*read_byte)(struct ps2_driver *self);
    bool        (*has_data)(struct ps2_driver *self);
    void        (*send_byte)(struct ps2_driver *self, k_uint8_t c);
};

void ps2_driver_constructor(struct ps2_driver *self);

#endif /* _PS2_DRIVER_H_ */
