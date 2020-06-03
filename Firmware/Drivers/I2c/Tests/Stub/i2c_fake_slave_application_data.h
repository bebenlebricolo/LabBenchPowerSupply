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
 * @brief handles the first byte of a read/write operation when configured as a slave
 * @param[out] buffer_data  : Will point to the right I2C exposed buffer if command encoded in the 'command_byte' parameter is valid
 * @param[in]  command_byte : Contains the encoded command which will be used to initialise buffer_data correctly
*/
i2c_slave_handler_error_t i2c_fake_slave_command_handler(volatile i2c_command_handling_buffers_t * buffer_data, uint8_t command_byte);

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