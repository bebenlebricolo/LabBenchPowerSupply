#include "i2c_fake_slave_application_data.h"

#include <stddef.h>
#include <string.h>

static i2c_fake_slave_application_data_exposed_data_t exposed_data = {0};


void i2c_fake_slave_application_data_clear(void)
{
    memset(&exposed_data, 0, sizeof(i2c_fake_slave_application_data_exposed_data_t));
}

i2c_slave_handler_error_t i2c_fake_slave_command_handler(volatile i2c_command_handling_buffers_t * buffer_data, uint8_t command_byte)
{
    i2c_slave_handler_error_t ret = I2C_SLAVE_HANDLER_ERROR_OK;
    if (NULL == buffer_data)
    {
        return I2C_SLAVE_HANDLER_ERROR_BUFFER_NULLPTR;
    }

    switch(command_byte)
    {
        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_FAN_SPEED:
            buffer_data->data[0] = exposed_data.fan_speed;
            buffer_data->length = 1;
            break;

        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_BYTE_ARRAY:
            memcpy(buffer_data->data, exposed_data.byte_array, I2C_FAKE_SLAVE_APPLICATION_DATA_MAX_BYTE_ARRAY_LENGTH);
            buffer_data->length = I2C_FAKE_SLAVE_APPLICATION_DATA_MAX_BYTE_ARRAY_LENGTH;
            break;

        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_MAX_CURRENT:
            buffer_data->data[0] = exposed_data.max_current;
            buffer_data->length = 1;
            break;

        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_MAX_VOLTAGE:
            buffer_data->data[0] = exposed_data.max_voltage;
            buffer_data->length = 1;
            break;

        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_ENABLED:
            buffer_data->data[0] = exposed_data.enabled;
            buffer_data->length = 1;
            break;

        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_UNDEFINED:
        default:
            memset(buffer_data->data, 0, I2C_MAX_BUFFER_SIZE);
            buffer_data->length = 0;
            ret = I2C_SLAVE_HANDLER_ERROR_UNKNOWN_COMMAND;
            break;
    }

    return ret;
}


i2c_fake_slave_application_data_exposed_data_t * i2c_fake_slave_application_data_get_exposed_data(void)
{
    return &exposed_data;
}
