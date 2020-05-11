#ifndef I2C_SIMULATOR_DEVICE_INTERFACE_HEADER
#define I2C_SIMULATOR_DEVICE_INTERFACE_HEADER

#include <stdint.h>

/* All devices registered on I2C bus simulator should implement this interface */
typedef struct
{
    uint8_t * data;             /**< Points to the internal device's data register                                                  */
    bool Start_sent;            /**< Tells if the device implementing this interface tries to send a Start condition over I2C bus.  */
    bool Stop_sent;             /**< Tells if the device implementing this interface tries to send a Stop condition over I2C bus.   */
    const uint8_t * address;    /**< Gives the current I2C address of targeted device                                               */
} i2c_simulator_device_interface_t;

#endif /* I2C_SIMULATOR_DEVICE_INTERFACE_HEADER */