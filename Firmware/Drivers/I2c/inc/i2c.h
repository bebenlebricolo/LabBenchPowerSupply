#ifndef I2C_HEADER
#define I2C_HEADER

#include <stdint.h>
#include <stdbool.h>
#include "i2c_reg.h"

#define I2C_CMD_WRITE_BIT 0U
#define I2C_CMD_READ_BIT  1U


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
    I2C_ERROR_OK,                 /**< Operation is successful                                                  */
    I2C_ERROR_NULL_POINTER,       /**< One or more input pointer is set to NULL                                 */
    I2C_ERROR_NULL_HANDLE,        /**< Given handle is not initialised with real addresses                      */
    I2C_ERROR_DEVICE_NOT_FOUND,   /**< Given device id is out of range                                          */
    I2C_ERROR_INVALID_ADDRESS,    /**< Given address is out of conventional I2C addresses range                 */
    I2C_ERROR_WRONG_STATE,        /**< Targeted device is in a wrong internal state                             */
    I2C_ERROR_MAX_RETRIES_HIT,    /**< Too much errors were encountered, maximum allowed retries count was hit  */
    I2C_ERROR_ALREADY_PROCESSING, /**< Not really an error : indicates driver is busy and get_state() might be  */
                                    /* called to know which state the I2C driver is running on                  */
} i2c_error_t;

typedef enum
{
    I2C_STATE_DISABLED              = 0, /**< Configured, but peripheral disabled                     */
    I2C_STATE_NOT_INITIALISED,           /**< Not configured, need to call i2c_init first             */
    I2C_STATE_READY,                     /**< Configured and enabled                                  */
    /* I2C busy states */
    I2C_STATE_SLAVE_RECEIVING,           /**< Peripheral is currently receiving data as slave device  */
    I2C_STATE_SLAVE_TRANSMITTING,        /**< Peripheral is currently sending data as slave device    */
    I2C_STATE_MASTER_RECEIVING,          /**< Peripheral is currently receiving data as master device */
    I2C_STATE_MASTER_TRANSMITTING,       /**< Peripheral is currently sending data as master device   */
    I2C_STATE_PERIPHERAL_ERROR           /**< Peripheral encountered errors and alerts application    */
} i2c_state_t;


/* #############################################################################################
   ######################################## Configuration API ##################################
   ############################################################################################# */


/**
 * @brief gets a default configuration for I2C driver, with non initialised handle (you have to manually input right register addresses)
 * @param[out]  config  : default configuration output
 * @return i2c_error_t :
 *      I2C_ERROR_OK            : Operation succeeded
 *      I2C_ERROR_NULL_POINTER  : Uninitialised pointer parameter
*/
i2c_error_t i2c_get_default_config(i2c_config_t * const config);

/**
 * @brief sets the selected device I2C registers handle
 * @param[in]   id      : selected I2C driver instance to be configured
 * @param[in]   handle  : contains pointers to hardware registers
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_POINTER        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_set_handle(const uint8_t id, const i2c_handle_t * const handle);

/**
 * @brief copies internal handle to given output object
 * @param[in]   id      : selected I2C driver instance to be configured
 * @param[out]  handle  : contains pointers to hardware registers
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_POINTER        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_get_handle(const uint8_t id, i2c_handle_t * const handle);


/* #############################################################################################
   ######################################## Single shot registers manipulators #################
   ############################################################################################# */

/**
 * @brief sets the selected device I2C slave address to which it is meant to respond
 * @param[in]   id      : selected I2C driver instance to be configured
 * @param[in]   address : slave address of the selected device
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_set_slave_address(const uint8_t id, const uint8_t address);

/**
 * @brief reads the selected I2C device slave address from registers
 * @param[in]   id      : selected I2C driver instance to be configured
 * @param[out]   address : slave address of the selected device
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_POINTER       : Uninitialised pointer parameter
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_get_slave_address(const uint8_t id, uint8_t * const address);

/**
 * @brief sets the selected device I2C prescaler register
 * @param[in]   id          : selected I2C driver instance to be configured
 * @param[in]   prescaler   : prescaler value written to device
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_set_prescaler(const uint8_t id, const i2c_prescaler_t prescaler);

/**
 * @brief reads the selected device's prescaler from hardware register
 * @param[in]   id          : selected I2C driver instance to be configured
 * @param[out]  prescaler   : prescaler value
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_POINTER       : Uninitialised pointer parameter
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_get_prescaler(const uint8_t id, i2c_prescaler_t * const prescaler);

/**
 * @brief sets the baudrate of selected I2C device
 * @param[in]   id          : selected I2C driver instance to be configured
 * @param[in]   baudrate    : baudrate value
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_set_baudrate(const uint8_t id, const uint8_t baudrate);

/**
 * @brief reads the baudrate of selected I2C device from hardware register
 * @param[in]   id          : selected I2C driver instance to be configured
 * @param[out]  baudrate    : baudrate value
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_POINTER       : Uninitialised pointer parameter
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_get_baudrate(const uint8_t id, uint8_t * const baudrate);

/**
 * @brief sets general call flag bit in selected I2C device's registers
 * @param[in]   id                      : selected I2C driver instance to be configured
 * @param[in]   general_call_enabled    : whether general call feature is enabled or not
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_set_general_call_enabled(const uint8_t id, const bool general_call_enabled);

/**
 * @brief reads general call flag bit from selected I2C device's registers
 * @param[in]   id                      : selected I2C driver instance to be configured
 * @param[out]  general_call_enabled    : whether general call feature is enabled or not
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_POINTER       : Uninitialised pointer parameter
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_get_general_call_enabled(const uint8_t id, bool * const general_call_enabled);

/**
 * @brief configure selected device processing mode : either interrupt-based or non-interrupt-based
 * Note : one will need to use i2c_process() in ordrer to use I2C device in non-interrupt-based mode
 * @param[in]   id             : selected I2C driver instance to be configured
 * @param[in]   use_interrupts : tells whether driver is used in interrupt-based processing mode or non-interrupt-based
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_set_interrupt_mode(const uint8_t id, const bool use_interrupts);

/**
 * @brief reads selected device processing mode : either interrupt-based or non-interrupt-based
 * @param[in]   id             : selected I2C driver instance to be configured
 * @param[out]  use_interrupts : tells whether driver is used in interrupt-based processing mode or non-interrupt-based
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_POINTER       : Uninitialised pointer parameter
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_get_interrupt_mode(const uint8_t id, bool * const use_interrupts);

/**
 * @brief reads selected device status code from TWSR register
 * @param[in]   id          : selected I2C driver instance to be configured
 * @param[out]  status_code : status code being written down to registers by hardware
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_POINTER       : Uninitialised pointer parameter
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_get_status_code(const uint8_t id, uint8_t * const status_code);


/* #############################################################################################
   ######################################## General API ########################################
   ############################################################################################# */

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
i2c_error_t i2c_slave_set_command_handler(const uint8_t id, void (*i2c_slave_command_handler)(uint8_t * const data_byte));


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
 * @brief returns selected driver instance's state machine current state
 * @param[in]   id    : selected I2C driver instance to be configured
 * @param[out]  state : current internal state of selected driver state machine
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_POINTER       : Uninitialised pointer parameter
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_get_state(const uint8_t id, i2c_state_t * const state);

/**
 * @brief this function shall be used when non-interrupt mode is used
 * It basically checks driver current state and performs actions whenever required
 * E.g: when performing an I2C write action :
 * my_module.c :
 *   i2c_write(0, 0x21, buffer, 26);
 *   -> copies the buffer address internally and posts a write operation if
 *      device is ready (@see get_state(id))
 *
 * main.c, in infinite loop:
 *  i2c_process();
 *  -> Will handle write operation triggered by my_module.c and as soon as I2C peripheral
 *     returns from its busy state (success or failure), next operation in the stack is triggered.
 *     In this example, execution flow will go as this :
 *     1 - i2c_process() sends a Start condition as master Transmitter mode and returns
 *     2 - next call will assert Peripheral is ready (TWINT is set), if so : write device's address on bus and return
 *     3 - next call will assert Peripheral is ready, if so : send first byte of data in buffer and return
 *     4 - will proceed as previous line until reaching the end of data buffer
 *     5 - will finally send a Stop condition on I2C bus and put driver back in its I2C_STATE_READY state
 * @return i2c_error_t
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_process(const uint8_t id);

/**
 * @brief writes data to I2C bus
 * @param[in]   id              : selected I2C driver instance to be configured
 * @param[in]   target_address  : targeted slave address on I2C bus
 * @param[in]   buffer          : pointer to a buffer of bytes holding the actual payload to be sent over I2C bus
 * @param[in]   length          : total length of given buffer
 * @param[in]   retries         : number of available tries before giving up
 * @return i2c_error_t :
 *      I2C_ERROR_OK                  : Operation succeeded
 *      I2C_ERROR_NULL_POINTER        : Uninitialised pointer parameter
 *      I2C_ERROR_NULL_HANDLE         : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND    : Selected instance id does not exist in available instances
 *      I2C_ERROR_ALREADY_PROCESSING  : Selected instance is already processing (either in master or slave mode). @see i2c_get_state()
*/
i2c_error_t i2c_write(const uint8_t id, const uint8_t target_address , const uint8_t * const buffer, const uint8_t length, const uint8_t retries);

/**
 * @brief reads data to I2C bus
 * @param[in]   id              : selected I2C driver instance to be configured
 * @param[in]   target_address  : targeted slave address on I2C bus (address is not checked at runtime, must not bit greater than 127)
 * @param[out]  buffer          : pointer to a buffer of bytes holding the actual payload to be sent over I2C bus
 * @param[in]   length          : total length of given buffer
 * @param[in]   retries         : number of available tries before giving up
 * @return i2c_error_t :
 *      I2C_ERROR_OK                  : Operation succeeded
 *      I2C_ERROR_NULL_POINTER        : Uninitialised pointer parameter
 *      I2C_ERROR_NULL_HANDLE         : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND    : Selected instance id does not exist in available instances
 *      I2C_ERROR_ALREADY_PROCESSING  : Selected instance is already processing (either in master or slave mode). @see i2c_get_state()
*/
i2c_error_t i2c_read(const uint8_t id, const uint8_t target_address, uint8_t * const buffer, const uint8_t length, const uint8_t retries);


#endif /* I2C_HEADER */