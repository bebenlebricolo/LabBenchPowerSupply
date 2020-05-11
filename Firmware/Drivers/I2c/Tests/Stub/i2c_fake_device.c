#include "i2c_fake_device.h"
#include <string.h>

#define MAX_MESSAGE_LENGTH 30U

typedef struct
{
    uint8_t current_byte;
    char msg[MAX_MESSAGE_LENGTH];
    uint8_t temperature_1;
    uint8_t temperature_2;
    i2c_fake_device_operating_modes_t mode;
    i2c_fake_device_commands_t command;
    uint8_t address;
} i2c_interface_t;


static i2c_interface_t interface = {0};
static i2c_fake_device_mode_t mode = MODE_IDLE;

void i2c_fake_device_clear(void)
{
    memset(&interface, 0, sizeof(i2c_interface_t));
    mode = MODE_IDLE;
    snprintf(interface.msg, MAX_MESSAGE_LENGTH, "Hello World!" );
}

void i2c_fake_device_init(const uint8_t address)
{
    interface.address = address;
    mode = MODE_IDLE;
}

void i2c_fake_device_set_mode(const i2c_fake_device_mode_t mode)
{
    interface.mode = mode;
}


void i2c_fake_device_process(void)
{
    switch(mode)
    {
        case MODE_MASTER_TRANSMITTER:
            break;
        case MODE_MASTER_RECEIVER:
            break;
        case MODE_SLAVE_TRANSMITTER:
            break;
        case MODE_SLAVE_RECEIVER:
            break;
        default:
            break;
    }
}

uint8_t * i2c_fake_device_get_current_byte(void)
{
    return &interface.current_byte;
}

