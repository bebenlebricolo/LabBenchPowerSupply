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

    buffer_data->locked = &exposed_data.locked;
    switch(command_byte)
    {
        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_FAN_SPEED:
            buffer_data->data = (uint8_t*) &exposed_data.fan_speed;
            buffer_data->length = 1;
            break;
        
        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_BYTE_ARRAY:
            buffer_data->data = (uint8_t*) &exposed_data.byte_array;
            buffer_data->length = I2C_FAKE_SLAVE_APPLICATION_DATA_MAX_BYTE_ARRAY_LENGTH;
            break;
        
        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_MAX_CURRENT:
            buffer_data->data = (uint8_t*) &exposed_data.max_current;
            buffer_data->length = 1;
            break;
        
        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_MAX_VOLTAGE:
            buffer_data->data = (uint8_t*) &exposed_data.max_voltage;
            buffer_data->length = 1;
            break;
        
        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_ENABLED:
            buffer_data->data = (uint8_t*) &exposed_data.enabled;
            buffer_data->length = 1;
            break;
        
        case I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_UNDEFINED:
        default:
            buffer_data->data = NULL;
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
