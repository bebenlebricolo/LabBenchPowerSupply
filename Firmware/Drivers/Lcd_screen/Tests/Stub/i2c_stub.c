#include "i2c.h"
#include <string.h>

/* Data section */
i2c_stub_buffer_t i2c_stub_buffer = {0};
static bool is_new = false;
static bool force_error_on_next_calls = false;
static i2c_error_t next_error = I2C_ERROR_OK;


/* Functions defition */

void i2c_stub_clear(void)
{
    force_error_on_next_calls = false;
    next_error = I2C_ERROR_OK;
    reset_i2c_stub_buffer();
}

void i2c_stub_force_error_on_next_calls(const i2c_error_t p_next_error)
{
    force_error_on_next_calls = true;
    next_error = p_next_error;
}

i2c_error_t i2c_get_state(const uint8_t id, i2c_state_t * const state)
{
    (void) id;
    *state = I2C_STATE_READY;
    return I2C_ERROR_OK;
}


i2c_error_t i2c_write(const uint8_t id, const uint8_t target_address , uint8_t * const buffer, const uint8_t length, const uint8_t retries){
    (void) id;
    (void) target_address;
    (void) retries;

    i2c_stub_buffer.buffer = buffer;
    i2c_stub_buffer.length = length;
    is_new = true;

    if(force_error_on_next_calls)
    {
        return next_error;
    }
    return I2C_ERROR_OK;
}

bool i2c_stub_get_buffer_content(const uint8_t index, uint8_t * const value, bool * const is_new_value)
{
    if (index >= i2c_stub_buffer.length)
    {
        return false;
    }

    *value = i2c_stub_buffer.buffer[index];
    *is_new_value = is_new;

    // Consume is_new so that we need a new call to i2c_write to enable is_new back again
    is_new = false;

    return true;
}

bool i2c_stub_data_was_sent(void)
{
    bool out = is_new;
    is_new = false;
    return out;
}



void reset_i2c_stub_buffer(void)
{
    memset(&i2c_stub_buffer, 0, sizeof(i2c_stub_buffer_t));
}