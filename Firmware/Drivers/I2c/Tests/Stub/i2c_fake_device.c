#include <string.h>
#include <stdio.h>
#include "i2c_fake_device.h"
#include "i2c_device_interface.h"

#define I2C_CMD_WRITE_BIT 0U
#define I2C_CMD_READ_BIT 1U


/**
 * @brief gives information on the actually accessed data
*/
typedef struct
{
    uint8_t * buffer;   /**< Data currently being accessed                                                  */
    uint8_t length;     /**< Length of data being accessed                                                  */
    uint8_t index;      /**< Current index of data being accessed                                           */
    bool bad_access;    /**< Indicates whether a bad command was given or out-of-bounds data was accessed   */
} data_access_t;

/* Let the device know what kind of operation it is expected to perform at next process() call */
typedef enum
{
    MODE_IDLE,                              /**< Idle mode, starting point of the I2C transaction for this device           */
    MODE_MASTER_TO_SLAVE_ADDRESSING,        /**< Device is addressing a slave                                               */
    MODE_MASTER_TRANSMITTER,                /**< Device is writing data on I2C bus as master                                */
    MODE_MASTER_RECEIVER,                   /**< Device is receiveing data on I2C bus as master                             */
    MODE_SLAVE_WAIT_FOR_MASTER_ADDRESSING,  /**< Device is set as a slave and waits to be addressed by i2c bus master       */
    MODE_SLAVE_TRANSMITTER,                 /**< Device is set as a slave and is transmitting data to master (read command) */
    MODE_SLAVE_RECEIVER,                    /**< Device is set as a slave and is receiving data from master (write command) */
} i2c_fake_device_internal_state_machine_t;

/**
 * @brief Gives information about the current request (master mode only)
*/
typedef enum
{
    I2C_FAKE_DEVICE_REQUEST_TYPE_UNKNOWN = 0,   /**< Undefined request type, default                                                                                */
    I2C_FAKE_DEVICE_REQUEST_TYPE_READ,          /**< A read operation is ongoing, basically tells the device to operate a write + a read operation (multi-mode read)*/
    I2C_FAKE_DEVICE_REQUEST_TYPE_WRITE,         /**< A write operation is ongoing                                                                                   */
} i2c_fake_device_request_type_t;

/**
 * @brief packs data used to handle the master mode of this device
*/
typedef struct
{
    bool master_mode;                           /**< Flag that states whether master mode is activated or not (redundant with request type but semantically more correct)   */
    bool write_op;                              /**< Tells if current operation is write (true) or read (false), this is the atomic operation level of this i2C device      */
    uint8_t target_address;                     /**< Targeted slave address                                                                                                 */
    uint8_t * buffer;                           /**< Pointed buffer used to supply/receive data exchanged on I2C bus                                                        */
    uint8_t length;                             /**< Length of buffer                                                                                                       */
    uint8_t index;                              /**< Index of current byte of data of the buffer. Index is incremented when a byte is received/sent successfully            */
    uint8_t retries;                            /**< Allows to retry sending / receiving last byte of data in case of failure (NACK sent/received)                          */
    i2c_fake_device_request_type_t request;     /**< Gives the request type registered within the device as master(either Master Write or Master Read). Defaults to Unknown)*/
} master_data_t;

/**
 * @brief static datas used to drive this instance of device
*/
static i2c_device_interface_t interface = {0};
static i2c_exposed_data_t exposed_data = {0};
static data_access_t data_access = {0};
static master_data_t master_data = {0};
static bool force_nack = false;     /**< Used by slave data receiving routine to know if it has to force ack statement. */
                                    /**< It will be used by unit testing to inject communication errors while writing to this device as a slave */
// When a wrong command is sent, device will loop on this fake byte until the end of the master
// operation
static uint8_t wrong_access_requested = 0;
static bool needs_opcode = false;

// State machine handling structure
static struct
{
    i2c_fake_device_internal_state_machine_t current;
    i2c_fake_device_internal_state_machine_t previous;
} states = { .current = MODE_IDLE, .previous = MODE_IDLE };


/**
 * @brief interpretes command written by master and sets the read / write pointer to the right location
 * @return bool :
 *      true  : operation succeeded
 *      false : command is invalid
*/
static inline bool interprete_command(const uint8_t command);
static inline void reset_data_access(void);
static inline void slave_clear_flags_from_interface(void);
static inline void master_clear_flags_from_interface(void);
static inline void master_stop_device(void);

// Common handler for idle mode (not slave nor master)
static void handle_idle(void);

// Slave handlers
static void handle_slave_wait_for_master_addressing(void);
static void handle_slave_receiving_data(void);
static void handle_slave_transmitting_data(void);

// Master handlers
static void handle_master_addressing_slave(void);
static void handle_master_transmitting_data(void);
static void handle_master_receiving_data(void);


i2c_exposed_data_t * i2c_fake_device_get_exposed_data(void)
{
    return &exposed_data;
}

void i2c_fake_device_clear(void)
{
    memset(&exposed_data, 0, sizeof(i2c_exposed_data_t));
    memset(&interface, 0 , sizeof(i2c_device_interface_t));
    states.current = MODE_IDLE;
    states.previous = MODE_IDLE;
    snprintf(exposed_data.msg, I2C_FAKE_DEVICE_MSG_LEN, "Toto est au bistro!" );
    memset(&master_data, 0, sizeof(master_data_t));
    force_nack = false;
    reset_data_access();
}

void i2c_fake_device_force_nack(void)
{
    force_nack = true;
    needs_opcode = false;
}

void i2c_fake_device_init(const uint8_t address, const bool general_call_enabled, const bool has_opcode)
{
    i2c_fake_device_clear();
    reset_data_access();
    interface.address = address;
    interface.general_call_enabled = general_call_enabled;
    needs_opcode = has_opcode;
}

void i2c_fake_device_set_mode(const i2c_fake_device_operating_modes_t mode)
{
    exposed_data.mode = mode;
}

i2c_fake_device_error_t i2c_fake_device_write(const uint8_t address, uint8_t * buffer, const uint8_t length, const uint8_t retries)
{
    if (MODE_IDLE != states.current)
    {
        return I2C_FAKE_DEVICE_ERROR_ALREADY_PROCESSING;
    }

    master_data.master_mode = true;
    master_data.target_address = address;
    master_data.length = length;
    master_data.index = 0;
    master_data.buffer = buffer;
    master_data.retries = retries;
    master_data.request = I2C_FAKE_DEVICE_REQUEST_TYPE_WRITE;
    master_data.write_op = true;

    return I2C_FAKE_DEVICE_ERROR_OK;
}

i2c_fake_device_error_t i2c_fake_device_read(const uint8_t address, uint8_t * buffer, const uint8_t length, const uint8_t retries)
{
    if (MODE_IDLE != states.current)
    {
        return I2C_FAKE_DEVICE_ERROR_ALREADY_PROCESSING;
    }

    master_data.master_mode = true;
    master_data.target_address = address;
    master_data.length = length;
    master_data.index = 0;
    master_data.buffer = buffer;
    master_data.retries = retries;
    master_data.request = I2C_FAKE_DEVICE_REQUEST_TYPE_READ;
    master_data.write_op = true;

    return I2C_FAKE_DEVICE_ERROR_OK;
}

void i2c_fake_device_process(const uint8_t id)
{
    (void) id;
    switch(states.current)
    {
        case MODE_IDLE:
            handle_idle();
            break;

        case MODE_SLAVE_WAIT_FOR_MASTER_ADDRESSING:
            handle_slave_wait_for_master_addressing();
            break;

        case MODE_MASTER_TO_SLAVE_ADDRESSING:
            handle_master_addressing_slave();
            break;

        case MODE_SLAVE_TRANSMITTER:
            handle_slave_transmitting_data();
            break;

        case MODE_SLAVE_RECEIVER:
            handle_slave_receiving_data();
            break;

        case MODE_MASTER_TRANSMITTER:
            handle_master_transmitting_data();
            break;

        case MODE_MASTER_RECEIVER:
            handle_master_receiving_data();
            break;

        default:
            data_access.buffer = &wrong_access_requested;

            break;
    }
}

void i2c_fake_device_get_interface(const uint8_t bus_id, i2c_device_interface_t ** const p_interface)
{
    (void) bus_id;
    *p_interface = &interface;
}

static inline bool interprete_command(const uint8_t command)
{
    bool succeeded = true;
    switch (command)
    {
        case I2C_FAKE_DEVICE_CMD_MESSAGE:
            data_access.bad_access = false;
            data_access.index = 0;
            data_access.buffer = (uint8_t *) exposed_data.msg;
            data_access.length = I2C_FAKE_DEVICE_MSG_LEN;
            break;

        case I2C_FAKE_DEVICE_CMD_THERMAL_THRESHOLD:
            data_access.bad_access = false;
            data_access.index = 0;
            data_access.buffer = &exposed_data.thermal_threshold;
            data_access.length = 1U;
            break;

        case I2C_FAKE_DEVICE_CMD_TEMPERATURE_1:
            data_access.bad_access = false;
            data_access.index = 0;
            data_access.buffer = &exposed_data.temperature_1;
            data_access.length = 1U;
            break;

        case I2C_FAKE_DEVICE_CMD_TEMPERATURE_2:
            data_access.bad_access = false;
            data_access.index = 0;
            data_access.buffer = &exposed_data.temperature_2;
            data_access.length = 1U;
            break;

        case I2C_FAKE_DEVICE_CMD_MODE_CHANGE:
            data_access.bad_access = false;
            data_access.index = 0;
            data_access.buffer = (uint8_t * ) &exposed_data.mode;
            data_access.length = 1U;
            break;

        case I2C_FAKE_DEVICE_CMD_UNKNOWN:
        default:
            // Will force device to loop on fake data instead of overflowing
            reset_data_access();
            data_access.bad_access = true;
            succeeded = false;
            break;
    }
    return succeeded;
}

static inline void reset_data_access(void)
{
    data_access.buffer = &wrong_access_requested;
    data_access.index = 0;
    data_access.length = 1;
}


static void handle_idle(void)
{
    reset_data_access();
    if (states.previous != states.current)
    {
        slave_clear_flags_from_interface();
    }
    states.previous = states.current;

    // Checks if start was sent on I2C bus and consume it (reset start_sent to false)
    // If start sent, switch to MODE_SLAVE_WAIT_FOR_MASTER_ADDRESSING
    if(interface.start_sent)
    {
        interface.start_sent = false;
        states.current = MODE_SLAVE_WAIT_FOR_MASTER_ADDRESSING;
        slave_clear_flags_from_interface();
    }
    else
    {
        // Otherwise, maybe this device is trying to become the new master of the bus
        if (true == master_data.master_mode)
        {
            interface.start_sent = true;
            states.current = MODE_MASTER_TO_SLAVE_ADDRESSING;
        }
    }

    // else, loop back in idle mode ...
}

static void handle_slave_wait_for_master_addressing(void)
{
    // Check incoming address
    // if address matches my own address,
    //   post an 'ACK' on interface
    //   if command write :
    //      switch to MODE_SLAVE_RECEIVER
    //   else command read
    //      switch to MODE_SLAVE_TRANSMITTER
    // else post NACK on interface
    //      switch back to MODE_IDLE
    const uint8_t address = ((interface.data & 0xFE) >> 1U);
    const uint8_t command = interface.data &0x01;

    states.previous = states.current;
    if (interface.address == address)
    {
        interface.ack_sent = true;
        if (I2C_CMD_WRITE_BIT == command)
        {
            states.current = MODE_SLAVE_RECEIVER;
        }
        else
        {
            states.current = MODE_SLAVE_TRANSMITTER;
        }
    }
    // General call
    else
    {
        if(0x00 == ((interface.data & 0xFE) >> 1U))
        {
            // Only available choice, general call cannot be sent with
            // the read command
            if (I2C_CMD_WRITE_BIT == command && interface.general_call_enabled)
            {
                states.current = MODE_SLAVE_RECEIVER;
            }
            else
            {
                interface.ack_sent = false;
            }
        }
        else
        {
            interface.ack_sent = false;
        }
    }
    slave_clear_flags_from_interface();
}

static void handle_slave_receiving_data(void)
{
    // Master wants to switch back to addressing mode
    if (interface.start_sent)
    {
        // Fallback in slave addressing mode (next byte will contain address + command)
        states.previous = states.current;
        states.current = MODE_SLAVE_WAIT_FOR_MASTER_ADDRESSING;
        return;
    }

    // Stop detected, stop further processing
    if (interface.stop_sent)
    {
        states.current = MODE_IDLE;
        states.previous = MODE_IDLE;
        reset_data_access();
        slave_clear_flags_from_interface();
        return;
    }

    // First byte contains command from master
    if (MODE_SLAVE_WAIT_FOR_MASTER_ADDRESSING == states.previous)
    {
        bool succeeded = interprete_command(interface.data);

        // Command was valid !
        if (true == succeeded)
        {
            interface.ack_sent = true;
            states.previous = states.current;
        }
        else
        {
            interface.ack_sent = false;
            // Master will resend the same command and maybe send a stop command
        }
    }
    // Not the first byte, data is being transferred
    // Will receive data using accessed_data structure
    else
    {
        // Overrides normal behavior of slave device if used
        if (true == force_nack)
        {
            interface.ack_sent = false;
            // Force flag consumed, reset it to false
            force_nack = false;
        }


        if(false == force_nack)
        {
            // Guard previous bad access (out-of-bounds for instance)
            if (true == data_access.bad_access)
            {
                *data_access.buffer = interface.data;
            }
            else
            {
                if (data_access.index < data_access.length)
                {
                    data_access.buffer[data_access.index] = interface.data;
                    data_access.index++;
                }
                else
                {
                    data_access.bad_access = true;
                    reset_data_access();
                }

            }
            // If goes here, assume transmission went ok
            // NACK case is handled by manually setting the nack state via force_
            interface.ack_sent = true;
        }
    }
    slave_clear_flags_from_interface();

}

static inline void slave_clear_flags_from_interface(void)
{
    interface.start_sent = false;
    interface.stop_sent = false;
}

static inline void master_clear_flags_from_interface(void)
{
    interface.start_sent = false;
    interface.stop_sent = false;
}

static inline void master_stop_device(void)
{
    interface.start_sent = false;
    interface.stop_sent = true;
    master_data.master_mode = false;
    master_data.request = I2C_FAKE_DEVICE_REQUEST_TYPE_UNKNOWN;
}


static void handle_slave_transmitting_data(void)
{
    // Master wants to switch back to addressing mode
    if (interface.start_sent)
    {
        // Fallback in slave addressing mode (next byte will contain address + command)
        states.previous = states.current;
        states.current = MODE_SLAVE_WAIT_FOR_MASTER_ADDRESSING;
        return;
    }

    // Stop detected, stop further processing
    if (interface.stop_sent)
    {
        states.current = MODE_IDLE;
        states.previous = MODE_IDLE;
        reset_data_access();
        slave_clear_flags_from_interface();
        return;
    }

    // First byte does not contain any command : the read command assumes a write was done before to make the
    // exposed data point to the right place. So all bytes starting from here are bytes that will be transmitted back to master
    if( MODE_SLAVE_TRANSMITTER == states.previous)
    {
        if (true == interface.ack_sent)
        {
            data_access.index++;
        }
    }
    else if (MODE_SLAVE_WAIT_FOR_MASTER_ADDRESSING == states.previous)
    {
        if (false == needs_opcode)
        {
            data_access.buffer = (uint8_t*) &exposed_data.mode;
            data_access.length = 1U;
            data_access.index = 0;
            data_access.bad_access = false;
        }
    }

    // Guard against wrong reads !
    if (true == data_access.bad_access)
    {
        // Loop on fake data (0)
        interface.data = *data_access.buffer;
    }
    else
    {
        if (data_access.index < data_access.length)
        {
            interface.data = data_access.buffer[data_access.index];
            // Index will be incremented on next loop, only if master responded with an Acknowledgement flag
        }
        else
        {
            // Still send data, but send fake one instead!
            data_access.bad_access = true;
            data_access.buffer = &wrong_access_requested;
            data_access.index = 0;
            data_access.length = 1;
        }
    }
    states.previous = MODE_SLAVE_TRANSMITTER;
}

static void handle_master_addressing_slave(void)
{
    interface.start_sent = false;
    states.previous = states.current;

    // If opcode needs to be sent, first we have to write it on the I2C bus as
    // a master TX transmission
    if (true == master_data.write_op)
    {
        interface.data = (master_data.target_address << 1U) | I2C_CMD_WRITE_BIT;
        states.current = MODE_MASTER_TRANSMITTER;
    }
    else
    {
        interface.data = (master_data.target_address << 1U) | I2C_CMD_READ_BIT;
        states.current = MODE_MASTER_RECEIVER;
    }
}

static void handle_master_transmitting_data(void)
{
    static uint8_t retries = 0;
    master_clear_flags_from_interface();

    // Handle previously sent byte acknowledgment
    if (MODE_MASTER_TO_SLAVE_ADDRESSING == states.previous)
    {
        // No slave responded to the requested address
        if (false == interface.ack_sent)
        {
            master_data.retries++;
            interface.data = (master_data.target_address << 1U) | I2C_CMD_WRITE_BIT;
        }
        else
        {
            // Only then, validate the transition and continue with remaining bytes transmission
            states.previous = states.current;
        }
    }
    else
    {
        if (true == interface.ack_sent)
        {
            master_data.index++;
        }
        else
        {
            retries++;
        }
    }

    // In case of a read request, as soon as we successfully sent the first byte of the buffer (slave's operation code byte)
    // switch back to Master addressing mode and resend a start condition
    if ((I2C_FAKE_DEVICE_REQUEST_TYPE_READ == master_data.request)
     && (1 == master_data.index))
    {
        interface.start_sent = true;
        states.current = MODE_MASTER_TO_SLAVE_ADDRESSING;
        master_data.write_op = false;
    }
    else
    {
        // Handle buffer overflows before they occur and stop transmission
        if (master_data.index < master_data.length)
        {
            interface.data = master_data.buffer[master_data.index];
        }
        else
        {
            master_stop_device();
            states.current = MODE_IDLE;
        }
    }

    // Max retries count hit : release control on I2C bus and reset device
    if ((retries != 0 ) && (retries >= master_data.retries))
    {
        master_stop_device();
        states.current = MODE_IDLE;
    }

}

static void handle_master_receiving_data(void)
{
    master_clear_flags_from_interface();

    // We just switched to this mode : this means the last byte
    // in interface.data still contains the address + read bit. Master needs to consume the
    // Acknowledgment first, then it might start to effectively read data
    if (states.previous != states.current)
    {
        if(interface.ack_sent)
        {
            states.previous = states.current;
        }
        else
        {
            // Slave did not recognize itself in read mode, retry or stop here.
            interface.stop_sent = true;
            master_stop_device();
            states.current = MODE_IDLE;
        }
    }
    else
    {
        if (master_data.index < master_data.length)
        {
            master_data.buffer[master_data.index] = interface.data;
            interface.ack_sent = true;
            master_data.index++;
            // If we reach the end of the buffer, next call to this function will send the stop condition over the bus
        }
        else
        {
            interface.ack_sent = false;
            master_stop_device();

            states.current = MODE_IDLE;
        }
    }
}
