#include "twi_hardware_stub.h"
#include "i2c_register_stub.h"

void twi_hardware_stub_process(const uint8_t id)
{
    /* Clear twint flag as if the real hardware has done it */
    i2c_register_stub[id].twcr_reg &= ~TWINT_MSK;
}