/*

------------------
@<FreeMyCode>
FreeMyCode version : 1.0 RC alpha
    Author : bebenlebricolo
    License : 
        name : GPLv3
        url : https://www.gnu.org/licenses/quick-guide-gplv3.html
    Date : 12/02/2021
    Project : LabBenchPowerSupply
    Description : The Lab Bench Power Supply provides a simple design based around an Arduino Nano board to convert AC main voltage into
 smaller ones, ranging from 0V to 16V, with voltage and current regulations
<FreeMyCode>@
------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "i2c.h"
#include "config.h"

#include <stddef.h>
#include <string.h>

#include "i2c_private.h"

#define I2C_MAX_ADDRESS 127U

#ifndef I2C_DEVICES_COUNT
    #error "I2C_DEVICES_COUNT is not defined. Please add #define I2C_DEVICES_COUNT in config.h to use this timer"
#elif I2C_DEVICES_COUNT == 0
    #warning "I2C_DEVICES_COUNT is set to 0. If you don't project to use this timer, prefer to not compile this file instead of setting this define to 0"
#endif

#ifndef UNIT_TESTING
    #include <avr/interrupt.h>
#else
    #include "test_isr_stub.h"
    #include "memutils.h"
#endif

/* Minimum I2C request size is 2 to account for : 1 op code + 1 read/write data for configurable devices
   or 1 for "dumb" devices such as IO Expanders with no Op Code */
#define MINIMUM_REQUEST_SIZE (1U)

#ifndef I2C_IMPLEM_FULL_DRIVER
static i2c_error_t i2c_disabled_process(const uint8_t id)
{
    (void) id;
    return I2C_ERROR_IMPLEM_DISABLED;
}
#endif

typedef struct
{
    bool is_initialised;                        /**< Determines whether this instance has been initialised or not                                   */
    i2c_handle_t handle;                        /**< Stores a collection of pointers to the actual TWI peripheral registers                         */
    i2c_state_t state;                          /**< Internal state machine used to handle subsequent calls to the ISR or process routines          */
    i2c_request_t request_type;                 /**< Describes the type of request, either I2C_REQUEST_READ, I2C_REQUEST_WRITE or I2C_REQUEST_IDLE  */
    struct
    {
        i2c_slave_data_handler_t data_handler;      /**< Pointer to a dedicated handler when this device is addressed in slave mode (its role is to
                                                     validate the incoming request and if valid, to initialise the i2c_buffer field of
                                                     internal_configuration structure. It essentially maps the i2c_buffer's fields according to the
                                                     registers and command sets exposed by this device as an I2C command API                        */
        i2c_slave_transmission_over_callback_t callback;
    } slave;
} i2c_internal_config_t;
static volatile i2c_internal_config_t internal_configuration[I2C_DEVICES_COUNT] = {0};

typedef struct
{
    uint8_t target_address;                     /**< Contains target slave address                                      */
    uint8_t command;                            /**< Contains target address + read/write bit                           */
    uint8_t index;                              /**< Increment used in Rx/Tx mode to iterate though the internal buffer */
    uint8_t retries;                            /**< Stores the maximum available retries                               */
    i2c_command_handling_buffers_t i2c_buffer;  /**< I2C buffer used to handle in/out data coming from I2C bus          */
} i2c_master_buffer_t;
static i2c_master_buffer_t master_buffer[I2C_DEVICES_COUNT] = {0};

#if defined(I2C_IMPLEM_SLAVE_TX) || defined(I2C_IMPLEM_SLAVE_RX)
static uint8_t slave_received_bytes[I2C_DEVICES_COUNT] = {0};
#endif

static inline volatile uint8_t * get_current_master_buffer_byte(const uint8_t id)
{
    // Split variables to ease debugging
    uint8_t index = master_buffer[id].index;
    uint8_t * data = master_buffer[id].i2c_buffer.data;
    return &data[index];
}

static inline void clear_twint(const uint8_t id)
{
    #ifdef UNIT_TESTING
    *internal_configuration[id].handle._TWCR &= ~TWINT_MSK;
    #else
    *internal_configuration[id].handle._TWCR |= TWINT_MSK;
    #endif
}

static inline void reset_i2c_master_buffer(const uint8_t id)
{
    master_buffer[id].i2c_buffer.data = NULL;
    master_buffer[id].i2c_buffer.length = 0;
    master_buffer[id].i2c_buffer.locked = false;
}

/**
 * @brief this function replaces the traditional |= operator
 * @param[in] id    : driver id
 * @param[in] mask  : bitmask which will be added to TWCR with an equivalent |= operation
 * @example :
 *      add_to_TWCR(id, TWSTA_MSK) is equivalent to :
 *          *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~ TWINT_MSK) | TWSTA_MSK
 * @note this function is required because traditional |= operator will trigger unwanted operations such as :
 *      TWCR |= TWSTA_MSK
 * is equivalent to :
 *      TWCR = TWCR | TWSTA_MSK
 * but as TWINT bit is a "Clear on write" bit, if TWINT bit is set while TWCR |= <mask> is written
 * then TWINT (=1) will be written over itself, clearing the flag in the same time and restarting TWI hardware
 *
 * So proper micro handling is required and TWCR should always be written with TWCR = (~TWINT_MSK | <new value>);
*/
static inline void add_to_TWCR(const uint8_t id, uint8_t new_value)
{
    *internal_configuration[id].handle._TWCR = (~TWINT_MSK & new_value);
    //volatile uint8_t old_TWCR = *internal_configuration[id].handle._TWCR & ~TWINT_MSK;
    //*internal_configuration[id].handle._TWCR = old_TWCR | mask;
}


/**
 * @brief this function should be used whenever we want to set the TWCR register to a new value
 * @param[in] id        : driver id
 * @param[in] new_value : new value for TWCR
 * @example :
 * @note this function is required because TWCR register contains the TWINT flag which is of the Clear On Write type
 *  Hence, this kind of manipulations, despite being conceptually right clears the TWINT flag if it is already set within TWCR !
 *      TWCR |= TWSTA_MSK
 * is equivalent to :
 *      TWCR = TWCR | TWSTA_MSK
 * but as TWINT bit is a "Clear on write" bit, if TWINT bit is set while TWCR |= <mask> is written
 * then TWINT (=1) will be written over itself, clearing the flag in the same time and restarting TWI hardware
 *
 * So proper micro handling is required and TWCR should always be written with TWCR = (~TWINT_MSK | <new value>);
*/
static inline void set_TWCR_register(const uint8_t id, uint8_t new_value)
{
    *internal_configuration[id].handle._TWCR = (~TWINT_MSK & new_value);
}

static inline void remove_from_TWCR(const uint8_t id, uint8_t mask)
{
    volatile uint8_t old_TWCR = *internal_configuration[id].handle._TWCR & ~TWINT_MSK;
    *internal_configuration[id].handle._TWCR = old_TWCR & (~mask);
}

/**
 * @brief Copies the content of a static volatile handle a regular one
*/
static void i2c_handle_vcopy(i2c_handle_t * dest, volatile i2c_handle_t const * const src);
static void i2c_handle_copyv(volatile i2c_handle_t * dest, i2c_handle_t const * const src);


/* handlers used to process in/out data when TWI peripheral needs servicing */
static i2c_error_t i2c_master_tx_process(const uint8_t id);
static i2c_error_t i2c_master_rx_process(const uint8_t id);
static i2c_error_t i2c_slave_tx_process(const uint8_t id);
static i2c_error_t i2c_slave_rx_process(const uint8_t id);


/* In case no handler is given, simply discard incoming data */
static inline i2c_slave_handler_error_t default_command_handler(uint8_t * const data_byte, const i2c_request_t request)
{
    (void) data_byte;
    (void) request;
    return I2C_SLAVE_HANDLER_ERROR_NO_HANDLER_SPECIFIED;
}

static inline i2c_slave_handler_error_t default_transmission_over_callback(void)
{
    return I2C_SLAVE_HANDLER_ERROR_NO_HANDLER_SPECIFIED;
}

static inline bool is_handle_initialised(const uint8_t id)
{
    bool out = true;
    out &= (NULL != internal_configuration[id].handle._TWCR);
    out &= (NULL != internal_configuration[id].handle._TWAMR);
    out &= (NULL != internal_configuration[id].handle._TWBR);
    out &= (NULL != internal_configuration[id].handle._TWDR);
    out &= (NULL != internal_configuration[id].handle._TWSR);
    out &= (NULL != internal_configuration[id].handle._TWAR);
    return out;
}

static inline bool is_id_valid(const uint8_t id)
{
    return id < I2C_DEVICES_COUNT;
}

static inline bool is_twint_set(const uint8_t id)
{
    return (0 != ((*internal_configuration[id].handle._TWCR) & TWINT_MSK));
}

static inline void reset_handle(i2c_handle_t * const handle)
{
   (void)memset(handle, 0, sizeof(i2c_handle_t));
}

#ifdef UNIT_TESTING
void i2c_driver_reset_memory(void)
{
    for (uint8_t i = 0; i < I2C_DEVICES_COUNT ; i++)
    {
        volatile_memset(&internal_configuration[i], 0, sizeof(i2c_internal_config_t));
        volatile_memset(&master_buffer[i], 0, sizeof(i2c_master_buffer_t));
    }
}

uint8_t * i2c_get_master_data_buffer(const uint8_t id)
{
    return master_buffer[id].i2c_buffer.data;
}

#endif


i2c_error_t i2c_get_default_config(i2c_config_t * const config)
{
    if (NULL == config)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    reset_handle(&config->handle);
    config->baudrate = 0;
    config->general_call_enabled = false;
    config->interrupt_enabled = false;
    config->prescaler = 0;
    config->slave.enable = false;
    config->slave.address = 0;
    config->slave.address_mask = 0;

    return I2C_ERROR_OK;
}

i2c_error_t i2c_set_handle(const uint8_t id, i2c_handle_t const * const handle)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == handle)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    i2c_handle_copyv(&internal_configuration[id].handle, handle);

    return I2C_ERROR_OK;
}

i2c_error_t i2c_get_handle(const uint8_t id, i2c_handle_t * const handle)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == handle)
    {
        return I2C_ERROR_NULL_POINTER;
    }

    i2c_handle_vcopy(handle, &internal_configuration[id].handle);
    return I2C_ERROR_OK;
}

i2c_error_t i2c_set_slave_mode(const uint8_t id, const bool enabled)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    *(internal_configuration[id].handle._TWCR) |= enabled * TWEA_MSK;
    return I2C_ERROR_OK;
}


i2c_error_t i2c_set_slave_address(const uint8_t id, const uint8_t address)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    volatile i2c_handle_t * handle = &internal_configuration[id].handle;
    *(handle->_TWAR) = (*(handle->_TWAR) & TWA_MSK) | (address << 1U);

    return I2C_ERROR_OK;
}

i2c_error_t i2c_get_slave_address(const uint8_t id, uint8_t * const address)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == address)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    *address = (*(internal_configuration[id].handle._TWAR) & TWA_MSK) >> 1U;
    return I2C_ERROR_OK;
}

i2c_error_t i2c_set_slave_address_mask(const uint8_t id, const uint8_t address_mask)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    volatile i2c_handle_t * handle = &internal_configuration[id].handle;
    *(handle->_TWAMR) = (*(handle->_TWAMR) & TWAMR_MSK) | (address_mask << 1U);

    return I2C_ERROR_OK;
}

i2c_error_t i2c_get_slave_address_mask(const uint8_t id, uint8_t * const address_mask)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == address_mask)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    *address_mask = (*(internal_configuration[id].handle._TWAMR) & TWAMR_MSK) >> 1U;
    return I2C_ERROR_OK;
}

i2c_error_t i2c_set_prescaler(const uint8_t id, const i2c_prescaler_t prescaler)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    *(internal_configuration[id].handle._TWSR) = ((*(internal_configuration[id].handle._TWSR) & TWPS_MSK) | prescaler);
    return I2C_ERROR_OK;
}

i2c_error_t i2c_get_prescaler(const uint8_t id, i2c_prescaler_t * const prescaler)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == prescaler)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    *prescaler = *(internal_configuration[id].handle._TWSR) & TWPS_MSK;
    return I2C_ERROR_OK;
}

i2c_error_t i2c_set_baudrate(const uint8_t id, const uint8_t baudrate)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    *(internal_configuration[id].handle._TWBR) = baudrate;
    return I2C_ERROR_OK;
}


i2c_error_t i2c_get_baudrate(const uint8_t id, uint8_t * const baudrate)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == baudrate)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    *baudrate = *(internal_configuration[id].handle._TWBR);
    return I2C_ERROR_OK;
}


i2c_error_t i2c_set_general_call_enabled(const uint8_t id, const bool general_call_enabled)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    if (general_call_enabled)
    {
        *(internal_configuration[id].handle._TWAR) |= TWGCE_MSK;
    }
    else
    {
        *(internal_configuration[id].handle._TWAR) &= ~TWGCE_MSK;
    }
    return I2C_ERROR_OK;
}


i2c_error_t i2c_get_general_call_enabled(const uint8_t id, bool * const general_call_enabled)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == general_call_enabled)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    *general_call_enabled = (bool) (*(internal_configuration[id].handle._TWAR) & TWGCE_MSK);
    return I2C_ERROR_OK;
}

i2c_error_t i2c_set_interrupt_mode(const uint8_t id, const bool use_interrupts)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    if (use_interrupts)
    {
        *(internal_configuration[id].handle._TWCR) |= TWIE_MSK;
    }
    else
    {
        *(internal_configuration[id].handle._TWCR) &= ~TWIE_MSK;
    }
    return I2C_ERROR_OK;
}

i2c_error_t i2c_get_interrupt_mode(const uint8_t id, bool * const use_interrupts)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == use_interrupts)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    *use_interrupts = (bool) (*(internal_configuration[id].handle._TWCR) & TWIE_MSK);
    return I2C_ERROR_OK;
}

i2c_error_t i2c_get_status_code(const uint8_t id, volatile uint8_t * const status)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == status)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    *status = (*internal_configuration[id].handle._TWSR & TWS_MSK);
    return I2C_ERROR_OK;
}


i2c_error_t i2c_enable(const uint8_t id)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    *(internal_configuration[id].handle._TWCR) |= TWEN_MSK;
    if (internal_configuration[id].is_initialised)
    {
        internal_configuration[id].state = I2C_STATE_READY;
    }
    else
    {
        internal_configuration[id].state = I2C_STATE_NOT_INITIALISED;
    }
    return I2C_ERROR_OK;
}

i2c_error_t i2c_disable(const uint8_t id)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    *(internal_configuration[id].handle._TWCR) &= ~TWEN_MSK;
    internal_configuration[id].state = I2C_STATE_DISABLED;
    return I2C_ERROR_OK;
}

i2c_error_t i2c_slave_set_data_handler(const uint8_t id, i2c_slave_data_handler_t data_handler)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == data_handler)
    {
        return I2C_ERROR_NULL_POINTER;
    }

    internal_configuration[id].slave.data_handler = data_handler;
    return I2C_ERROR_OK;
}

i2c_error_t i2c_slave_set_transmission_over_callback(const uint8_t id, i2c_slave_transmission_over_callback_t callback)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == callback)
    {
        return I2C_ERROR_NULL_POINTER;
    }

    internal_configuration[id].slave.callback = callback;
    return I2C_ERROR_OK;
}


#ifdef UNIT_TESTING
i2c_slave_data_handler_t i2c_slave_get_command_handler(const uint8_t id)
{
    return internal_configuration[id].slave.data_handler;
}
#endif

static i2c_error_t write_config(const uint8_t id, const i2c_config_t * const config)
{
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    /* Baudrate */
    *(internal_configuration[id].handle._TWBR) = config->baudrate;

    /* Prescaler */
    *(internal_configuration[id].handle._TWSR) &= ~TWPS_MSK;
    *(internal_configuration[id].handle._TWSR) |= config->prescaler;

    /* Slave address */
    *(internal_configuration[id].handle._TWAR) &= ~TWA_MSK;
    *(internal_configuration[id].handle._TWAR) |= (config->slave.address << 1U);

    /* Slave address mask */
    *(internal_configuration[id].handle._TWAMR) &= ~TWAMR_MSK;
    *(internal_configuration[id].handle._TWAMR) |= (config->slave.address_mask << 1U);

    /* General call enabled flag */
    *(internal_configuration[id].handle._TWAR) &= ~TWGCE_MSK;
    *(internal_configuration[id].handle._TWAR) |= (uint8_t) config->general_call_enabled;

    /* Interrupt enabled flag */
    *(internal_configuration[id].handle._TWCR) &= ~TWIE_MSK;
    *(internal_configuration[id].handle._TWCR) |= (uint8_t) config->interrupt_enabled;

    // Enables this device as a slave, or not depending on the enable bit config
    *(internal_configuration[id].handle._TWCR) |= config->slave.enable * TWEA_MSK;

    return I2C_ERROR_OK;
}

i2c_error_t i2c_init(const uint8_t id, const i2c_config_t * const config)
{
    // Initialises handlers to default implementations first (otherwise this might crash somehow)
    internal_configuration[id].slave.callback = default_transmission_over_callback;
    internal_configuration[id].slave.data_handler = default_command_handler;

    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == config)
    {
        return I2C_ERROR_NULL_POINTER;
    }

    i2c_error_t local_error = i2c_set_handle(id, &config->handle);
    if (I2C_ERROR_OK != local_error)
    {
        return local_error;
    }

    local_error = write_config(id, config);
    if (I2C_ERROR_OK != local_error)
    {
        return local_error;
    }

    local_error = i2c_enable(id);
    if (I2C_ERROR_OK != local_error)
    {
        return local_error;
    }

    internal_configuration[id].state = I2C_STATE_READY;
    internal_configuration[id].is_initialised = true;

    return local_error;
}


i2c_error_t i2c_deinit(const uint8_t id)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    i2c_error_t local_error = i2c_disable(id);
    if (I2C_ERROR_OK != local_error)
    {
        return local_error;
    }
    i2c_config_t config;
    (void) i2c_get_default_config(&config);
    local_error = write_config(id, &config);
    if (I2C_ERROR_OK != local_error)
    {
        return local_error;
    }
    /* Restores the handle back to its default state : filled with NULL */
    local_error = i2c_set_handle(id, &(config.handle));
    internal_configuration[id].is_initialised = false;
    internal_configuration[id].state = I2C_STATE_DISABLED;;
    return local_error;
}

i2c_error_t i2c_get_state(const uint8_t id, i2c_state_t * const state)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == state)
    {
        return I2C_ERROR_NULL_POINTER;
    }

    *state = internal_configuration[id].state;
    return I2C_ERROR_OK;
}

#ifdef UNIT_TESTING
void i2c_set_state(const uint8_t id, const i2c_state_t state)
{
    internal_configuration[id].state = state;
}
#endif

static i2c_error_t i2c_master_tx_process(const uint8_t id)
#ifndef I2C_IMPLEM_MASTER_TX
{
   return i2c_disabled_process(id);
}
#else
{
    static uint8_t retries = 0;
    i2c_error_t ret = I2C_ERROR_OK;

    const volatile uint8_t status = *internal_configuration[id].handle._TWSR & TWS_MSK;
    // Interprete status code:
    switch (status)
    {
        case MAS_TX_START_TRANSMITTED:
        case MAS_TX_REPEATED_START:
            // Transmition was successfully started
            // Send slave address + write flag to start writing
            *internal_configuration[id].handle._TWDR = master_buffer[id].command;
            break;

        case MAS_TX_SLAVE_WRITE_ACK:
            // Slave replied ACK to its address in write mode, proceed further and send next byte
            *internal_configuration[id].handle._TWDR = *get_current_master_buffer_byte(id);

            // Clear TWSTA to prevent repeated start action
            set_TWCR_register(id, *internal_configuration[id].handle._TWCR & ~TWSTA_MSK);
            //*internal_configuration[id].handle._TWCR &= ~TWSTA_MSK;

            retries = 0;
            break;

        case MAS_TX_SLAVE_WRITE_NACK:
            // Slave did not reply correctly to its address, or some issue were encountered on I2C line
            // Resend Slave + write command
            *internal_configuration[id].handle._TWDR = master_buffer[id].command;
            retries++;
            break;

        case MAS_TX_DATA_TRANSMITTED_ACK:
            // If we are facing an i2c read operation, only first byte has to be sent (contains the target's operation code to locate the right register)
            // Note: we need to increment index for all control flows because the first byte has been consumed and shall not be used for the i2c read operation
            master_buffer[id].index++;
            if (I2C_REQUEST_READ == internal_configuration[id].request_type)
            {
                /* Overwrite the command buffer with (Slave address + i2c read bit)
                   and send a repeated start condition to indicate a multi-mode I2C read.
                   Basically, this skips the Stop + Start conditions we would have to do if only
                   using unitary I2C commands such as Write (Start, payload, Stop) + Read (Start, payload, Stop)
                   and keeps the line busy until read operation completes, or fails.
                   Hence, the result is a shorter command [Start, payload, Start, payload, Stop] with the guarantee that the master
                   keeps the I2C bus priority for the whole communication duration */
                master_buffer[id].command = (master_buffer[id].target_address << 1U) | I2C_CMD_READ_BIT;
                set_TWCR_register(id, *internal_configuration[id].handle._TWCR | TWSTA_MSK);

                //*internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWSTA_MSK;
                /* Switch to TX finished state at next process loop to indicate we are ready to perform the 'real' read operation */
                internal_configuration[id].state = I2C_STATE_MASTER_TX_FINISHED;
            }
            else
            {
                if (master_buffer[id].i2c_buffer.length != master_buffer[id].index)
                {
                    // Send next byte of data
                    *internal_configuration[id].handle._TWDR = *get_current_master_buffer_byte(id) ;
                }
                else
                {
                    // If we hit the end of the buffer, stop the transmission
                    //set_TWCR_register(id, *internal_configuration[id].handle._TWCR | TWSTO_MSK);
                    *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWSTO_MSK;
                    //*internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWSTO_MSK;
                    internal_configuration[id].state = I2C_STATE_MASTER_TX_FINISHED;
                }
            }
            retries = 0;
            break;

        case MAS_TX_DATA_TRANSMITTED_NACK:
            // Resend last byte of data
            *internal_configuration[id].handle._TWDR = *get_current_master_buffer_byte(id) ;
            retries++;
            break;

        case MAS_TX_ARBITRATION_LOST:
        default:
            // Restore peripheral to its original state
            reset_i2c_master_buffer(id);
            internal_configuration[id].state = I2C_STATE_READY;
            retries = 0;
            break;
    }

    /* If maximum retries count was hit, reset peripheral back to its default state and
       end the transmission by writing a Stop condition
       Note : if maximum available retries is set to 0, it means we don't want to use the retry feature at all */
    if ((0 != master_buffer[id].retries)
        && (master_buffer[id].retries < retries))
    {
        reset_i2c_master_buffer(id);
        internal_configuration[id].state = I2C_STATE_READY;
        internal_configuration[id].request_type = I2C_REQUEST_IDLE;
        set_TWCR_register(id, (*internal_configuration[id].handle._TWCR | TWSTO_MSK) & ~(TWSTA_MSK | TWEA_MSK));

        //*internal_configuration[id].handle._TWCR ((= (*internal_configuration[id].handle._TWCR& )~TWINT_MSK) | TWSTO_MSK;
        //*internal_configuration[id].handle._TWCR &= ~(TWSTA_MSK | TWEA_MSK);
        retries = 0;
        ret = I2C_ERROR_MAX_RETRIES_HIT;
    }

    clear_twint(id);
    return ret;
}
#endif


static i2c_error_t i2c_master_rx_process(const uint8_t id)
#ifndef I2C_IMPLEM_MASTER_RX
{
   return i2c_disabled_process(id);
}
#else
{
    static uint8_t retries = 0;
    i2c_error_t ret = I2C_ERROR_OK;


    uint8_t status;
    ret = i2c_get_status_code(id, &status);
    if (I2C_ERROR_OK != ret)
    {
        // return here because if we can't read device's registers, we should stop any execution
        return ret;
    }

    // Escape early, nothing is there yet for the driver to handle
    if (I2C_MISC_NO_RELEVANT_STATE == status )
    {
        return ret;
    }

    // Interprete status code:
    switch (status)
    {
        case MAS_RX_START_TRANSMITTED:
        case MAS_RX_REPEATED_START:
            // Transmission was successfully started
            // Send slave address + read flag to start writing
            *internal_configuration[id].handle._TWDR = master_buffer[id].command;
            break;

        case MAS_RX_SLAVE_READ_ACK:
            /* The slave being read from recognized its address and is ready to transmit data over the bus
             * Send an acknowledge bit 'ACK' to tell that we are ready to accept data                       */
            *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~(TWINT_MSK | TWSTA_MSK)) | TWEA_MSK;
            retries = 0;
            break;

        case MAS_RX_SLAVE_READ_NACK:
            /* Slave did not reply correctly to its address, or some issue were encountered on I2C line
               TWI hardware will resend a Start condition automatically, which will switch back to the MAS_RX_REPEATED_START case above */
            retries++;
            break;

        case MAS_RX_DATA_RECEIVED_ACK:
            /* Read received byte and store it within the buffer */
            *get_current_master_buffer_byte(id) = *internal_configuration[id].handle._TWDR;
            master_buffer[id].index++;

            if (master_buffer[id].i2c_buffer.length == master_buffer[id].index)
            {
                /* We finished to read data from I2C bus, exiting gracefully
                   Will proceed with next byte and return a NACK before switching to MAS_RX_DATA_RECEIVED_NACK case
                  (used this time to indicate the end of communication) */

                 // Send a NACK to indicate the end of communication to the Slave TX
                *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~(TWINT_MSK | TWEA_MSK));
                clear_twint(id);

                // Then send a stop condition straight away to deassert the line.
                // Otherwise it'll take another byte exchange for the system to understand the communication is over.
                // Doing the 2 requests consecutively ensures that we are jumping from the MAS_RX_DATA_RECEIVED_ACK directly to MAS_RX_DATA_RECEIVED_NACK
                // of TWI hardware which then allows us to break the connection.
                *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWSTO_MSK;
                internal_configuration[id].state = I2C_STATE_MASTER_RX_FINISHED;

            }
            else
            {
                /* Send an acknowledge bit 'ACK' to indicate we are ready to receive next byte */
                *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~(TWINT_MSK)) | (TWEA_MSK);
            }

            retries = 0;
            break;

        case MAS_RX_DATA_RECEIVED_NACK:
            if (master_buffer[id].i2c_buffer.length == master_buffer[id].index)
            {
                *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWSTO_MSK;
                internal_configuration[id].state = I2C_STATE_MASTER_RX_FINISHED;
            }
            else
            {
                /* Reset the internal_buffer index to its starting location
                   but bear in mind that at the starting point, opcode has already been sent. That is why we need
                   to reset the internal index to 1 instead of 0 */
                master_buffer[id].index = 1U;

                *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWEA_MSK;
                retries++;
            }
            break;

        case MAS_RX_ARBITRATION_LOST_NACK:
        default:
            // Restore peripheral to its original state
            reset_i2c_master_buffer(id);
            internal_configuration[id].state = I2C_STATE_READY;
            retries = 0;
            break;
    }

    // If maximum retries count was hit, reset peripheral back to its default state and
    // end the transmission by writing a Stop condition
    if ((0 != master_buffer[id].retries)
        && (master_buffer[id].retries < retries))
    {
        reset_i2c_master_buffer(id);
        internal_configuration[id].state = I2C_STATE_READY;
        internal_configuration[id].request_type = I2C_REQUEST_IDLE;
        *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWSTO_MSK;
        *internal_configuration[id].handle._TWCR &= ~(TWSTA_MSK | TWEA_MSK);
        retries = 0;
        ret = I2C_ERROR_MAX_RETRIES_HIT;
    }

    clear_twint(id);
    return ret;
}
#endif

static i2c_error_t i2c_slave_tx_process(const uint8_t id)
#ifndef I2C_IMPLEM_SLAVE_TX
{
   return i2c_disabled_process(id);
}
#else
{
    static uint8_t retries = 0;
    i2c_error_t ret = I2C_ERROR_OK;
    i2c_slave_handler_error_t sla_err = I2C_SLAVE_HANDLER_ERROR_OK;

    uint8_t status;
    ret = i2c_get_status_code(id, &status);
    if (I2C_ERROR_OK != ret)
    {
        // return here because if we can't read device's registers, we should stop any execution
        return ret;
    }

    // Check if handlers are set, otherwise abort execution
    if ((NULL == internal_configuration[id].slave.callback)
    ||  (NULL == internal_configuration[id].slave.data_handler))
    {
        return I2C_ERROR_SLAVE_HANDLERS_NOT_SET;
    }

    // Interprete status code:
    switch (status)
    {
        case SLA_TX_OWN_ADDR_SLAVE_READ_ACK:
        case SLA_TX_ARBITRATION_LOST_OWN_ADDR_RECEIVED_ACK:
            // Copy data to buffer, now this is a read!

            // A previous write command was issued and i2c handling buffers shall have been initialised
            sla_err = internal_configuration->slave.data_handler(&slave_received_bytes[id], I2C_REQUEST_WRITE);
            if (I2C_SLAVE_HANDLER_ERROR_OK == sla_err)
            {
                *internal_configuration[id].handle._TWDR = slave_received_bytes[id];
                *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWEA_MSK;
            }
            else if (I2C_SLAVE_HANDLER_ERROR_OK_LAST_BYTE  == sla_err)
            {
                *internal_configuration[id].handle._TWDR = slave_received_bytes[id];
                *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~(TWINT_MSK | TWEA_MSK));
            }
            else
            {
                // Send back an "error" code in the form of a NACK
                *internal_configuration[id].handle._TWCR &= ~TWEA_MSK;
                retries++;
            }
            break;

        case SLA_TX_DATA_TRANSMITTED_ACK:
            // Previous data read from master was successful (received 'ACK'), now we assume that
            // our buffer has been correctly initialised by i2c read command handler and we can proceed
            // Note : if i2c command handler sets the buffer to point to a wrong place, this will either : 1) crash or 2) read from unwanted memory locations
            sla_err = internal_configuration->slave.data_handler(&slave_received_bytes[id], I2C_REQUEST_WRITE);
            if (I2C_SLAVE_HANDLER_ERROR_OK == sla_err)
            {
                *internal_configuration[id].handle._TWDR = slave_received_bytes[id];
                *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWEA_MSK;
            }
            else if (I2C_SLAVE_HANDLER_ERROR_OK_LAST_BYTE  == sla_err)
            {
                *internal_configuration[id].handle._TWDR = slave_received_bytes[id];
                *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~(TWINT_MSK | TWEA_MSK));
            }
            else
            {
                // Forbidden access : master tries to read past the end of the allocated buffer
                // Slave shall send back a NACK command and switch to the next statement (DATA transmitted and NACK received)
                *internal_configuration[id].handle._TWDR = 0;
                *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~(TWINT_MSK | TWEA_MSK));
                retries++;
            }
            break;

        // TWI hardware does not let us resend last byte, so break transmission
        case SLA_TX_DATA_TRANSMITTED_NACK:
        case SLA_TX_LAST_DATA_TRANSMITTED_ACK:
        default:
            slave_received_bytes[id] = 0;

            // Signal to the application transmission is over
            sla_err = internal_configuration[id].slave.callback();
            if (I2C_SLAVE_HANDLER_ERROR_OK != sla_err)
            {
                // Do something to handle slave handler error
            }

            /* Re-enable this device to be addressed as a Slave, but switch to the non-addressed mode (a subsequent Start + Slave address will
               be needed to reinitiate a transmission) */
            *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWEA_MSK ;
            internal_configuration[id].state = I2C_STATE_READY;
            retries = 0;
            break;
    }

#if 0
    // If maximum retries count was hit, reset peripheral back to its default state and
    // end the transmission by writing a Stop condition
    if ((0 != slave_buffer[id].retries)
        && (slave_buffer[id].retries < retries))
    {
        internal_configuration[id].state = I2C_STATE_READY;
        reset_i2c_slave_buffer(id);

        // Force TWI to stop any transmission (hard reset by toggling it off and back on)
        *internal_configuration[id].handle._TWCR &= ~TWIE_MSK;

        *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWIE_MSK;
        retries = 0;
        ret = I2C_ERROR_MAX_RETRIES_HIT;
    }
#endif

    clear_twint(id);
    return ret;
}
#endif

static i2c_error_t i2c_slave_rx_process(const uint8_t id)
#ifndef I2C_IMPLEM_SLAVE_RX
{
   return i2c_disabled_process(id);
}
#else
{
    static uint8_t retries = 0;
    static uint8_t processed_bytes = 0;
    i2c_error_t ret = I2C_ERROR_OK;
    i2c_slave_handler_error_t sla_err = I2C_SLAVE_HANDLER_ERROR_OK;

    uint8_t status;
    ret = i2c_get_status_code(id, &status);
    if (I2C_ERROR_OK != ret)
    {
        // return here because if we can't read device's registers, we should stop any execution
        return ret;
    }

    // Check if handlers are set, otherwise abort execution
    if ((NULL == internal_configuration[id].slave.callback)
    ||  (NULL == internal_configuration[id].slave.data_handler))
    {
        return I2C_ERROR_SLAVE_HANDLERS_NOT_SET;
    }

    // Interprete status code:
    switch (status)
    {
        case SLA_RX_SLAVE_WRITE_ACK:
        case SLA_RX_ARBITRATION_LOST_OWN_ADDR_RECEIVED_ACK :
        case SLA_RX_GENERAL_CALL_RECEIVED_ACK :
        case SLA_RX_ARBITRATION_LOST_GENERAL_CALL_RECEIVED_ACK :
            if (processed_bytes == 0)
            {
                // Drop this byte as it only contains Slave device's address in TWDR
                processed_bytes++;
                break;
            }
            // Lock buffer in order to tell to the caller data is being used
            /* Receiving I2C data, consumes it (application will know how to process it) */
            slave_received_bytes[id] = *internal_configuration[id].handle._TWDR;
            sla_err = internal_configuration[id].slave.data_handler(&slave_received_bytes[id], I2C_REQUEST_READ);
            if (I2C_SLAVE_HANDLER_ERROR_OK == sla_err)
            {
                /* Command handler has initialised data structures and buffer pointers */
                *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWEA_MSK;
                ++processed_bytes;
            }
            else
            {
                // An error was encountered inside command handler
                *internal_configuration[id].handle._TWCR &= ~TWEA_MSK;
                retries++;
            }
            break;

        case SLA_RX_PREV_ADDRESSED_DATA_RECEIVED_ACK:
        case SLA_RX_GENERAL_CALL_ADDRESSED_DATA_RECEIVED_ACK :
            // First byte of data contains the command code
            /* Write is accepted because we are still well inside the targeted buffer boundaries */
            slave_received_bytes[id] = *internal_configuration[id].handle._TWDR;
            sla_err = internal_configuration[id].slave.data_handler(&slave_received_bytes[id], I2C_REQUEST_READ);
            if (I2C_SLAVE_HANDLER_ERROR_OK == sla_err)
            {
                *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWEA_MSK;
                ++processed_bytes;
            }
            else
            {
                // Forbidden access : master tries to write past the end of the allocated buffer
                // Slave shall send back a NACK command and switch to the next statement (DATA received and NACK sent)
                *internal_configuration[id].handle._TWCR &= ~TWEA_MSK;
                retries++;
            }
            break;

        /* When written in multi-mode I2C style, we may get a repeated start in the middle of a transmission */
        case SLA_RX_START_STOP_COND_RECEIVED_WHILE_OPERATING:
            // Unlock buffer because we can either be at the start of a new request from master (start again) or a real stop
            // condition. So unlock the buffer now and eventually relock it afterwards.

            // Signal to the application transmission is over
            sla_err = internal_configuration[id].slave.callback();
            if (I2C_SLAVE_HANDLER_ERROR_OK != sla_err)
            {
                // Do something to handle slave handler error
            }

            /* Only one byte processed : we have just consumed the opcode, ready to switch to the not addressed Slave mode
               And allow this slave to recognize its own address (or general call, if enabled) */
            *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWEA_MSK;
            processed_bytes = 0;
            internal_configuration[id].state = I2C_STATE_READY;
            break;


        /* If NACK was returned by this device while writing to it (the 2 above cases), this means the slave
           decides to break the data exchange. TWEA bit is enabled to tell the TWI hardware to
           still respond to its own address and General call if enabled */
        case SLA_RX_PREV_ADDRESSED_DATA_LOST_NACK:
        case SLA_RX_GENERAL_CALL_ADDRESSED_DATA_LOST_NACK :
        default:
            *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWEA_MSK;
            internal_configuration[id].state = I2C_STATE_READY;
            processed_bytes = 0;
            retries = 0;
            break;
    }

#if 0
    // If maximum retries count was hit, reset peripheral back to its default state and
    // end the transmission by writing a Stop condition
    if ((0 != slave_buffer[id].retries)
        && (slave_buffer[id].retries < retries))
    {
        internal_configuration[id].state = I2C_STATE_READY;
        reset_i2c_slave_buffer(id);
        // Force TWI to stop any transmission (hard reset by toggling it off and back on)
        *internal_configuration[id].handle._TWCR &= ~TWIE_MSK;
        *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWIE_MSK;
        retries = 0;
        ret = I2C_ERROR_MAX_RETRIES_HIT;
    }
#endif

    clear_twint(id);
    return ret;
}
#endif

static i2c_error_t process_helper_single(const uint8_t id)
{
    i2c_error_t ret = I2C_ERROR_OK;
    volatile uint8_t status = 0;
    ret = i2c_get_status_code(id, &status);
    if (I2C_ERROR_OK != ret)
    {
        // TODO : check if this code is about an error status (misc statuses)
    }

    // Forces I2C driver to restart the last communication
    if (status == (uint8_t) I2C_MISC_BUS_ERROR_ILLEGAL_START_STOP)
    {
        internal_configuration[id].state = I2C_STATE_READY;
        reset_i2c_master_buffer(id);

        // Release from BUS error state
        *internal_configuration[id].handle._TWCR |= TWSTO_MSK | TWINT_MSK;

        // Tell to the upper layer a bus error occurred and some fixes need to be made
        // For instance, rebooting the screen controller is not a bad idea in such cases !
        return I2C_ERROR_BUS_ERROR_HARDWARE;
    }

    switch(internal_configuration[id].state)
    {
        /* TWINT is raised while no operation asked : this is the slave mode being activated by I2C bus */
        case I2C_STATE_READY:

            /* Wait and proceed current transfer, TWINT is low (=0) */
            if (status == (uint8_t) I2C_MISC_NO_RELEVANT_STATE)
            {
                break;
            }

            // Check if status code is somewhere inside SLAVE RX/TX status codes ranges
            if((status >= (uint8_t) SLA_RX_SLAVE_WRITE_ACK)
            && (status <= (uint8_t) SLA_RX_START_STOP_COND_RECEIVED_WHILE_OPERATING))
            {
                internal_configuration[id].state = I2C_STATE_SLAVE_RECEIVING;
                ret = i2c_slave_rx_process(id);
            }
            else if((status >= (uint8_t) SLA_TX_OWN_ADDR_SLAVE_READ_ACK)
                 && (status <= (uint8_t) SLA_TX_LAST_DATA_TRANSMITTED_ACK))
            {
                internal_configuration[id].state = I2C_STATE_SLAVE_TRANSMITTING;
                ret = i2c_slave_tx_process(id);
            }
            break;

        // handles Multi-mode read operation (write + repeated start + read + stop)
        case I2C_STATE_MASTER_TX_FINISHED:
            if (I2C_REQUEST_READ == internal_configuration[id].request_type)
            {
                internal_configuration[id].state = I2C_STATE_MASTER_RECEIVING;
                ret = i2c_master_rx_process(id);
            }
            else
            {
                internal_configuration[id].request_type = I2C_REQUEST_IDLE;
                internal_configuration[id].state = I2C_STATE_READY;

                // Soft-unlock data buffer
                master_buffer[id].i2c_buffer.locked = false;
            }
            break;

        // We go there whenever read command completes and has sent a Stop condition over the line
        case I2C_STATE_MASTER_RX_FINISHED:
            internal_configuration[id].request_type = I2C_REQUEST_IDLE;
            internal_configuration[id].state = I2C_STATE_READY;

            // Soft-unlock data buffer
            master_buffer[id].i2c_buffer.locked = false;
            break;

        /* Either a Start condition written from i2c_write was sent or a Tx operation is already ongoing */
        case I2C_STATE_MASTER_TRANSMITTING:
            if (is_twint_set(id))
            {
                ret = i2c_master_tx_process(id);
            }
            break;

        /* Either a Start condition was written from i2c_read or a Rx operation is already ongoing */
        case I2C_STATE_MASTER_RECEIVING:
            if (is_twint_set(id))
            {
                ret = i2c_master_rx_process(id);
            }
            break;

        /* A Slave receive operation is ongoing */
        case I2C_STATE_SLAVE_TRANSMITTING:
            if (is_twint_set(id))
            {
                ret = i2c_slave_tx_process(id);
            }
            break;

        /* A Slave transmit operation is ongoing */
        case I2C_STATE_SLAVE_RECEIVING:
            ret = i2c_slave_rx_process(id);
            break;

        /* Normal behavior of I2C device when waiting for a transaction to finish */
        case I2C_STATE_DEVICE_BUSY:
            if (is_twint_set(id))
            {
                internal_configuration[id].state = I2C_STATE_READY;
            }
            ret = I2C_ERROR_OK;
            break;

        default:
            /* Do nothing otherwise */
            ret = I2C_ERROR_WRONG_STATE;

            // Soft-unlock data buffer
            master_buffer[id].i2c_buffer.locked = false;
            break;
    }
    return ret;
}

/* Iterates over available i2c devices to find which one needs servicing */
#pragma GCC push_options
#pragma GCC optimize ("unroll-loops")
static void process_helper(void)
{
    for(uint8_t i = 0 ; i < I2C_DEVICES_COUNT ; i++)
    {
        if (is_twint_set(i))
        {
            (void) process_helper_single(i);
        }
    }
}
#pragma GCC pop_options

i2c_error_t i2c_process(const uint8_t id)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (!internal_configuration[id].is_initialised)
    {
        return I2C_ERROR_NOT_INITIALISED;
    }
    return process_helper_single(id);
}

i2c_error_t i2c_write(const uint8_t id, const uint8_t target_address , uint8_t * const buffer, const uint8_t length, const uint8_t retries)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == buffer)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    if (false == internal_configuration[id].is_initialised)
    {
        return I2C_ERROR_NOT_INITIALISED;
    }
    if (I2C_MAX_ADDRESS < target_address)
    {
        return I2C_ERROR_INVALID_ADDRESS;
    }

    uint8_t status_code = 0U;
    i2c_error_t err = i2c_get_status_code(id, &status_code);

    if (I2C_ERROR_OK != err)
    {
        return err;
    }

    // Check if device is already processing
    // Or I2C is not in its idling state (248) and twint is not set
    if (I2C_STATE_READY != internal_configuration[id].state
    || ((status_code != (uint8_t)I2C_MISC_NO_RELEVANT_STATE)
        && !is_twint_set(id)
        && status_code != (uint8_t) I2C_MISC_BUS_ERROR_ILLEGAL_START_STOP))
    {
        return I2C_ERROR_ALREADY_PROCESSING;
    }

    // Devices with OP Code needs 2 bytes whereas other simple devices such as IO expander sometimes does not implement OP Codes
    // and directly expose functionality using their input register only
    if (length < MINIMUM_REQUEST_SIZE)
    {
        return I2C_ERROR_REQUEST_TOO_SHORT;
    }

    // Reject requests that are bigger than available buffer
    if (length > I2C_MAX_BUFFER_SIZE)
    {
        return I2C_ERROR_REQUEST_TOO_LONG;
    }

    // Don't forget to lock the buffer to inform the end user it is still being used and shall not be modified
    master_buffer[id].i2c_buffer.data = buffer;
    master_buffer[id].i2c_buffer.locked = true;
    master_buffer[id].i2c_buffer.length = length;

    master_buffer[id].index = 0;
    master_buffer[id].retries = retries;
    master_buffer[id].target_address = target_address;
    master_buffer[id].command = (target_address << 1U) | I2C_CMD_WRITE_BIT;

    /* Switch internal state to master TX state and send a start condition on I2C bus */
    internal_configuration[id].state = I2C_STATE_MASTER_TRANSMITTING;
    internal_configuration[id].request_type = I2C_REQUEST_WRITE;
    *internal_configuration[id].handle._TWCR |= TWSTA_MSK;
    // Twint flag is set when setting TWSTA to TWCR
    //clear_twint(id);

    return I2C_ERROR_OK;
}

i2c_error_t i2c_read(const uint8_t id, const uint8_t target_address, uint8_t * const buffer, const uint8_t length, const bool has_opcode, const uint8_t retries)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == buffer)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    if (false == internal_configuration[id].is_initialised)
    {
        return I2C_ERROR_NOT_INITIALISED;
    }
    if (I2C_MAX_ADDRESS < target_address)
    {
        return I2C_ERROR_INVALID_ADDRESS;
    }
    // Don't check TWINT because when we try to use the driver for the first time,
    // TWI hardware does not know yet if an operation is ongoing and puts itself in a
    // waiting mode by default. This implies that TWINT is never set when the device boots up
    // because TWI hardware is waiting either for a request to be sent by itself as master or to receive data on I2C
    // bus.
    if (I2C_STATE_READY != internal_configuration[id].state)
    {
        return I2C_ERROR_ALREADY_PROCESSING;
    }
    if (length < MINIMUM_REQUEST_SIZE)
    {
        return I2C_ERROR_REQUEST_TOO_SHORT;
    }
    if (length > I2C_MAX_BUFFER_SIZE)
    {
        return I2C_ERROR_REQUEST_TOO_LONG;
    }

    /* Initialises internal buffer with supplied data */
    master_buffer[id].i2c_buffer.data = buffer;
    master_buffer[id].i2c_buffer.locked = true;
    master_buffer[id].i2c_buffer.length = length;

    master_buffer[id].index = 0;
    master_buffer[id].retries = retries;
    master_buffer[id].target_address = target_address;

    // Send opcode to slave device using a regular I2C master TX transmission
    if (true == has_opcode)
    {
        // First we need to write an operation code to our slave, so first send its address in write mode
        // Then once addressing is done and opcode is sent, switch to read mode !
        master_buffer[id].command = (target_address << 1U) | I2C_CMD_WRITE_BIT;

        /* Switch internal state to master TX state and send a start condition on I2C bus */
        internal_configuration[id].state = I2C_STATE_MASTER_TRANSMITTING;
    }
    else
    {
        // No opcode requested so we can directly address our slave straight away in read mode
        master_buffer[id].command = (target_address << 1U) | I2C_CMD_READ_BIT;

        /* Switch internal state to master TX state and send a start condition on I2C bus */
        internal_configuration[id].state = I2C_STATE_MASTER_RECEIVING;
    }
    internal_configuration[id].request_type = I2C_REQUEST_READ;
    *internal_configuration[id].handle._TWCR = (*internal_configuration[id].handle._TWCR & ~TWINT_MSK) | TWSTA_MSK;
    clear_twint(id);

    return I2C_ERROR_OK;
}

bool i2c_is_master_buffer_locked(const uint8_t id)
{
    return master_buffer[id].i2c_buffer.locked;
}

ISR(TWI_vect)
{
    process_helper();
}

void i2c_handle_vcopy(i2c_handle_t * dest, volatile i2c_handle_t const * const src)
{
    if (NULL == dest || NULL == src)
    {
        return;
    }

    dest->_TWAMR = src->_TWAMR;
    dest->_TWAR = src->_TWAR;
    dest->_TWBR = src->_TWBR;
    dest->_TWCR = src->_TWCR;
    dest->_TWDR = src->_TWDR;
    dest->_TWSR = src->_TWSR;
}

void i2c_handle_copyv(volatile i2c_handle_t * dest, i2c_handle_t const * const src)
{
    if (NULL == dest || NULL == src)
    {
        return;
    }

    dest->_TWAMR = src->_TWAMR;
    dest->_TWAR = src->_TWAR;
    dest->_TWBR = src->_TWBR;
    dest->_TWCR = src->_TWCR;
    dest->_TWDR = src->_TWDR;
    dest->_TWSR = src->_TWSR;
}
