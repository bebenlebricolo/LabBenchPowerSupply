#include "i2c.h"
#include "config.h"

#include <stddef.h>
#include <string.h>

#define I2C_MAX_ADDRESS 127U

#ifndef I2C_DEVICES_COUNT
    #error "I2C_DEVICES_COUNT is not defined. Please add #define I2C_DEVICES_COUNT in config.h to use this timer"
#elif I2C_DEVICES_COUNT == 0
    #warning "I2C_DEVICES_COUNT is set to 0. If you don't project to use this timer, prefer to not compile this file instead of setting this define to 0"
#endif

static struct
{
    bool is_initialised;
    i2c_handle_t handle;
    i2c_state_t state;
    void (*i2c_command_handler)(uint8_t * const data_byte);
} internal_configuration[I2C_DEVICES_COUNT] = {0};

static volatile struct
{
    uint8_t command;    /**< Contains target address + read/write bit                           */
    uint8_t index;      /**< Increment used in Rx/Tx mode to iterate though the internal buffer */
    uint8_t retries;

    uint8_t length;     /**< Length of Read/Write command                                       */
    uint8_t * buffer;   /**< Source/Destination buffer                                          */
} internal_buffer[I2C_DEVICES_COUNT] = {0};

/* handlers used to process in/out data when TWI peripheral needs servicing */
static i2c_error_t i2c_master_tx_process(const uint8_t id);
static i2c_error_t i2c_master_rx_process(const uint8_t id);
static i2c_error_t i2c_slave_tx_process(const uint8_t id);
static i2c_error_t i2c_slave_rx_process(const uint8_t id);


/* In case no handler is given, simply discard incoming data */
static inline void default_command_handler(uint8_t * const data_byte)
{
    (void) data_byte;
}

static inline bool is_handle_initialised(const uint8_t id)
{
    bool out = false;
    out |= (NULL == internal_configuration[id].handle.TWCR);
    out |= (NULL == internal_configuration[id].handle.TWAMR);
    out |= (NULL == internal_configuration[id].handle.TWBR);
    out |= (NULL == internal_configuration[id].handle.TWDR);
    out |= (NULL == internal_configuration[id].handle.TWSR);
    out |= (NULL == internal_configuration[id].handle.TWAR);
    return out;
}

static inline bool is_id_valid(const uint8_t id)
{
    return id < I2C_DEVICES_COUNT;
}

static inline bool is_twint_set(const uint8_t id)
{
    return (0 != ((*internal_configuration[id].handle.TWCR) & TWINT_MSK));
}

static inline void reset_handle(i2c_handle_t * const handle)
{
   (void)memset(handle, 0, sizeof(i2c_handle_t));
}

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
    config->slave_address = 0;

    return I2C_ERROR_OK;
}

i2c_error_t i2c_set_handle(const uint8_t id, const i2c_handle_t * const handle)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == handle)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    memcpy(&internal_configuration[id].handle, handle, sizeof(i2c_handle_t));
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

    memcpy(handle, &internal_configuration[id].handle, sizeof(i2c_handle_t));
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

    i2c_handle_t * handle = &internal_configuration[id].handle;
    *(handle->TWAR) = (*(handle->TWAR) & TWA_MSK) | (address << 1U);

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

    *address = (*(internal_configuration[id].handle.TWAR) & TWA_MSK) >> 1U;
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

    *(internal_configuration[id].handle.TWSR) = ((*(internal_configuration[id].handle.TWSR) & TWPS_MSK) | prescaler);
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

    *prescaler = *(internal_configuration[id].handle.TWSR) & TWPS_MSK;
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

    *(internal_configuration[id].handle.TWBR) = baudrate;
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

    *baudrate = *(internal_configuration[id].handle.TWBR);
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
        *(internal_configuration[id].handle.TWAR) |= TWGCE_MSK;
    }
    else
    {
        *(internal_configuration[id].handle.TWAR) &= ~TWGCE_MSK;
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

    *general_call_enabled = (bool) *(internal_configuration[id].handle.TWAR) & TWGCE_MSK;
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
        *(internal_configuration[id].handle.TWCR) |= TWIE_MSK;
    }
    else
    {
        *(internal_configuration[id].handle.TWCR) &= ~TWIE_MSK;
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

    *use_interrupts = (bool) *(internal_configuration[id].handle.TWCR) & TWIE_MSK;
    return I2C_ERROR_OK;
}

i2c_error_t i2c_get_status_code(const uint8_t id, uint8_t * const status_code)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == status_code)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    *status_code = ((*internal_configuration[id].handle.TWSR & TWS_MSK) >> TWS3_BIT);
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

    *(internal_configuration[id].handle.TWCR) |= TWEN_MSK;
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

    *(internal_configuration[id].handle.TWCR) &= ~TWEN_MSK;
    internal_configuration[id].state = I2C_STATE_DISABLED;
    return I2C_ERROR_OK;
}

i2c_error_t i2c_slave_set_command_handler(const uint8_t id, void (*i2c_slave_command_handler)(uint8_t * const data_byte))
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == i2c_slave_command_handler)
    {
        return I2C_ERROR_NULL_POINTER;
    }

    internal_configuration[id].i2c_command_handler = i2c_slave_command_handler;
    return I2C_ERROR_OK;
}

static i2c_error_t write_config(const uint8_t id, const i2c_config_t * const config)
{
    if (!is_handle_initialised(id))
    {
        return I2C_ERROR_NULL_HANDLE;
    }

    /* Baudrate */
    *(internal_configuration[id].handle.TWBR) = config->baudrate;

    /* Prescaler */
    *(internal_configuration[id].handle.TWSR) &= ~TWPS_MSK;
    *(internal_configuration[id].handle.TWSR) |= config->prescaler;

    /* Slave address */
    *(internal_configuration[id].handle.TWAR) &= ~TWA_MSK;
    *(internal_configuration[id].handle.TWAR) |= (config->slave_address << 1U);

    /* General call enabled flag */
    if (true == config->general_call_enabled)
    {
        *(internal_configuration[id].handle.TWAR) &= ~TWGCE_MSK;
    }
    else
    {
        *(internal_configuration[id].handle.TWAR) |= TWGCE_MSK;
    }

    /* Interrupt enabled flag */
    if (true == config->interrupt_enabled)
    {
        *(internal_configuration[id].handle.TWCR) &= ~TWIE_MSK;
    }
    else
    {
        *(internal_configuration[id].handle.TWCR) |= TWIE_MSK;
    }

    return I2C_ERROR_OK;
}

i2c_error_t i2c_init(const uint8_t id, const i2c_config_t * const config)
{
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

static i2c_error_t i2c_master_tx_process(const uint8_t id)
{
    static uint8_t retries = 0;
    i2c_error_t ret = I2C_ERROR_OK;

    if (internal_buffer[id].index == internal_buffer[id].length)
    {
        // Reached the end of the buffer, stop there
    }
    else
    {
        i2c_slave_transmitter_mode_status_codes_t status;
        ret = i2c_get_status_code(id, (uint8_t * ) &status);
        if (I2C_ERROR_OK != ret)
        {
            // return here because if we can't read device's registers, we should stop any execution
            return ret;
        }

        // Interprete status code:
        switch (status)
        {
            case MAS_TX_START_TRANSMITTED:
            case MAS_TX_REPEATED_START:
                // Transmition was successfully started
                // Send slave address + write flag to start writing
                *internal_configuration[id].handle.TWDR = internal_buffer[id].command;
                *internal_configuration[id].handle.TWCR |= TWINT_MSK;
                break;
            case MAS_TX_SLAVE_WRITE_ACK:
                // Slave replied ACK to its address in write mode, proceed further and send next byte
                *internal_configuration[id].handle.TWDR = internal_buffer[id].buffer[internal_buffer[id].index] ;
                *internal_configuration[id].handle.TWCR |= TWINT_MSK;
                retries = 0;
                break;
            case MAS_TX_SLAVE_WRITE_NACK:
                // Slave did not reply correcly to its address, or some issue were encountered on I2C line
                // Resend Slave + write command
                *internal_configuration[id].handle.TWDR = internal_buffer[id].command;
                *internal_configuration[id].handle.TWCR |= TWINT_MSK;
                retries++;
                break;
            case MAS_TX_DATA_RECEIVED_ACK:
                internal_buffer[id].index++;
                if (internal_buffer[id].length != internal_buffer[id].index)
                {
                    // Send next byte of data
                    *internal_configuration[id].handle.TWDR = internal_buffer[id].buffer[internal_buffer[id].index] ;
                    *internal_configuration[id].handle.TWCR |= TWINT_MSK;
                }
                else
                {
                    // If we hit the end of the buffer, stop the transmition
                    *internal_configuration[id].handle.TWCR |= TWSTO_MSK;
                    *internal_configuration[id].handle.TWCR |= TWINT_MSK;
                    internal_configuration[id].state = I2C_STATE_READY;
                }
                retries = 0;
                break;
            case MAS_TX_DATA_RECEIVED_NACK:
                // Resend last byte of data
                *internal_configuration[id].handle.TWDR = internal_buffer[id].buffer[internal_buffer[id].index] ;
                *internal_configuration[id].handle.TWCR |= TWINT_MSK;
                retries++;
                break;
            case MAS_TX_ABRITRATION_LOST:
            default:
                // Restore peripheral to its original state
                internal_configuration[id].state = I2C_STATE_READY;
                retries = 0;
                break;
        }

        // If maximum retries count was hit, reset peripheral back to its default state and
        // end the transmition by writing a Stop condition
        if ((0 != internal_buffer[id].retries)
         && (internal_buffer[id].retries < retries))
        {
            internal_configuration[id].state = I2C_STATE_READY;
            *internal_configuration[id].handle.TWCR |= TWSTO_MSK;
            *internal_configuration[id].handle.TWCR |= TWINT_MSK;
            retries = 0;
            ret = I2C_ERROR_MAX_RETRIES_HIT;
        }
    }

    return ret;
}


static i2c_error_t process_helper_single(const uint8_t id)
{
    i2c_error_t ret = I2C_ERROR_OK;
    switch(internal_configuration[id].state)
    {
        /* TWINT is raised while no operation asked : this is the slave mode being activated by I2C bus */
        case I2C_STATE_READY:
            /* TODO : retrieve which mode between I2C slave receive or I2C slave transmit we are in */
            break;
        /* Either a Start condition written from i2c_write was sent or a Tx operation is already ongoing */
        case I2C_STATE_MASTER_TRANSMITTING:
            ret = i2c_master_tx_process(id);
            break;
        /* Either a Start condition was written from i2c_read or a Rx operation is already ongoing */
        case I2C_STATE_MASTER_RECEIVING:
            ret = i2c_master_rx_process(id);
            break;
        /* A Slave receive operation is ongoing */
        case I2C_STATE_SLAVE_TRANSMITTING:
            ret = i2c_slave_tx_process(id);
            break;
        /* A Slave transmit operation is ongoing */
        case I2C_STATE_SLAVE_RECEIVING:
            ret = i2c_slave_rx_process(id);
            break;
        default:
            /* Do nothing otherwise */
            ret = I2C_ERROR_WRONG_STATE;
            break;
    }
    return ret;
}

/* Iterates over available i2c devices to find which one needs servicing */
static void process_helper(void)
{
    for(uint8_t i = 0 ; i < I2C_DEVICES_COUNT ; i++)
    {
        if (is_twint_set(i))
        {
            process_helper_single(i);
        }
    }
}

i2c_error_t i2c_process(const uint8_t id)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    process_helper_single(id);
    return I2C_ERROR_OK;
}

i2c_error_t i2c_write(const uint8_t id, const uint8_t target_address , const uint8_t * const buffer, const uint8_t length, const uint8_t retries)
{
    if (!is_id_valid(id))
    {
        return I2C_ERROR_DEVICE_NOT_FOUND;
    }
    if (NULL == buffer)
    {
        return I2C_ERROR_NULL_POINTER;
    }
    if (I2C_MAX_ADDRESS < target_address)
    {
        return I2C_ERROR_INVALID_ADDRESS;
    }
    if (I2C_STATE_READY != internal_configuration[id].state || is_twint_set(id))
    {
        return I2C_ERROR_ALREADY_PROCESSING;
    }

    /* Initialises internal buffer with supplied data */
    internal_buffer[id].buffer = buffer;
    internal_buffer[id].length = length;
    internal_buffer[id].index = 0;
    internal_buffer[id].retries = retries;
    internal_buffer[id].command = (target_address << 1U) | I2C_CMD_WRITE_BIT;

    /* Switch internal state to master TX state and send a start condition on I2C bus */
    internal_configuration[id].state = I2C_STATE_MASTER_TRANSMITTING;
    *internal_configuration[id].handle.TWCR |= TWSTA_MSK;

    return I2C_ERROR_OK;
}