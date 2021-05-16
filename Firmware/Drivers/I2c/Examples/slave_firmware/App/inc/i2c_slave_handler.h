#ifndef I2C_SLAVE_HANDLER_HEADER
#define I2C_SLAVE_HANDLER_HEADER

#include "i2c.h"

/**
 * @brief handles input/output byte from I2C driver.
 * When a read operation (master reads from this slave), current_byte is considered as an output.
 * When a write operation (master writes to this slave), current_byte is considered as an input.
 * The request parameter informs about the nature of the current i2c transaction (read or write requests)
 * @param[in/out]   current_byte : interface byte used by the I2C driver
 * @param[in]       request      : nature of the I2C transaction currently taking place
 * @return I2C_SLAVE_HANDLER_ERROR_OK or any other error from the i2c_slave_handler_error_t enum in case of failure.
*/
i2c_slave_handler_error_t i2c_slave_data_handler(uint8_t * const current_byte, const i2c_request_t request);

/**
 * @brief This callback will be fired by the I2C slave driver whenever a transmission is over in order to reset
 * internal data and states back to default.
*/
i2c_slave_handler_error_t i2c_slave_transmission_over_callback(void);

/**
 * @brief Packs available I2C command bytes for a fictional device.
 * This fictional device provides the following functionalities :
 * * Temperature readings (read only command, write has no effect)
 * * Speed readings (read only command, write has no effect)
 * * Measured output power (read only command, write has no effect)
 * * Operating mode selection (read and write commands)
 *   -> read  : returns the currently selected operating mode (0%, 25%, 50%, 75%, 100%) encoded via unsigned int (0,1,2,3,4)
 *   -> write : allows to modify the operating mode (0%, 25%, 50%, 75%, 100%) using an unsigned int (0,1,2,3,4)
 * * Powers the device ON (write command only, read returns 0)
 * * Powers the device OFF (write command only, read returns 0)
*/
typedef enum
{
    I2C_SLAVE_COMMAND_UNDEFINED,       /**< Default value used to map invalid or unsupported received commands         */
    I2C_SLAVE_COMMAND_MEAS_TEMP,       /**< Exposes temperature measurements on I2C exposed data (read only)           */
    I2C_SLAVE_COMMAND_MEAS_SPEED,      /**< Exposes speed measurements on I2C exposed data (read only)                 */
    I2C_SLAVE_COMMAND_MEAS_POWER,      /**< Exposes power measurements on I2C exposed data (read only)                 */
    I2C_SLAVE_COMMAND_MODE,            /**< Allows to read of modify currently selected operating mode (read/write)    */
    I2C_SLAVE_COMMAND_SWITCH_ON_OFF,   /**< Allows to powercycle the device ON or OFF (write only)                     */
} i2c_slave_command_t;

/**
 * @brief describes the fictional operating modes exposed by this device
*/
typedef enum
{
    I2C_SLAVE_OPERATING_MODE_0 = 0,         /**< 0%             */
    I2C_SLAVE_OPERATING_MODE_25 = 1,        /**< 25%            */
    I2C_SLAVE_OPERATING_MODE_50 = 2,        /**< 50%            */
    I2C_SLAVE_OPERATING_MODE_75 = 3,        /**< 75%            */
    I2C_SLAVE_OPERATING_MODE_100 = 4,       /**< 100%           */
    I2C_SLAVE_OPERATING_MODE_UNDEFINED = 5  /**< Invalid value  */
}i2c_slave_operating_modes_t;

/**
 * @brief safely converts a received byte into the adequate operating mode (decoding)
 * If an invalid code is used (byte), undefined operating mode is returned.
 * @return  I2C_SLAVE_OPERATING_MODE_0-100      : conversion worked
 *          I2C_SLAVE_OPERATING_MODE_UNDEFINED  : invalid input data
*/
i2c_slave_operating_modes_t i2c_slave_operating_modes_from_byte(const uint8_t byte);

/**
 * @brief Slave device exposed data
 * Note : the lock byte is used to tell to the application that data is being used by the slave data handlers
 * and shall not be modified until transaction is over
*/
typedef struct
{
    uint8_t speed;                      /**< mapped from 0 to 100                                                                       */
    int8_t temperature;                 /**< mapped from -80°C to +115°C                                                                */
    uint16_t power;                     /**< mapped from 0 W to 12 kW                                                                   */
    i2c_slave_operating_modes_t mode;   /**< power mode                                                                                 */
    bool enabled;                       /**< device's state (switched on or off)                                                        */
    bool locked;                        /**< tells if the I2C driver is using this structure or not (tells if a transaction is ongoing) */
} i2c_slave_exposed_data_t;

// Data exposed on the I2C bus by the slave application.
extern i2c_slave_exposed_data_t i2c_slave_exposed_data;

#endif /* I2C_SLAVE_HANDLER_HEADER */