#ifndef I2C_DEVICE_INTERFACE_HEADER
#define I2C_DEVICE_INTERFACE_HEADER

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    I2C_ACKNOWLEDGMENT_ACK,
    I2C_ACKNOWLEDGMENT_NACK,
    I2C_ACKNOWLEDGMENT_UNKNOWN
} i2c_acknowledgment_t;

/* All devices registered on I2C bus simulator should implement this interface */
typedef struct
{
    uint8_t data;          /**< Points to the internal device's data register                                                  */
    uint8_t address; /**< Gives the current I2C address of targeted device                                               */
    bool start_sent;       /**< Tells if the device implementing this interface tries to send a Start condition over I2C bus.  */
    bool stop_sent;        /**< Tells if the device implementing this interface tries to send a Stop condition over I2C bus.   */
    bool ack_sent;         /**< An Acknoledged bit was sent (or not) over the bus.                                             */
    bool lost_arbitration; /**< I2C bus determines if this device has lost arbitration or not.                                 */
} i2c_device_interface_t;

typedef void (*i2c_interface_getter_function)(i2c_device_interface_t ** const interface);
typedef void (*i2c_process_function)(const uint8_t id);

/* Common I2C device interface used by I2C Bus simulator to handle its registered devices */
typedef struct
{
    i2c_process_function process;       /**< function to be called on the targeted device at each I2C bus simulator process() call */
    i2c_device_interface_t * interface; /**< targeted device i2c bus interface                                                     */
} i2c_device_stub_t;



#ifdef __cplusplus
}
#endif

#endif /* I2C_DEVICE_INTERFACE_HEADER */