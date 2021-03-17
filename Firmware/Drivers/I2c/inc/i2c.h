#ifndef I2C_HEADER
#define I2C_HEADER

#include <stdint.h>
#include <stdbool.h>
#include "i2c_reg.h"

#define I2C_CMD_WRITE_BIT 0U
#define I2C_CMD_READ_BIT  1U
#define I2C_CMD_MASK (0x01)

/* We can override this buffer with -D or in a future config file */
/* TODO : add support for a config file !*/
#ifndef I2C_MAX_BUFFER_SIZE
#define I2C_MAX_BUFFER_SIZE (30U)
#endif

#ifdef __cplusplus
extern "C"
{
#endif


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
    uint8_t slave_address_mask; /**< Address mask used to drive address recognition module of TWI hardware                            */
    bool general_call_enabled;  /**< Activate the response to general call (0x00) or not                                              */
    bool interrupt_enabled;     /**< Use the interrupt-based workflow or not (if not, i2c_process() will have to be called regularly) */
    i2c_handle_t handle;        /**< Handle which will be used to effectively interact with peripheral                                */
} i2c_config_t;

/**
 * @brief generic structure used to handle common I2C functions returned
*/
typedef enum
{
    I2C_ERROR_OK,                       /**< Operation is successful                                                  */
    I2C_ERROR_NULL_POINTER,             /**< One or more input pointer is set to NULL                                 */
    I2C_ERROR_NULL_HANDLE,              /**< Given handle is not initialised with real addresses                      */
    I2C_ERROR_DEVICE_NOT_FOUND,         /**< Given device id is out of range                                          */
    I2C_ERROR_INVALID_ADDRESS,          /**< Given address is out of conventional I2C addresses range                 */
    I2C_ERROR_WRONG_STATE,              /**< Targeted device is in a wrong internal state                             */
    I2C_ERROR_NOT_INITIALISED,          /**< Extension of the error above (device not initialised)                    */
    I2C_ERROR_MAX_RETRIES_HIT,          /**< Too much errors were encountered, maximum allowed retries count was hit  */
    I2C_ERROR_REQUEST_TOO_SHORT,        /**< The given request (i2c_read or i2c_write) is too short                   */
    I2C_ERROR_REQUEST_TOO_LONG,         /**< The given request (i2c_read or i2c_write) is too long for this driver    */
    I2C_ERROR_ALREADY_PROCESSING,       /**< Not really an error : indicates driver is busy and get_state() might be  */
                                          /* called to know which state the I2C driver is running on                  */
    I2C_ERROR_BUS_ERROR_HARDWARE,       /**< A bus error was encountered and I2C hardware recovered from it           */
    I2C_ERROR_SLAVE_HANDLERS_NOT_SET    /**< Internal slave handlers are not set (NULL), aborting execution           */
} i2c_error_t;

/**
 * @brief tells if an I2C read command succeeded or not. Basically, a call might fail if the given command is invalid
*/
typedef enum
{
    I2C_SLAVE_HANDLER_ERROR_OK,                     /**< Operation is successful                                            */
    I2C_SLAVE_HANDLER_ERROR_UNKNOWN_COMMAND,        /**< Command written into TWI data register cannot be resolved          */
    I2C_SLAVE_HANDLER_ERROR_BUFFER_NULLPTR,         /**< Given buffer is set to NULL (uninitialized)                        */
    I2C_SLAVE_HANDLER_ERROR_BUFFER_OVERFLOW_GUARD,  /**< Prevents to read or write past the length of slave virtual buffer  */
    I2C_SLAVE_HANDLER_ERROR_NO_HANDLER_SPECIFIED,   /**< Generic error thrown by i2c driver in case slave data handlers still
                                                         point to default ones                                              */
} i2c_slave_handler_error_t;

typedef enum
{
    I2C_STATE_DISABLED              = 0, /**< Configured, but peripheral disabled                               */
    I2C_STATE_NOT_INITIALISED,           /**< Not configured, need to call i2c_init first                       */
    I2C_STATE_READY,                     /**< Configured and enabled                                            */
    I2C_STATE_DEVICE_BUSY,               /**< General Busy flag stating that device is used (TWINT is set to 0) */

    /* I2C busy states */
    I2C_STATE_SLAVE_RECEIVING,           /**< Peripheral is currently receiving data as slave device  */
    I2C_STATE_SLAVE_TRANSMITTING,        /**< Peripheral is currently sending data as slave device    */
    I2C_STATE_MASTER_RECEIVING,          /**< Peripheral is currently receiving data as master device */
    I2C_STATE_MASTER_TRANSMITTING,       /**< Peripheral is currently sending data as master device   */

    /* I2C finished operations states */
    I2C_STATE_MASTER_TX_FINISHED,       /**< Just finished an i2c write transmission                    */
    I2C_STATE_MASTER_RX_FINISHED,       /**< Just finished an i2c read transmission type (write + read) */

    I2C_STATE_PERIPHERAL_ERROR          /**< Peripheral encountered errors and alerts application    */
} i2c_state_t;


/**
 * @brief structure to be initialised by the i2c command handler function
*/
typedef struct
{
    uint8_t* data;  /**< pointer to targeted buffer. NULL if command is invalid or after first initialisation */
    uint8_t length; /**< length of the selected buffer, to prevent writing/reading past the end of the buffer */
    bool locked;    /**< This boolean lock tells if this buffer is still in use or not                        */
} i2c_command_handling_buffers_t;

/**
 * @brief records the kind of request is ongoing in the internal state machine
*/
typedef enum
{
    I2C_REQUEST_WRITE,  /**< Write request ongoing                */
    I2C_REQUEST_READ,   /**< Read request ongoing                 */
    I2C_REQUEST_IDLE,   /**< No request ongoing, driver is idling */
} i2c_request_t;


/**
 * @brief function pointer which has to be defined by the application software
 * @details This function pointer will be used when the I2C device enters its slave mode and has been addressed
 * by a master on the bus.
 * Its role is to parse the incoming uint8_t byte (which is the value stored in TWI Data Register) and to initialise the
 * i2c_command_handling_buffers_t with pointers to internal data.
 * @param[in]   uint8_t -> byte                         : this is the data found in TWDR (to be parsed)
 * @param[out]  i2c_command_handling_buffers_t->buffer  : the buffer which needs initialisation.

 * Example given :
 *  Let's think about an I2C slave device which monitors temperature in a complex system and exposes its internal reading
 *  as such : 2 analog values (temperature), and 2 boolean values (2 thermostats for instance, which represents thermo-mechanical switches).
 *  Now, assume we expose the following data to be read from :
 *
 *      - An array to read the 2 analog sensors values  :       uint8_t temperatures[2]
 *                                                              bool temperatures_lock;

 *      - A register where the 2 thermostats are mapped :       uint8_t thermostat_switches (bit mapped)
 *                                                              bool thermostat_switches_lock;

 *  And we can also imagine that this thermal controlled device has thermal regulations capabilities (has a threshold above which cooling is activated)
 *  So that's one register to be written to as well :
 *      - Cooling threshold :                                   uint8_t threshold;
 *                                                              bool threshold_lock;
 *
 *  Now, we can imagine that this device will accept at least 3 commands, for a simple I2C interface :
 *      - THERMAL_MONITOR_TEMPERATURES  : when receiving this command, the device shall initialise the structure like so:
 *                                        *     buffer->data to point to temperatures array
 *                                        *     buffer->length = 2;
 *                                        *     buffer->lock = &temperatures_lock;
 *                                          -> This will be used by the I2C driver to signal the application software when
 *                                          I2C communication is finished and releases the control over the data

 *      - THERMAL_MONITOR_THERMOSTATS   : when receiving this command, the device shall initialise the structure like so:
 *                                        *     buffer->data = &thermostat_switches;
 *                                        *     buffer->length = 1;
 *                                        *     buffer->lock = &thermostat_switches_lock;

 *      - THERMAL_MONITOR_THRESHOLD     : when receiving this command, the device shall initialise the structure like so:
 *                                        *     buffer->data = &threshold;
 *                                        *     buffer->length = 1;
 *                                        *     buffer->lock = &threshold_lock;
*/
typedef i2c_slave_handler_error_t (*i2c_slave_data_handler_t)(uint8_t * /* current byte */, const i2c_request_t /* request */);

typedef i2c_slave_handler_error_t (*i2c_slave_transmission_over_callback_t)(void);

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
i2c_error_t i2c_set_handle(const uint8_t id, i2c_handle_t const * const handle);

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
 * @param[in]    id      : selected I2C driver instance to be configured
 * @param[out]   address : slave address of the selected device
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_NULL_POINTER       : Uninitialised pointer parameter
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_get_slave_address(const uint8_t id, uint8_t * const address);

/**
 * @brief writes to the slave address mask register
 * @param[in]   id              : selected I2C driver instance to be configured
 * @param[out]  address_mask    : slave address mask of the selected device
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_set_slave_address_mask(const uint8_t id, const uint8_t address_mask);

/**
 * @brief reads the address_mask register
 * @param[in]   id              : selected I2C driver instance to be configured
 * @param[out]  address_mask    : slave address mask of the selected device
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_NULL_POINTER       : Uninitialised pointer parameter
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_get_slave_address_mask(const uint8_t id, uint8_t * const address_mask);

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
i2c_error_t i2c_get_status_code(const uint8_t id, volatile uint8_t * const status_code);


/* #############################################################################################
   ######################################## General API ########################################
   ############################################################################################# */

/**
 * @brief powers up selected device (no need for it to be configured or not), but the handle has to be set for this to work
 * @param[in]   id      : selected I2C driver instance to be configured
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_enable(const uint8_t id);

/**
 * @brief switches off selected device (no need for it to be configured or not)
 * @param[in]   id      : selected I2C driver instance to be configured
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_disable(const uint8_t id);

/**
 * @brief registers the given function into selected I2C driver command handler used when receiving commands as a slave device
 * @see i2c_command_handler_t documentation for further details about this command handler
 * @param[in]   id                          : selected I2C driver instance to be configured
 * @param[in]   i2c_slave_command_handler   : command handler which will be used to handle incoming data when working as a slave device
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_POINTER       : Uninitialised pointer parameter
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_slave_set_data_handler(const uint8_t id, i2c_slave_data_handler_t command_handler);

/**
 * @brief registers the given function into selected I2C driver command handler used when receiving commands as a slave device
 * @see i2c_command_handler_t documentation for further details about this command handler
 * @param[in]   id                          : selected I2C driver instance to be configured
 * @param[in]   i2c_slave_command_handler   : command handler which will be used to handle incoming data when working as a slave device
 * @return i2c_error_t :
 *      I2C_ERROR_OK                 : Operation succeeded
 *      I2C_ERROR_NULL_POINTER       : Uninitialised pointer parameter
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_slave_set_transmission_over_callback(const uint8_t id, i2c_slave_transmission_over_callback_t callback);


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
 *      I2C_ERROR_NULL_HANDLE        : Uninitialised handle in config object
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
 *      I2C_ERROR_NOT_INITIALISED    : Device is not initialised, operation was aborted
 *      I2C_ERROR_DEVICE_NOT_FOUND   : Selected instance id does not exist in available instances
*/
i2c_error_t i2c_process(const uint8_t id);

/**
 * @brief writes data to I2C bus
 * @param[in]   id              : selected I2C driver instance to be configured
 * @param[in]   target_address  : targeted slave address on I2C bus
 * @param[in]   buffer          : pointer to a buffer of bytes holding the actual payload to be sent over I2C bus
 *                                Note that buffer shall also contain the slave command as the first byte of the buffer if required
 *                                e.g. buffer[0] = LCD_SCREEN_CMD_SET_CURSOR
 *                                     buffer[1] = data ..
 *                                      ...
 *                                     buffer[length - 1] = end of data
 * @param[in]   length          : total length of given buffer
 * @param[in]   retries         : number of available tries before giving up
 * @return i2c_error_t :
 *      I2C_ERROR_OK                  : Operation succeeded
 *      I2C_ERROR_NULL_POINTER        : Uninitialised pointer parameter
 *      I2C_ERROR_NULL_HANDLE         : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_INVALID_ADDRESS     : Targeted slave address is not I2C compatible on 7 bits address mode (>= 128)
 *      I2C_ERROR_NOT_INITIALISED     : Device is not initialised, operation was aborted
 *      I2C_ERROR_DEVICE_NOT_FOUND    : Selected instance id does not exist in available instances
 *      I2C_ERROR_REQUEST_TOO_SHORT   : Given request's length is too short (shall be >= 2)
 *      I2C_ERROR_ALREADY_PROCESSING  : Selected instance is already processing (either in master or slave mode). @see i2c_get_state()
*/
i2c_error_t i2c_write(const uint8_t id, const uint8_t target_address , uint8_t * const buffer, const uint8_t length, const uint8_t retries);

/**
 * @brief reads data to I2C bus
 * @param[in]   id              : selected I2C driver instance to be configured
 * @param[in]   target_address  : targeted slave address on I2C bus (address is not checked at runtime, must not bit greater than 127)
 * @param[out]  buffer          : pointer to a buffer of bytes holding the actual payload to be sent over I2C bus
 *                                Note that the first byte of buffer shall contain the slave command (mainly indicates where slave should
 *                                read data from ; so it is actually an i2c write action (single byte) followed by a subsequent read starting
 *                                from pointed slave register's address)
 * @param[in]   length          : total length of given buffer
 * @param[in]   retries         : number of available tries before giving up
 * @return i2c_error_t :
 *      I2C_ERROR_OK                  : Operation succeeded
 *      I2C_ERROR_NULL_POINTER        : Uninitialised pointer parameter
 *      I2C_ERROR_NULL_HANDLE         : Uninitialised handle in config object (could not access to device's registers)
 *      I2C_ERROR_NOT_INITIALISED     : Device is not initialised, operation was aborted
 *      I2C_ERROR_DEVICE_NOT_FOUND    : Selected instance id does not exist in available instances
 *      I2C_ERROR_REQUEST_TOO_SHORT   : Given request's length is too short (shall be >= 2)
 *      I2C_ERROR_ALREADY_PROCESSING  : Selected instance is already processing (either in master or slave mode). @see i2c_get_state()
*/
i2c_error_t i2c_read(const uint8_t id, const uint8_t target_address, uint8_t * const buffer, const uint8_t length, const uint8_t retries);

/**
 * @brief this function tells whether the master buffer passed in i2c_read and i2c_write is still used by the driver or not
 * In case it is still used, it means caller shall not modify the given buffer, otherwise it might compromise the whole
 * I2C driver current operation.
 * @return
 *      true    : buffer is currently used and is soft-locked by the driver. Do not modify it.
 *      false   : driver's operations completed and data is now ready to be used
*/
bool i2c_is_master_buffer_locked(const uint8_t id);

#ifdef __cplusplus
}
#endif


#endif /* I2C_HEADER */