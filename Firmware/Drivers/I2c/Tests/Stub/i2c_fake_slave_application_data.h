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

#ifndef I2C_FAKE_SLAVE_APPLICATION_DATA
#define I2C_FAKE_SLAVE_APPLICATION_DATA

#ifdef __cplusplus
extern "C"
{
#endif

#include "i2c.h"
#define I2C_FAKE_SLAVE_APPLICATION_DATA_MAX_BYTE_ARRAY_LENGTH (30U)

/**
 * @brief Fake commands that could be used to simulate an application firmware code
*/
typedef enum
{
    I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_UNDEFINED,      /**< Default command code, this result in an invalid request (NACK might be returned when device is processed)  */
    I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_FAN_SPEED,      /**< Sets the internal buffer to a FAN_SPEED register                                                           */
    I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_BYTE_ARRAY,     /**< Exposes a multi-bytes register to I2C device (could be anything, e.g. error message, pointsets, etc.)      */
    I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_MAX_CURRENT,    /**< Register controlling maximum current                                                                       */
    I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_MAX_VOLTAGE,    /**< Register controlling maximum voltage                                                                       */
    I2C_FAKE_SLAVE_APPLICATION_DATA_CMD_ENABLED,        /**< Register controlling the behavior of slave device (enabled/disabled)                                       */
} i2c_fake_slave_application_data_available_commands_t;

typedef enum
{
    I2C_FAKE_SLAVE_APPLICATION_DATA_FAN_SPEED_0,
    I2C_FAKE_SLAVE_APPLICATION_DATA_FAN_SPEED_25,
    I2C_FAKE_SLAVE_APPLICATION_DATA_FAN_SPEED_50,
    I2C_FAKE_SLAVE_APPLICATION_DATA_FAN_SPEED_75,
    I2C_FAKE_SLAVE_APPLICATION_DATA_FAN_SPEED_100,
} i2c_fake_slave_application_data_fan_speed_t;

typedef struct
{
    i2c_fake_slave_application_data_fan_speed_t fan_speed;
    uint8_t byte_array[I2C_FAKE_SLAVE_APPLICATION_DATA_MAX_BYTE_ARRAY_LENGTH];
    uint8_t max_current;
    uint8_t max_voltage;
    bool enabled;
    bool locked;
} i2c_fake_slave_application_data_exposed_data_t;

/**
 * @brief initialises internal memory of this fake i2c application
*/
void i2c_fake_slave_application_init(void);

/**
 * @brief handles the first byte of a read/write operation when configured as a slave
 * @param[in/out]   byte    : gives the current byte on which we are working
 * @param[in]       request : gives the current I2C operation from master's point of view
 *                            => when master reads from slave, request shall be set to I2C_REQUEST_READ
*/
i2c_slave_handler_error_t i2c_fake_slave_command_handler(uint8_t * byte, const i2c_request_t request);

/**
 * @brief handles transmission over request from i2c device whenever either a READ or WRITE operation from
 * master of i2c bus is finished
*/
i2c_slave_handler_error_t i2c_fake_slave_transmission_over_callback(void);

/**
 * @brief used to reset data
*/
void i2c_fake_slave_application_data_clear(void);

/**
 * @brief gives access to the i2c exposed data, for unit-testing purposes
*/
i2c_fake_slave_application_data_exposed_data_t * i2c_fake_slave_application_data_get_exposed_data(void);

#ifdef __cplusplus
}
#endif

#endif /* I2C_FAKE_SLAVE_APPLICATION_DATA */