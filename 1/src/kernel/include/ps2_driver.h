#ifndef PS2_DRIVER_H
#define PS2_DRIVER_H

#include "type.h"

#define PS2Data 0x60
#define PS2StatusCmd 0x64
#define bit(x) (1U << (x))


struct ps2_driver
{
    k_uint16_t data_port;
    k_uint16_t status_port;

    k_uint8_t (*read_byte)(struct ps2_driver *self);
    bool (*has_data)(struct ps2_driver *self);
    void (*send_byte)(struct ps2_driver *self, k_uint8_t c);
};

/* Fonctions d'initialisation/destruction */
void ps2_driver_init(struct ps2_driver *self);

#endif /* PS2_DRIVER_H */
