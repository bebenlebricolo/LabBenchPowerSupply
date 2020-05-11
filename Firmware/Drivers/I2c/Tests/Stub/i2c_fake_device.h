#ifndef I2C_FAKE_DEVICE_HEADER
#define I2C_FAKE_DEVICE_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "i2c_device_interface.h"

/* Device operating modes */
typedef enum
{
    I2C_FAKE_DEVICE_OPERATING_MODE_FULL_CONTROL, /**< Target mode might be the full controlled mode                         */
    I2C_FAKE_DEVICE_OPERATING_MODE_TEMP_1_ONLY,  /**< Target mode only activates regulation based on Temperature 1 sensor   */
    I2C_FAKE_DEVICE_OPERATING_MODE_TEMP_2_ONLY,  /**< Target mode only activates regulation based on Temperature 2 sensor   */
    I2C_FAKE_DEVICE_OPERATING_MODE_FREE_WHEEL,   /**< Target mode disables all active regulation of temperature             */
    I2C_FAKE_DEVICE_OPERATING_MODE_UNKNOWN,
}i2c_fake_device_operating_modes_t;

/* Device op-codes */
typedef enum
{
    I2C_FAKE_DEVICE_CMD_MESSAGE,            /**< access to a message being posted by the fake device (let's say "Hello World!")     */
    I2C_FAKE_DEVICE_CMD_THERMAL_THRESHOLD,  /**< Exposes a Thermal threshold value on I2C bus                                       */
    I2C_FAKE_DEVICE_CMD_TEMPERATURE_1,      /**< Exposes the temperature read from sensor 1 on I2C bus                              */
    I2C_FAKE_DEVICE_CMD_TEMPERATURE_2,      /**< Exposes the temperature read from sensor 2 on I2C bus                              */
    I2C_FAKE_DEVICE_CMD_MODE_CHANGE,        /**< Gives the ability to change I2C fake device operating mode, @see Operating Modes   */
    I2C_FAKE_DEVICE_CMD_UNKNOWN
} i2c_fake_device_commands_t;

/* Let the device know what kind of operation it is expected to perform at next process() call */
typedef enum
{
    MODE_MASTER_TRANSMITTER,
    MODE_MASTER_RECEIVER,
    MODE_SLAVE_TRANSMITTER,
    MODE_SLAVE_RECEIVER,
    MODE_IDLE
} i2c_fake_device_mode_t;


/* Interface */
void i2c_fake_device_init(const uint8_t address);
void i2c_fake_device_clear(void);
void i2c_fake_device_process(void);
void i2c_fake_device_set_mode(const i2c_fake_device_mode_t mode);
void i2c_fake_device_get_interface(i2c_device_interface_t ** const interface);

#ifdef __cplusplus
}
#endif

#endif /* I2C_FAKE_DEVICE_HEADER */