#include <stddef.h>
#include "i2c.h"
#include "i2c_slave_handler.h"

static struct
{
    struct
    {
        uint8_t index;
        uint8_t length;
    } virtual_buffer;
    i2c_slave_command_t command;   /**< Currently selected command */
    uint8_t processed_bytes;
    i2c_request_t last_request;
} tracking;

i2c_slave_exposed_data_t i2c_slave_exposed_data = {0};

/**
 * @brief handles data coming from the master device (so the slave reads data from the i2c bus and processes it)
 * @param[in] byte : received byte
 * As it could be confusing, this handler is used when the master device tries to write to a slave (master tx / slave rx)
*/
static i2c_slave_handler_error_t slave_data_handler_read(uint8_t * const byte);

/**
 * @brief handles data to be sent to the master device. Slave device writes data from its i2c_slave_exposed_data structure onto the i2c bus to be
 * transmitted back to master device.
 * As it could be confusing, this handler is used when the master devices tries to read from a slave (master rx / slave tx)
 * @param[in] byte : received byte
*/
static i2c_slave_handler_error_t slave_data_handler_write(uint8_t * const byte);

i2c_slave_handler_error_t i2c_slave_data_handler(uint8_t * const byte, const i2c_request_t request)
{
    i2c_slave_handler_error_t ret = I2C_SLAVE_HANDLER_ERROR_OK;
    if (NULL == byte)
    {
        return I2C_SLAVE_HANDLER_ERROR_BUFFER_NULLPTR;
    }

    if (request == I2C_REQUEST_READ)
    {
        return slave_data_handler_read(byte);
    }
    else
    {
        return slave_data_handler_write(byte);
    }

    return ret;
}

i2c_slave_handler_error_t i2c_slave_transmission_over_callback(void)
{
    // Do not reset last command byte as we can have a master TX and then a master RX for the same command byte
    tracking.virtual_buffer.index = 0;

    // Reset last request as well, so that we can determine at next data handler call
    // that this is a new transmission
    tracking.last_request = I2C_REQUEST_IDLE;

    // Unlock exposed data
    i2c_slave_exposed_data.locked = false;

    return I2C_SLAVE_HANDLER_ERROR_OK;
}


i2c_slave_handler_error_t slave_data_handler_read(uint8_t * const byte)
{
    i2c_slave_handler_error_t ret = I2C_SLAVE_HANDLER_ERROR_OK;
    if ( (0 != tracking.processed_bytes)
    &&  ( tracking.virtual_buffer.index >= tracking.virtual_buffer.length))
    {
        return I2C_SLAVE_HANDLER_ERROR_BUFFER_OVERFLOW_GUARD;
    }

    // Start of a new transmission
    if (tracking.last_request != I2C_REQUEST_READ)
    {
        tracking.processed_bytes = 0;
        tracking.virtual_buffer.index = 0;
        tracking.virtual_buffer.length = 0;
        tracking.last_request = I2C_REQUEST_READ;
        tracking.command = I2C_SLAVE_COMMAND_UNDEFINED;
    }

    // First byte read from master is a command byte for this application
    if ((0 == tracking.virtual_buffer.index)
    &&  (0 == tracking.processed_bytes))
    {
        tracking.command = (i2c_slave_command_t) *byte;
        switch(tracking.command)
        {
            case I2C_SLAVE_COMMAND_MODE:
            case I2C_SLAVE_COMMAND_SWITCH_ON_OFF:
                tracking.virtual_buffer.length = 1;
                break;

            // Those commands do not support read operations
            case I2C_SLAVE_COMMAND_MEAS_SPEED:
            case I2C_SLAVE_COMMAND_MEAS_TEMP:
            case I2C_SLAVE_COMMAND_MEAS_POWER:
                tracking.virtual_buffer.length = 0;
                break;

            // Not a valid command bro !
            case I2C_SLAVE_COMMAND_UNDEFINED:
            default:
                ret = I2C_SLAVE_HANDLER_ERROR_UNKNOWN_COMMAND;
                break;
        }

        tracking.processed_bytes++;
    }
    // Handle the read command now
    else
    {
        if (tracking.virtual_buffer.index < tracking.virtual_buffer.length)
        {
            // Protects exposed data
            i2c_slave_exposed_data.locked = true;
            switch(tracking.command)
            {
                // This command requires special handling as we need to validate incoming data.
                // If sent data is not valid, we may suspect a communication error and inform the master
                // device something is wrong
                case I2C_SLAVE_COMMAND_MODE:
                    {
                        i2c_slave_operating_modes_t temp_mode = i2c_slave_operating_modes_from_byte(*byte);
                        if (I2C_SLAVE_OPERATING_MODE_UNDEFINED == i2c_slave_exposed_data.mode)
                        {
                            ret = I2C_SLAVE_HANDLER_ERROR_INVALID_PAYLOAD;
                        }
                        else
                        {
                            i2c_slave_exposed_data.mode = temp_mode;
                        }
                    }
                    break;

                 // byte is discarded, writting to the measured values has no effect
                case I2C_SLAVE_COMMAND_MEAS_POWER :
                case I2C_SLAVE_COMMAND_MEAS_TEMP :
                case I2C_SLAVE_COMMAND_MEAS_SPEED :
                    (void) byte;
                    break;

                case I2C_SLAVE_COMMAND_SWITCH_ON_OFF:
                    i2c_slave_exposed_data.enabled = (bool) *byte;
                    break;

                case I2C_SLAVE_COMMAND_UNDEFINED:
                default:
                    tracking.virtual_buffer.index = 0;
                    tracking.virtual_buffer.length = 0;
                    ret = I2C_SLAVE_HANDLER_ERROR_UNKNOWN_COMMAND;
                    break;
            }

            // If everything is right at that point, don't forget to increment
            // internal tracking index
            if (I2C_SLAVE_HANDLER_ERROR_OK == ret)
            {
                tracking.virtual_buffer.index++;
                tracking.processed_bytes++;
            }
        }
        else
        {
            ret = I2C_SLAVE_HANDLER_ERROR_BUFFER_OVERFLOW_GUARD;
            i2c_slave_exposed_data.locked = false;
        }

    }
    return ret;
}

i2c_slave_handler_error_t slave_data_handler_write(uint8_t * byte)
{
    i2c_slave_handler_error_t ret = I2C_SLAVE_HANDLER_ERROR_OK;
    if (tracking.virtual_buffer.index >= tracking.virtual_buffer.length)
    {
        return I2C_SLAVE_HANDLER_ERROR_BUFFER_OVERFLOW_GUARD;
    }

    // Start of a new transmission
    // Don't touch at the command byte as this time this slave has to remember what
    // was the requested memory area
    if (tracking.last_request != I2C_REQUEST_WRITE)
    {
        tracking.processed_bytes = 0;
        tracking.virtual_buffer.index = 0;
        tracking.last_request = I2C_REQUEST_WRITE;
    }

    switch(tracking.command)
    {
        case I2C_SLAVE_COMMAND_MEAS_SPEED:
            *byte = i2c_slave_exposed_data.speed;
            break;

        // Measured power is encoded on a 16-bits wide unsigned integer.
        // So we'll need to send the lower bits first (0-7) and send the higer order bits in
        // next send (8-15)
        case I2C_SLAVE_COMMAND_MEAS_POWER:
            *byte = ((uint8_t*)(&i2c_slave_exposed_data.power))[tracking.virtual_buffer.index];
            break;

        // Temperature data is encoded as a 8-bit wide signed integer (maps from -80°C to +110°C range)
        // So we'll have to cast it as an uint8_t first to comply with the data format
        // Then data should be decoded by master upon reception.
        case I2C_SLAVE_COMMAND_MEAS_TEMP:
            *byte = (uint8_t)i2c_slave_exposed_data.temperature;
            break;

        // Current power profile (mode) selection does not need further protection,
        // we assume that data from i2c_slave_exposed_data structure is always posted valid by the application software.
        // We can simply perform a direct cast then.
        case I2C_SLAVE_COMMAND_MODE:
            *byte = (uint8_t) i2c_slave_exposed_data.mode;
            break;

        // This commands is not meant to be read (device's power state)
        // although we can implement it to be readable, let's pretend it's part of the slave device's restriction
        // not to be able to read from it.
        case I2C_SLAVE_COMMAND_SWITCH_ON_OFF:
            *byte = 0U;
            break;

        default:
            *byte = 0;
            tracking.virtual_buffer.index = 0;
            tracking.virtual_buffer.length = 0;
            ret = I2C_SLAVE_HANDLER_ERROR_UNKNOWN_COMMAND;
            break;
    }

    // If everything is right at that point, don't forget to increment
    // internal tracking index
    if (I2C_SLAVE_HANDLER_ERROR_OK == ret)
    {
        tracking.virtual_buffer.index++;
    }
    return I2C_SLAVE_HANDLER_ERROR_OK;;
}


i2c_slave_operating_modes_t i2c_slave_operating_modes_from_byte(const uint8_t byte)
{
    i2c_slave_operating_modes_t outmode = I2C_SLAVE_OPERATING_MODE_UNDEFINED;

    // Only peform a direct cast when byte is within acceptable range otherwise return the undefined state to
    // inform about an invalid conversion
    if (byte <= I2C_SLAVE_OPERATING_MODE_100)
    {
        outmode = (i2c_slave_operating_modes_t) byte;
    }
    return outmode;
}
