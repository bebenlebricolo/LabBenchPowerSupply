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
    MODE_MASTER_TRANSMITTER,                /**< Device is writing data on I2C bus as master                                */
    MODE_MASTER_RECEIVER,                   /**< Device is receiveing data on I2C bus as master                             */
    MODE_SLAVE_WAIT_FOR_MASTER_ADDRESSING,  /**< Device is set as a slave and waits to be addressed by i2c bus master       */
    MODE_SLAVE_TRANSMITTER,                 /**< Device is set as a slave and is transmitting data to master (read command) */
    MODE_SLAVE_RECEIVER,                    /**< Device is set as a slave and is receiving data from master (write command) */
} i2c_fake_device_internal_state_machine_t;

static i2c_device_interface_t interface = {0};
static i2c_exposed_data_t exposed_data = {0};
static data_access_t data_access = {0};

/**
 * @brief used by slave data receiving routine to know if it has to force ack statement
 * It will be used by unit testing to inject communication errors while writing to this device as a slave
*/
static bool force_nack = false;

// State machine handling structure
static struct
{
    i2c_fake_device_internal_state_machine_t current;
    i2c_fake_device_internal_state_machine_t previous;
} states = { .current = MODE_IDLE, .previous = MODE_IDLE };

// When a wrong command is sent, device will loop on this fake byte until the end of the master
// operation
static uint8_t wrong_access_requested = 0;

/**
 * @brief interpretes command written by master and sets the read / write pointer to the right location
 * @return bool :
 *      true  : operation succeeded
 *      false : command is invalid
*/
static inline bool interprete_command(const uint8_t command);
static inline void reset_data_access(void);
static inline void slave_clear_flags_from_interface(void);

// Slave handlers
static void handle_idle(void);
static void handle_slave_wait_for_master_addressing(void);
static void handle_slave_receiving_data(void);
static void handle_slave_transmitting_data(void);

// Master handlers
// Not implemented yet
static void handle_master_addressing_slave(void) {}
static void handle_master_transmitting_data(void) {}
static void handle_master_receiving_data(void) {}


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

}

void i2c_fake_device_init(const uint8_t address, const bool general_call_enabled)
{
    i2c_fake_device_clear();
    reset_data_access();
    interface.address = address;
    interface.general_call_enabled = general_call_enabled;
}

void i2c_fake_device_set_mode(const i2c_fake_device_operating_modes_t mode)
{
    exposed_data.mode = mode;
}

// Not implemented yet
void i2c_fake_device_write(const uint8_t address, uint8_t * buffer, const uint8_t length, const uint8_t retries)
{
    (void) address;
    (void) buffer;
    (void) length;
    (void) retries;
}

// Not implemented yet
void i2c_fake_device_read(const uint8_t address, uint8_t * buffer, const uint8_t length, const uint8_t retries)
{
    (void) address;
    (void) buffer;
    (void) length;
    (void) retries;
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
        
        case MODE_SLAVE_TRANSMITTER:
            handle_slave_transmitting_data();
            break;
        
        case MODE_SLAVE_RECEIVER:
            handle_slave_receiving_data();
            break;
        
        case MODE_MASTER_TRANSMITTER:
            break;
        
        case MODE_MASTER_RECEIVER:
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
    // Checks if start was sent on I2C bus and consume it (reset start_sent to false)
    // If start sent, switch to MODE_SLAVE_WAIT_FOR_MASTER_ADDRESSING
    reset_data_access();
    states.previous = states.current;

    if(interface.start_sent)
    {
        interface.start_sent = false;
        states.current = MODE_SLAVE_WAIT_FOR_MASTER_ADDRESSING;
        slave_clear_flags_from_interface();
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
    
    // Check if last byte sent gave a successful response
    if (MODE_SLAVE_WAIT_FOR_MASTER_ADDRESSING != states.previous)
    {
        if (true == interface.ack_sent)
        {
            data_access.index++;
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
}

