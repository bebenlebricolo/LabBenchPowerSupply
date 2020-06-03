#ifndef I2C_FAKE_DEVICE_HEADER
#define I2C_FAKE_DEVICE_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "i2c_device_interface.h"

#define I2C_FAKE_DEVICE_MSG_LEN 30U


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

/**
 * @brief device available error codes
*/
typedef enum
{
    I2C_FAKE_DEVICE_ERROR_OK,
    I2C_FAKE_DEVICE_ERROR_ALREADY_PROCESSING,
    I2C_FAKE_DEVICE_ERROR_UNKNOWN
} i2c_fake_device_error_t;


typedef struct
{
    char msg[I2C_FAKE_DEVICE_MSG_LEN];
    uint8_t temperature_1;
    uint8_t temperature_2;
    uint8_t thermal_threshold;
    i2c_fake_device_operating_modes_t mode;
    i2c_fake_device_commands_t command;
} i2c_exposed_data_t;

/* Interface */
void i2c_fake_device_init(const uint8_t address, const bool general_call_enabled);
void i2c_fake_device_clear(void);

/**
 * @brief this function might be used while testing to inject some ack/nack while master is writing to the targeted slave
*/
void i2c_fake_device_force_nack(void);

i2c_fake_device_error_t i2c_fake_device_write(const uint8_t address, uint8_t * buffer, const uint8_t length, const uint8_t retries);
i2c_fake_device_error_t i2c_fake_device_read(const uint8_t address, uint8_t * buffer, const uint8_t length, const uint8_t retries);

void i2c_fake_device_process(const uint8_t id);
void i2c_fake_device_set_mode(const i2c_fake_device_operating_modes_t mode);
void i2c_fake_device_get_interface(const uint8_t bus_id, i2c_device_interface_t ** const interface);

i2c_exposed_data_t * i2c_fake_device_get_exposed_data(void);

#ifdef __cplusplus
}
#endif

#endif /* I2C_FAKE_DEVICE_HEADER */