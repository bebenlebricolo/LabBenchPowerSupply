#include "i2c_register_stub.h"
#include "i2c.h"
#include "string.h"

i2c_register_stub_t i2c_register_stub[I2C_DEVICES_COUNT] = {0};

void i2c_register_stub_erase(const uint8_t id)
{
    memset(&i2c_register_stub[id], 0, sizeof(i2c_register_stub_t));
}

void i2c_register_stub_init_handle(const uint8_t id, i2c_handle_t * handle)
{
    i2c_register_stub_t * stub = &i2c_register_stub[id];
    handle->TWCR = &stub->twcr_reg;
    handle->TWBR = &stub->twbr_reg;
    handle->TWDR = &stub->twdr_reg;
    handle->TWSR = &stub->twsr_reg;
    handle->TWAR = &stub->twar_reg;
    handle->TWAMR = &stub->twamr_reg;
}
