#include "twi_hardware_stub.h"
#include "i2c_register_stub.h"


static i2c_device_interface_t interface[I2C_DEVICES_COUNT] = {0};
static i2c_register_stub_t previous_registers_state[I2C_DEVICES_COUNT] = {0};


void twi_hardware_stub_process(const uint8_t id)
{
    /* Clear twint flag as if the real hardware has done it */
    i2c_register_stub[id].twcr_reg &= ~TWINT_MSK;

    




}

void twi_hardware_stub_get_interface(const uint8_t bus_id, i2c_device_interface_t ** const p_interface)
{
    *p_interface = &interface[bus_id];
}
