#ifndef TARGETED_SLAVE_I2C_INTERFACE_HEADER
#define TARGETED_SLAVE_I2C_INTERFACE_HEADER
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

#endif /* TARGETED_SLAVE_I2C_INTERFACE_HEADER */