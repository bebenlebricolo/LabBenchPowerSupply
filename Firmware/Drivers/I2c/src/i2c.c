#include "i2c.h"
#include "config.h"

#include <stddef.h>
#include <string.h>

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
