/*

------------------
@<FreeMyCode>
FreeMyCode version : 1.0 RC alpha
    Author : bebenlebricolo
    License : 
        name : GPLv3
        url : https://www.gnu.org/licenses/quick-guide-gplv3.html
    Date : 12/02/2021
    Project : LabBenchPowerSupply
    Description : The Lab Bench Power Supply provides a simple design based around an Arduino Nano board to convert AC main voltage into
 smaller ones, ranging from 0V to 16V, with voltage and current regulations
<FreeMyCode>@
------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "i2c_fake_slave_application_data.h"

#include <stddef.h>
#include <string.h>

#include "memutils.h"

static i2c_fake_slave_application_data_exposed_data_t exposed_data = {0};

// Stores the last command byte in order to correctly update internal memory
// once a master finishes to write data to this slave

static struct
{
    struct
    {
        uint8_t index;
        uint8_t length;
    } virtual_buffer;
    i2c_fake_slave_application_data_available_commands_t command;
    uint8_t processed_bytes;
    i2c_request_t last_request;
} tracking;

// Static handlers for read and write operations
static i2c_slave_handler_error_t slave_data_handler_read(uint8_t * byte);
static i2c_slave_handler_error_t slave_data_handler_write(uint8_t * byte);

void i2c_fake_slave_application_init(void)
{
    i2c_fake_slave_application_data_clear();
}

void i2c_fake_slave_application_data_clear(void)
{
    memset(&exposed_data, 0, sizeof(i2c_fake_slave_application_data_exposed_data_t));
    tracking.command = I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_UNDEFINED;
    tracking.processed_bytes = 0;
    tracking.virtual_buffer.index = 0;
    tracking.virtual_buffer.length = 0;
    tracking.last_request = I2C_REQUEST_IDLE;
}

i2c_slave_handler_error_t i2c_fake_slave_command_handler(uint8_t * byte, const i2c_request_t request)
{
    i2c_slave_handler_error_t ret = I2C_SLAVE_HANDLER_ERROR_OK;
    if (NULL == byte)
    {
        return I2C_SLAVE_HANDLER_ERROR_BUFFER_NULLPTR;
    }

    if (request == I2C_REQUEST_READ)
    {
        return slave_data_handler_read(byte);
    }
    else
    {
        return slave_data_handler_write(byte);
    }

    return ret;
}

i2c_slave_handler_error_t i2c_fake_slave_transmission_over_callback(void)
{
    // Do not reset last command byte as we can have a master TX and then a master RX for the same command byte
    tracking.virtual_buffer.index = 0;

    // Reset last request as well, so that we can determine at next data handler call
    // that this is a new transmission
    tracking.last_request = I2C_REQUEST_IDLE;

    return I2C_SLAVE_HANDLER_ERROR_OK;
}

i2c_fake_slave_application_data_exposed_data_t * i2c_fake_slave_application_data_get_exposed_data(void)
{
    return &exposed_data;
}

i2c_slave_handler_error_t slave_data_handler_read(uint8_t * byte)
{
    i2c_slave_handler_error_t ret = I2C_SLAVE_HANDLER_ERROR_OK;
    if ( (0 != tracking.processed_bytes)
    &&  ( tracking.virtual_buffer.index >= tracking.virtual_buffer.length))
    {
        return I2C_SLAVE_HANDLER_ERROR_BUFFER_OVERFLOW_GUARD;
    }

    // Start of a new transmission
    if (tracking.last_request != I2C_REQUEST_READ)
    {
        tracking.processed_bytes = 0;
        tracking.virtual_buffer.index = 0;
        tracking.virtual_buffer.length = 0;
        tracking.last_request = I2C_REQUEST_READ;
        tracking.command = I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_UNDEFINED;
    }

    // First byte read from master is a command byte for this application
    if ((0 == tracking.virtual_buffer.index)
    &&  (0 == tracking.processed_bytes))
    {
        tracking.command = (i2c_fake_slave_application_data_available_commands_t) *byte;
        switch(tracking.command)
        {
            case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_FAN_SPEED:
                tracking.virtual_buffer.length = 1;
                break;

            case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_BYTE_ARRAY:
                tracking.virtual_buffer.length = I2C_FAKE_SLAVE_APPLICATION_DATA_MAX_BYTE_ARRAY_LENGTH;
                break;

            case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_MAX_CURRENT:
                tracking.virtual_buffer.length = 1;
                break;

            case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_MAX_VOLTAGE:
                tracking.virtual_buffer.length = 1;
                break;

            case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_ENABLED:
                tracking.virtual_buffer.length = 1;
                break;

            // Not a valid command bro !
            case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_UNDEFINED:
            default:
                ret = I2C_SLAVE_HANDLER_ERROR_UNKNOWN_COMMAND;
                break;
        }

        tracking.processed_bytes++;
    }
    // Handle the read command now
    else
    {
        if (tracking.virtual_buffer.index < tracking.virtual_buffer.length)
        {
            switch(tracking.command)
            {
                case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_FAN_SPEED:
                    exposed_data.fan_speed = *byte;
                    break;

                // Setup buffer_data (flat i2c buffer used as a data interface) to point to the application data
                case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_BYTE_ARRAY:
                    exposed_data.byte_array[tracking.virtual_buffer.index] = *byte;
                    break;

                case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_MAX_CURRENT:
                    exposed_data.max_current = *byte;
                    break;

                case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_MAX_VOLTAGE:
                    exposed_data.max_voltage = *byte;
                    break;

                case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_ENABLED:
                    exposed_data.enabled = (bool) *byte;
                    break;

                case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_UNDEFINED:
                default:
                    tracking.virtual_buffer.index = 0;
                    tracking.virtual_buffer.length = 0;
                    ret = I2C_SLAVE_HANDLER_ERROR_UNKNOWN_COMMAND;
                    break;
            }

            // If everything is right at that point, don't forget to increment
            // internal tracking index
            if (I2C_SLAVE_HANDLER_ERROR_OK == ret)
            {
                tracking.virtual_buffer.index++;
                tracking.processed_bytes++;
            }
        }
        else
        {
            ret = I2C_SLAVE_HANDLER_ERROR_BUFFER_OVERFLOW_GUARD;
        }

    }


    return ret;
}

i2c_slave_handler_error_t slave_data_handler_write(uint8_t * byte)
{
    i2c_slave_handler_error_t ret = I2C_SLAVE_HANDLER_ERROR_OK;
    if (tracking.virtual_buffer.index >= tracking.virtual_buffer.length)
    {
        return I2C_SLAVE_HANDLER_ERROR_BUFFER_OVERFLOW_GUARD;
    }

    // Start of a new transmission
    // Don't touch at the command byte as this time this slave has to remember what
    // was the requested memory area
    if (tracking.last_request != I2C_REQUEST_WRITE)
    {
        tracking.processed_bytes = 0;
        tracking.virtual_buffer.index = 0;
        tracking.last_request = I2C_REQUEST_WRITE;
    }

    switch(tracking.command)
    {
        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_FAN_SPEED:
            *byte = exposed_data.fan_speed;
            break;

        // Setup buffer_data (flat i2c buffer used as a data interface) to point to the application data
        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_BYTE_ARRAY:
            *byte = exposed_data.byte_array[tracking.virtual_buffer.index];
            break;

        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_MAX_CURRENT:
            *byte = exposed_data.max_current;
            break;

        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_MAX_VOLTAGE:
            *byte = exposed_data.max_voltage;
            break;

        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_ENABLED:
            *byte = exposed_data.enabled;
            break;

        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_UNDEFINED:
        default:
            *byte = 0;
            tracking.virtual_buffer.index = 0;
            tracking.virtual_buffer.length = 0;
            ret = I2C_SLAVE_HANDLER_ERROR_UNKNOWN_COMMAND;
            break;
    }

    // If everything is right at that point, don't forget to increment
    // internal tracking index
    if (I2C_SLAVE_HANDLER_ERROR_OK == ret)
    {
        tracking.virtual_buffer.index++;
    }


    return I2C_SLAVE_HANDLER_ERROR_OK;;
}
