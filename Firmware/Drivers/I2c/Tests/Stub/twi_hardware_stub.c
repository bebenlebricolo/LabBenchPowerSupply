#include "twi_hardware_stub.h"
#include "i2c_register_stub.h"
#include "i2c.h"

typedef enum
{
    INTERNAL_STATE_IDLE = 0,
    
    INTERNAL_STATE_MASTER_TO_SLAVE_ADDRESSING, /**< When master device sends the address + command byte over I2C bus before starting an operation    */
    INTERNAL_STATE_MASTER_TX,                  /**< Device is put in master state, writing mode (transmits data) and will check for ack at next loop */
    INTERNAL_STATE_MASTER_RX,                  /**< Device is put in master state, receiving mode and responds back with ack/nack on its interface   */
    
    INTERNAL_STATE_SLAVE_WAIT_MASTER_ADDRESSING, /**< When device receives a start condition while in Idle state, this mode let the device listen for its address on bus           */
    INTERNAL_STATE_SLAVE_TX,                   /**< Device is addressed as a slave, in transmitter mode (slave sends data over I2C bus : this is a read command from the master  */
    INTERNAL_STATE_SLAVE_RX,                   /**< Device is addressed as a slave, in receiver mode (slave receives data over I2C bus : this is a write command from the master */
} internal_state_t;

static struct 
{
    internal_state_t current;
    internal_state_t previous;
} states[I2C_DEVICES_COUNT] = {0};

static i2c_device_interface_t interface[I2C_DEVICES_COUNT] = {0};

static void master_update_interface_from_regs(const uint8_t id);
static void master_clear_flags_from_reg(const uint8_t id);

static void handle_idle(const uint8_t id);
static void handle_master_to_slave_addressing(const uint8_t id);
static void handle_master_tx(const uint8_t id);
static void handle_master_rx(const uint8_t id);

static void master_update_interface_from_regs(const uint8_t id)
{
    i2c_register_stub_t * stub = &i2c_register_stub[id];
    i2c_device_interface_t * interface = &interface[id];
    interface->data = stub->twdr_reg;
    interface->start_sent = (bool) stub->twcr_reg & TWSTA_MSK;
    interface->stop_sent = (bool) stub->twcr_reg & TWSTO_MSK;
}

static void master_clear_flags_from_reg(const uint8_t id)
{
    i2c_register_stub[id].twcr_reg &= ~(TWSTA_MSK | TWSTO_MSK | TWINT_MSK);
}

static void handle_master_tx(const uint8_t id)
{
    // Check if last I2C command succeeded
    bool received_ack = interface[id].ack_sent;
    
    // If we were addressing a slave right before 
    if (INTERNAL_STATE_MASTER_TO_SLAVE_ADDRESSING == states[id].previous)
    {
        // Handle arbitration lost which can only occur (in this implementation) when
        // master writes slave's address on I2C bus
        if (interface[id].lost_arbitration)
        {
            i2c_register_stub[id].twsr_reg = MAS_TX_ARBITRATION_LOST;
            states[id].current = INTERNAL_STATE_SLAVE_WAIT_MASTER_ADDRESSING;
            states[id].previous = INTERNAL_STATE_SLAVE_WAIT_MASTER_ADDRESSING;
        }
        else
        {
            // Slave recognized itself and told the master device it is ready for next step
            if (received_ack)
            {
                i2c_register_stub[id].twsr_reg = MAS_TX_SLAVE_WRITE_ACK;
            }
            else
            {
                i2c_register_stub[id].twsr_reg = MAS_TX_SLAVE_WRITE_NACK;
            }
        }
    }
    else
    {
        uint8_t flags = 0;
        flags |= (interface[id].start_sent ? 1 : 0);
        flags |= (interface[id].stop_sent ? 1 : 0) << 1U;

        switch(flags)        
        {
            case 0x00 :
                // Last operation succeeded (data has been transmitted)
                if (received_ack)
                {
                    i2c_register_stub[id].twsr_reg = MAS_TX_DATA_TRANSMITTED_ACK;
                }
                else
                {
                    i2c_register_stub[id].twsr_reg = MAS_TX_DATA_TRANSMITTED_NACK;
                }
                break;
            
            // Start sent, switch to REPEATED START
            case 0x01:
                i2c_register_stub[id].twsr_reg = MAS_TX_REPEATED_START;
                break;
            
            // Stop sent
            case 0x02:
                // Clear status register (?)
                i2c_register_stub[id].twsr_reg = 0;
                break;
            
            // A Stop will be followed by a start action
            case 0x03:
                i2c_register_stub[id].twsr_reg = 0;
                states[id].current = INTERNAL_STATE_IDLE;
                states[id].previous = INTERNAL_STATE_IDLE;
                break;
        }
    }

    // Clear internal flags to prevent side-effect when running the driver's process()
    // Clears flags as if real hardware has done it
    master_clear_flags_from_reg(id);
    (void) i2c_process(id);

    // Transfers data from registers to interface
    master_update_interface_from_regs(id);

    // Reset ack received flag
    interface[id].ack_sent = false;
    states[id].previous = states[id].current;
}

static void handle_idle(const uint8_t id)
{
    // Addressed by another device before this one had a chance to post it start action 
    // to its I2C interface -> outside start action was taken at the previous round!
    if (interface[id].start_sent)
    {
        states[id].current = INTERNAL_STATE_SLAVE_WAIT_MASTER_ADDRESSING;
        // driver has nothing to do until being addressed by another master, no need to run process() then
    }
    else if(0 != (i2c_register_stub[id].twcr_reg & TWSTA_MSK))
    {
        master_update_interface_from_regs(id);
        states[id].current = INTERNAL_STATE_MASTER_TO_SLAVE_ADDRESSING;
        i2c_register_stub[id].twsr_reg = MAS_TX_START_TRANSMITTED;
    }
    else
    {
        // Do nothing, loop back in same state for now
    }
}

static void handle_master_to_slave_addressing(const uint8_t id)
{
    // Driver now should engage slave addressing logic
    master_clear_flags_from_reg(id);
    i2c_process(id);

    // Reset interface flags (never handled by twi hardware at this moment)
    interface[id].start_sent = false;
    interface[id].stop_sent = false;
    master_update_interface_from_regs(id);
    states[id].previous = states[id].current;
    states[id].current = INTERNAL_STATE_MASTER_TX;
}


void twi_hardware_stub_process(const uint8_t id)
{
    /* Clear twint flag as if the real hardware has done it */
    i2c_register_stub[id].twcr_reg &= ~TWINT_MSK;

    switch(states[id].current)
    {
        case INTERNAL_STATE_IDLE:
            /* - Check for start flag in registers, if start is there, writes this to its i2C interface and switch to 
                    -> INTERNAL_STATE_MASTER_TO_SLAVE_ADDRESSING
                    -> set MAS_TX_START_TRANSMITTED status code within register
                 If not in registers and start comes from the interface instead, this device shall be put in slave addressing in waiting mode
                    -> INTERNAL_STATE_SLAVE_TO_MASTER_ADDRESSING
                    -> No status code available, device is still in unaddressed mode until its own address is detected
               - Otherwise, fallback to this same INTERNAL_STATE_IDLE */
            break;

        case INTERNAL_STATE_MASTER_TO_SLAVE_ADDRESSING:
            // Clear start flag !
            /*  Load slave adress + command type to interface (shall already be present in data register by now) 
                and post it to device interface. Transition to INTERNAL_STATE_MASTER_TX */
            states[id].previous = states[id].current;
            states[id].current = INTERNAL_STATE_MASTER_TX;
            break;

        case INTERNAL_STATE_MASTER_TX:
            /* Check ack/nack state.

                ~~ Handle slave addressing stuff ~~ 
                if (previous state was INTERNAL_STATE_MASTER_TO_SLAVE_ADDRESSING)
                    if (NACK  received)
                        status_code = MAS_TX_SLAVE_WRITE_NACK
                    else
                        status_code = MAS_TX_SLAVE_WRITE_ACK
                    
                    check registers and select from :
                            * proceed and load data from registers
                            * send a repeated start
                            * send a stop
                            * send a stop/start
                
                ~~ Handle data write stuff ~~
                else (previous state was INTERNAL_STATE_MASTER_TX)
                    if (NACK received)
                        status_code 
               If previous status code was MAS_TX_START_TRANSMITTED :
                if ack was received, switch status code to MAS_TX_SLAVE_WRITE_ACK
                 + load data to interface
                else set it to MAS_TX_SLAVE_WRITE_NACK
                 + switch back to 
               Otherwise, 
             */
            handle_master_tx(id);
            break;

        case INTERNAL_STATE_MASTER_RX:
            break;

        case INTERNAL_STATE_SLAVE_TX:
            break;

        case INTERNAL_STATE_SLAVE_RX:
            break;
        default:
            break;
    }

}

void twi_hardware_stub_get_interface(const uint8_t bus_id, i2c_device_interface_t ** const p_interface)
{
    *p_interface = &interface[bus_id];
}
