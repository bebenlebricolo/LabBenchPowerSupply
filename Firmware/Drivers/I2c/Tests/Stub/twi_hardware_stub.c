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

#include "twi_hardware_stub.h"
#include "i2c_register_stub.h"
#include "i2c.h"
#include "string.h"

/* ##############################################################################################  */
/* ##############################################################################################  */
/* ##############################################################################################  */

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

/* ##############################################################################################  */
/* ##############################################################################################  */
/* ##############################################################################################  */



static inline void write_status_code_to_reg(const uint8_t id, uint8_t status_code);

static void master_update_interface_from_regs(const uint8_t id);
static void master_clear_flags_from_reg(const uint8_t id);

static void handle_idle(const uint8_t id);
static void handle_master_to_slave_addressing(const uint8_t id);
static void handle_master_tx(const uint8_t id);
static void handle_master_rx(const uint8_t id);
static void handle_master_start_stop(const uint8_t id);

static void handle_slave_wait_master_addressing(const uint8_t id);
static void handle_slave_tx(const uint8_t id);
static void handle_slave_rx(const uint8_t id);
static bool handle_slave_start_stop(const uint8_t id);
static void set_twint(const uint8_t id);

static void master_update_interface_from_regs(const uint8_t id)
{
    interface[id].data = i2c_register_stub[id].twdr_reg;
    interface[id].start_sent = (bool) (i2c_register_stub[id].twcr_reg & TWSTA_MSK);
    interface[id].stop_sent = (bool) (i2c_register_stub[id].twcr_reg & TWSTO_MSK);
}

static void slave_update_interface_from_regs(const uint8_t id)
{
    interface[id].ack_sent = (bool) (i2c_register_stub[id].twcr_reg & TWEA_MSK);
}

static void master_clear_flags_from_reg(const uint8_t id)
{
    i2c_register_stub[id].twcr_reg &= ~(TWSTA_MSK | TWSTO_MSK );
    i2c_register_stub[id].twcr_reg |= TWINT_MSK;
}

static inline void write_status_code_to_reg(const uint8_t id, uint8_t status_code)
{
    i2c_register_stub[id].twsr_reg &= ~TWS_MSK;
    i2c_register_stub[id].twsr_reg |= status_code;
}

static inline uint8_t get_status_code_from_reg(const uint8_t id)
{
    return i2c_register_stub[id].twsr_reg & TWS_MSK;
}


void twi_hardware_stub_clear(void)
{
    for (uint8_t i = 0 ;  i < I2C_DEVICES_COUNT ; i++)
    {
        states[i].previous = INTERNAL_STATE_IDLE;
        states[i].current = INTERNAL_STATE_IDLE;
        write_status_code_to_reg(i, I2C_MISC_NO_RELEVANT_STATE);
        memset(&interface[i], 0, sizeof(i2c_device_interface_t));
    }
}

static void set_twint(const uint8_t id)
{
    i2c_register_stub[id].twcr_reg |= TWINT_MSK;
}


static void handle_master_start_stop(const uint8_t id)
{
    // Handle process results published on interface
    uint8_t flags = 0;
    flags |= (interface[id].start_sent ? 1 : 0);
    flags |= (interface[id].stop_sent ? 1 : 0) << 1U;
    states[id].previous = states[id].current;

    switch(flags)
    {

        // Start sent, switch to REPEATED START
        case 0x01:
            write_status_code_to_reg(id, MAS_TX_REPEATED_START);
            states[id].current = INTERNAL_STATE_MASTER_TO_SLAVE_ADDRESSING;
            break;

        // Stop sent
        case 0x02:
            // Clearing status register (setting it to 0) is a bad idea as
            // the status code 0x0 states for "I2C_MISC_BUS_ERROR_ILLEGAL_START_STOP"
            // which is a hardware error in itself and buffer might be cleared.
            // The other solution is to set the hardware status registers in their "idling state"
            // which depicts hardware has nothing to process for now.
            i2c_register_stub[id].twsr_reg = I2C_MISC_NO_RELEVANT_STATE;
            states[id].current = INTERNAL_STATE_IDLE;
            states[id].previous = INTERNAL_STATE_IDLE;
            break;

        // A Stop will be followed by a start action
        case 0x03:
            // Same remark as above
            i2c_register_stub[id].twsr_reg = I2C_MISC_NO_RELEVANT_STATE;
            states[id].current = INTERNAL_STATE_IDLE;
            states[id].previous = INTERNAL_STATE_IDLE;
            break;

        // Nothing to do
        default:
        case 0x00 :
            break;
    }
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
                write_status_code_to_reg(id, MAS_TX_SLAVE_WRITE_ACK);
                states[id].previous = states[id].current;
            }
            else
            {
                write_status_code_to_reg(id, MAS_TX_SLAVE_WRITE_NACK);
            }
        }
    }
    else
    {
        if (received_ack)
        {
            write_status_code_to_reg(id, MAS_TX_DATA_TRANSMITTED_ACK);
        }
        else
        {
            write_status_code_to_reg(id, MAS_TX_DATA_TRANSMITTED_NACK);
        }
    }

    // Clear internal flags to prevent side-effect when running the driver's process()
    // Clears flags as if real hardware has done it
    master_clear_flags_from_reg(id);
    (void) i2c_process(id);

    // Transfers data from registers to interface
    master_update_interface_from_regs(id);

    handle_master_start_stop(id);

    // Reset ack received flag
    interface[id].ack_sent = false;
    set_twint(id);
}

static void handle_master_rx(const uint8_t id)
{
    i2c_register_stub[id].twdr_reg = interface[id].data;

    // First time this function is called is when we have just received an acknoledgment over slave addressing
    if (INTERNAL_STATE_MASTER_TO_SLAVE_ADDRESSING == states[id].previous)
    {
        if (interface[id].ack_sent)
        {
            write_status_code_to_reg(id, MAS_RX_SLAVE_READ_ACK);
        }
        else
        {
            write_status_code_to_reg(id, MAS_RX_SLAVE_READ_NACK);
        }
        states[id].previous = states[id].current;
        interface[id].ack_sent = false;
    }
    else
    {
        // Force accept incoming data bytes
        interface[id].ack_sent = true;
        write_status_code_to_reg(id, MAS_RX_DATA_RECEIVED_ACK);
    }

    handle_master_start_stop(id);

    // Then, we need to process what's coming using the i2c interface
    (void) i2c_process(id);
    master_update_interface_from_regs(id);

    if (false == interface[id].start_sent && false == interface[id].stop_sent)
    {
        interface[id].ack_sent = (bool) (i2c_register_stub[id].twcr_reg & TWEA_MSK);
    }

    set_twint(id);
}


static void handle_idle(const uint8_t id)
{
    // Addressed by another device before this one had a chance to post it start action
    // to its I2C interface -> outside start action was taken at the previous round!
    if (interface[id].start_sent)
    {
        states[id].current = INTERNAL_STATE_SLAVE_WAIT_MASTER_ADDRESSING;
        interface[id].address = (i2c_register_stub[id].twar_reg & 0xFE) >> 1U;
        interface[id].start_sent = false;
        interface[id].stop_sent = false;
        // driver has nothing to do until being addressed by another master, no need to run process() then
    }
    else if(0 != (i2c_register_stub[id].twcr_reg & TWSTA_MSK))
    {
        master_update_interface_from_regs(id);
        states[id].current = INTERNAL_STATE_MASTER_TO_SLAVE_ADDRESSING;
        write_status_code_to_reg(id, MAS_TX_START_TRANSMITTED);
    }
    else
    {
        // Do nothing, loop back in same state for now
        i2c_state_t state;
        (void) i2c_get_state(id, &state);
        if(I2C_STATE_MASTER_TX_FINISHED == state || I2C_STATE_MASTER_RX_FINISHED == state)
        {
            // Reprocess the device to fall back in Idle mode
            (void) i2c_process(id);
        }
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
    if (I2C_CMD_READ_BIT == (interface[id].data & 0x01))
    {
        states[id].current = INTERNAL_STATE_MASTER_RX;
    }
    else
    {
        states[id].current = INTERNAL_STATE_MASTER_TX;
    }
    set_twint(id);

}


void twi_hardware_stub_process(const uint8_t id)
{
    switch(states[id].current)
    {
        case INTERNAL_STATE_IDLE:
            handle_idle(id);
            break;

        case INTERNAL_STATE_MASTER_TO_SLAVE_ADDRESSING:
            handle_master_to_slave_addressing(id);
            break;

        case INTERNAL_STATE_SLAVE_WAIT_MASTER_ADDRESSING :
            handle_slave_wait_master_addressing(id);
            break;

        case INTERNAL_STATE_MASTER_TX:
            handle_master_tx(id);
            break;

        case INTERNAL_STATE_MASTER_RX:
            handle_master_rx(id);
            break;

        case INTERNAL_STATE_SLAVE_TX:
            handle_slave_tx(id);
            break;

        case INTERNAL_STATE_SLAVE_RX:
            handle_slave_rx(id);
            break;

        default:
            break;
    }

}

void twi_hardware_stub_get_interface(const uint8_t bus_id, i2c_device_interface_t ** const p_interface)
{
    *p_interface = &interface[bus_id];
}

bool twi_hardware_stub_is_busy(const uint8_t id)
{
    return (INTERNAL_STATE_IDLE != states[id].current);
}

static void handle_slave_wait_master_addressing(const uint8_t id)
{
    states[id].previous = states[id].current;
    // Is this device being addressed ?
    if (interface[id].address == ((interface[id].data & 0xFE) >> 1))
    {
        if (I2C_CMD_READ_BIT == (interface[id].data & 0x01))
        {
            states[id].current = INTERNAL_STATE_SLAVE_TX;
            write_status_code_to_reg(id, SLA_TX_OWN_ADDR_SLAVE_READ_ACK);
        }
        else
        {
            states[id].current = INTERNAL_STATE_SLAVE_RX;
            write_status_code_to_reg(id, SLA_RX_PREV_ADDRESSED_DATA_RECEIVED_ACK);
        }
        interface[id].ack_sent = true;
        set_twint(id);
    }
    else
    {
        // General call ?
        if((interface[id].general_call_enabled )
        && (0x00 == (interface[id].data & 0xFE)))
        {
            // Read command using General call feature is illegal, reset to idle state
            if (I2C_CMD_READ_BIT == (interface[id].data & 0x01))
            {
                states[id].current = INTERNAL_STATE_IDLE;
                interface[id].ack_sent = false;
            }
            else
            {
                states[id].current = INTERNAL_STATE_SLAVE_RX;
                write_status_code_to_reg(id, SLA_RX_GENERAL_CALL_RECEIVED_ACK);
                interface[id].ack_sent = true;
            }
        }
        else
        {
            states[id].current = INTERNAL_STATE_IDLE;
        }
    }
}

void handle_slave_tx(const uint8_t id)
{
    // Check start stop conditions
    bool break_execution = handle_slave_start_stop(id);

    if (!break_execution)
    {
        // If we were listening a master before
        if (INTERNAL_STATE_SLAVE_WAIT_MASTER_ADDRESSING == states[id].previous)
        {
            states[id].previous = states[id].current;
            interface[id].data = i2c_register_stub[id].twdr_reg;
            states[id].previous = states[id].current;

        }
        else
        {
            // Check if last I2C command succeeded
            if (true == interface[id].ack_sent)
            {
                write_status_code_to_reg(id, SLA_TX_DATA_TRANSMITTED_ACK);
            }
            else
            {
                write_status_code_to_reg(id, SLA_TX_DATA_TRANSMITTED_NACK);
            }
        }
    }

    // Clear internal flags to prevent side-effect when running the driver's process()
    // Clears flags as if real hardware has done it
    (void) i2c_process(id);

    interface[id].data = i2c_register_stub[id].twdr_reg;

    // Reset ack received flag
    interface[id].ack_sent = false;
    set_twint(id);
}

void handle_slave_rx(const uint8_t id)
{
    bool break_execution = handle_slave_start_stop(id);
    states[id].previous = states[id].current;

    if (break_execution)
    {
        write_status_code_to_reg(id, SLA_RX_START_STOP_COND_RECEIVED_WHILE_OPERATING);
    }

    // Handle incoming data
    i2c_register_stub[id].twdr_reg = interface[id].data;
    i2c_process(id);
    slave_update_interface_from_regs(id);

    if (!break_execution)
    {
        if (interface[id].ack_sent)
        {
            write_status_code_to_reg(id, SLA_RX_PREV_ADDRESSED_DATA_RECEIVED_ACK);
        }
        else
        {
            write_status_code_to_reg(id, SLA_RX_PREV_ADDRESSED_DATA_LOST_NACK);
        }
    }

    set_twint(id);
}

static bool handle_slave_start_stop(const uint8_t id)
{
    bool break_needed = true;
    // Handle process results published on interface
    uint8_t flags = 0;
    flags |= (interface[id].start_sent ? 1 : 0);
    flags |= (interface[id].stop_sent ? 1 : 0) << 1U;

    switch(flags)
    {

        // Start sent, switch to REPEATED START
        case 0x01:
            states[id].current = INTERNAL_STATE_SLAVE_WAIT_MASTER_ADDRESSING;
            break;

        // Stop sent
        case 0x02:
            // Clear status register (?)
            i2c_register_stub[id].twsr_reg = 0;
            states[id].current = INTERNAL_STATE_IDLE;
            states[id].previous = INTERNAL_STATE_IDLE;
            break;

        // A Stop will be followed by a start action
        case 0x03:
            i2c_register_stub[id].twsr_reg = 0;
            states[id].current = INTERNAL_STATE_IDLE;
            states[id].previous = INTERNAL_STATE_IDLE;
            break;

        // Nothing to do
        default:
        case 0x00 :
            break_needed = false;
            break;
    }

    // Clear flags, as if the twi hardware had consumed them
    interface[id].start_sent = false;
    interface[id].stop_sent = false;
    return break_needed;
}