#ifndef I2C_HEADER
#define I2C_HEADER

#include <stdint.h>
#include <stdbool.h>
#include "i2c_reg.h"
/**
 * @brief base I2C driver prescaler values
*/
typedef enum
{
    I2C_PRESCALER_1  = 0x00,
    I2C_PRESCALER_4  = 0x01,
    I2C_PRESCALER_16 = 0x02,
    I2C_PRESCALER_64 = 0x03,
} i2c_prescaler_t;

/**
 * @brief gives exhaustive configuration needed by I2C driver
*/
typedef struct
{
    uint8_t baudrate;           /**< Baudrate to be fed into bit rate generator register (final baudrate also depends on prescaler)   */
    i2c_prescaler_t prescaler;  /**< TWI clock based on main CPU clock, divided by prescaler                                          */
    uint8_t slave_address;      /**< Address to which this device will respond                                                        */
    bool general_call_enabled;  /**< Activate the response to general call (0x00) or not                                              */
    bool interrupt_enabled;     /**< Use the interrupt-based workflow or not (if not, i2c_process() will have to be called regularly) */
    i2c_handle_t handle;        /**< Handle which will be used to effectively interact with peripheral                                */
} i2c_config_t;

/**
 * @brief generic structure used to handle common I2C functions returned
*/
typedef enum
{
    I2C_ERROR_OK,                /**< Operation is successful                               */
    I2C_ERROR_NULL_POINTER,      /**< One or more input pointer is set to NULL              */
    I2C_ERROR_NULL_HANDLE,       /**< Given handle is not initialised with real addresses   */
    I2C_ERROR_DEVICE_NOT_FOUND,  /**< Given device id is out of range                       */
} i2c_error_t;

typedef enum
{
    I2C_STATE_READY,                /**< Configured and enabled                                  */
    I2C_STATE_NOT_INITIALISED,      /**< Not configured, need to call i2c_init first             */
    I2C_STATE_DISABLED,             /**< Configured, but peripheral disabled                     */
    /* I2C busy states */
    I2C_STATE_SLAVE_RECEIVING,      /**< Peripheral is currently receiving data as slave device  */
    I2C_STATE_SLAVE_TRANSMITTING,   /**< Peripheral is currently sending data as slave device    */
    I2C_STATE_MASTER_RECEIVING,     /**< Peripheral is currently receiving data as master device */
    I2C_STATE_MASTER_TRANSMITTING,  /**< Peripheral is currently sending data as master device   */
    I2C_STATE_PERIPHERAL_ERROR      /**< Peripheral encountered errors and alerts application    */
} i2c_state_t;

/**
 * @brief gets a default configuration for I2C driver, with non initialised handle (you have to manually input right register addresses)
 * @param[out]  config  : default configuration output
 * @return i2c_error_t :
 *      I2C_ERROR_OK            : Operation succeeded
 *      I2C_ERROR_NULL_POINTER  : Uninitialised pointer parameter
*/
i2c_error_t i2c_get_default_config(i2c_config_t * const config);

/**
 * @brief initialises targeted instance of I2C driver with provided configuration object.
 * Note that handle field of the config object has to be initialised set before this function is called to prevent
 * getting an error about unitialised handle.
 * @param[in]   id      : selected I2C driver instance to be configured
 * @param[in]   config  : configuration representation used to configure physical TWI peripheral
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_POINTER       : Uninitialised pointer parameter
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_init(const uint8_t id, const i2c_config_t * const config);

/**
 * @brief restores selected peripheral's configuration back to default and disable it
 * @param[in]   id      : selected I2C driver instance to be configured
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_deinit(const uint8_t id);

/**
 * @brief powers up selected device (no need for it to be configured or not)
 * @param[in]   id      : selected I2C driver instance to be configured
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_enable(const uint8_t id);

/**
 * @brief switches off selected device (no need for it to be configured or not)
 * @param[in]   id      : selected I2C driver instance to be configured
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_disable(const uint8_t id);

/**
 * @brief registers the given function into selected I2C driver command handler used when receiving commands as a slave device
 * @param[in]   id                          : selected I2C driver instance to be configured
 * @param[in]   i2c_slave_command_handler   : command handler which will be used to handle incoming data when working as a slave device
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_POINTER       : Uninitialised pointer parameter
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_slave_set_command_handler(const uint8_t id, void (*i2c_slave_command_handler)(uint8_t data_byte));

/**
 * @brief sets the selected device I2C slave address to which it is meant to respond
 * @param[in]   id      : selected I2C driver instance to be configured
 * @param[in]   address : slave address of the selected device
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not write to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_set_slave_address(const uint8_t id, const uint8_t address);
i2c_error_t i2c_get_slave_address(const uint8_t id, uint8_t * const address);
i2c_error_t i2c_set_prescaler(const uint8_t id, const i2c_prescaler_t prescaler);
i2c_error_t i2c_get_prescaler(const uint8_t id, i2c_prescaler_t * const prescaler);
i2c_error_t i2c_set_baudrate(const uint8_t id, const uint8_t baudrate);
i2c_error_t i2c_get_baudrate(const uint8_t id, uint8_t * const baudrate);
i2c_error_t i2c_set_general_call_enabled(const uint8_t id, const bool general_call_enabled);
i2c_error_t i2c_get_general_call_enabled(const uint8_t id, bool * const general_call_enabled);
i2c_error_t i2c_set_interrupt_mode(const uint8_t id, const bool use_interrupts);
i2c_error_t i2c_get_interrupt_mode(const uint8_t id, bool * const use_interrupts);


i2c_state_t i2c_write(const uint8_t id, const uint8_t * const payload, const uint8_t length);
i2c_state_t i2c_read(const uint8_t id, uint8_t * const buffer, const uint8_t length);


#endif /* I2C_HEADER */