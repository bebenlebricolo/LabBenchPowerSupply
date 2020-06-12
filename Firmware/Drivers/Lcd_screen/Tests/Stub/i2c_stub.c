#include "i2c.h"
#include <stdbool.h>

static bool force_error_on_next_calls = false;

void i2c_stub_clear(void)
{
    force_error_on_next_calls = false;
}

void i2c_stub_force_error_on_next_calls(void)
{
    force_error_on_next_calls = true;
}

i2c_error_t i2c_get_state(const uint8_t id, i2c_state_t * const state)
{
    (void) id;
    if(force_error_on_next_calls)
    {
        return I2C_ERROR_INVALID_ADDRESS;
    }

    *state = I2C_STATE_READY;

    return I2C_ERROR_OK;
}


i2c_error_t i2c_write(const uint8_t id, const uint8_t target_address , uint8_t * const buffer, const uint8_t length, const uint8_t retries)
{
    (void) id;
    (void) target_address;
    (void) buffer;
    (void) length;
    (void) retries;

    if(force_error_on_next_calls)
    {
        return I2C_ERROR_INVALID_ADDRESS;
    }
    return I2C_ERROR_OK;
}