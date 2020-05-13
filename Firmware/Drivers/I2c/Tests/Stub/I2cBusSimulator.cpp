#include "I2cBusSimulator.hpp"
#include <algorithm>
#include <cstdlib>

#define GENERAL_CALL_ADDRESS (0x00)

void I2cBusSimulator::devices_process(const uint8_t id)
{
    for (auto& device : devices)
    {
        device.process(id);
    }
}


void I2cBusSimulator::idle_process(const uint8_t id)
{
    potential_masters_indexes.clear();
    slaves_indexes.clear();
    
    devices_process(id);

    // Find potential masters to be watched carefully when Slave addressing mode is entered
    for(uint8_t i = 0 ; i < devices.size() ; i++)
    {
        if (devices[i].interface->start_sent)
        {
            potential_masters_indexes.push_back(i);
        }
    }

    // Perform the transition to Slave addressing if at least one master was discovered
    if (0 != potential_masters_indexes.size())
    {
        // Let all devices know that one of them became the master of the i2c bus
        for (uint8_t index = 0 ; index < devices.size() ; index++)
        {
            devices[index].interface->start_sent = true;
            devices[index].interface->bus_busy = true;
        }
        // Switch to next state
        state_machine = I2cBusSimulator::StateMachine::SlaveAddressing;
    }
    // else, fallback to Idle mode, waiting for next change
}

void I2cBusSimulator::slave_addressing_process(const uint8_t id)
{
    uint8_t address = (devices[master_index].interface->data & 0xFE) >> 1U;
    slaves_indexes.clear();

    // Perform arbitration in case more than one potential master was found
    // This is basically an abstraction of what the competing devices actually do in real-world application
    // as this software implementation works on full bytes, sequentially whereas in real world everything happens simultaneously
    if (potential_masters_indexes.size() > 1)
    {
        master_index = potential_masters_indexes[rand() % potential_masters_indexes.size()];
        for (uint8_t i = 0 ; i < potential_masters_indexes.size(); i++)
        {
            // Signal to each device it has lost the arbitration process
            if (potential_masters_indexes[i] != master_index)
            {
                devices[potential_masters_indexes[i]].interface->lost_arbitration = true;
            }
        } 
    }

    // Only the master device needs to process its internal datas in the first place
    devices[master_index].process(id);
    
    // Handle stop conditions
    if (devices[master_index].interface->stop_sent)
    {
        // Post the stop action to all devices and go back to Idle mode
        for (auto& device : devices)
        {
            device.interface->stop_sent = true;
            device.interface->bus_busy = false;
        }
        // Revert to Idle state
        state_machine = I2cBusSimulator::StateMachine::Idle;
        return;
    }

    TransactionMode transaction_mode =  static_cast<TransactionMode>(devices[master_index].interface->data & I2C_CMD_MASK);
    
    // Check for general calls
    if (address == GENERAL_CALL_ADDRESS)
    {
        transaction_mode = TransactionMode::GeneralCall;
    }

    // List available slaves for that transaction
    for (uint8_t i = 0 ; i < devices.size() ; i++)
    {
        if (i != master_index )
        {
            if ((address == devices[i].interface->address) 
            ||  (address == GENERAL_CALL_ADDRESS  && devices[i].interface->general_call_enabled))
            {
                slaves_indexes.push_back(i);
            }
        }
    }


    // Accept the transition to next state        
    if (slaves_indexes.size() != 0)
    {
        // Execute slave's code to check if they manage to recognize their address
        bool slave_recognized = false;
        for(auto slave_index : slaves_indexes)
        {
            devices[slave_index].process(id);
            slave_recognized |= devices[slave_index].interface->ack_sent;
        }

        // If a device is found
        if(slave_recognized)
        {
            devices[master_index].interface->ack_sent = true;
        }
        
        mode = transaction_mode;
        state_machine = I2cBusSimulator::StateMachine::Active;
    }
    else
    {
        // Inform the master no slave device was found
        // and loop back in SlaveAddressing mode until getting a Stop condition from master
        devices[master_index].interface->ack_sent = I2C_ACKNOWLEDGMENT_NACK;
    }
}

void I2cBusSimulator::active_process(const uint8_t id)
{
    // Check for start condition 
    bool start_sent = devices[master_index].interface->start_sent;
    bool stop_sent = devices[master_index].interface->stop_sent;

    // TODO : further check this statement, not sure it is true ... !
    // Exclude start condition if a stop is sent at the same time (this would be a software issue only, 
    // as on physicall bus, the start condition will win against a stop condition and will always be seen as such)
    if (start_sent && stop_sent)
    {
        start_sent = false;
    }

    // Reverts back to Idle because transmission is over
    if (stop_sent)
    {
        // Transmission is over
        state_machine = StateMachine::Idle;
        for (auto& device : devices)
        {
            device.interface->stop_sent = true;
            device.interface->bus_busy = false;
        }
        return;
    }

    // Reverts back to SlaveAddressing state as this is a Repeated Start condition 
    // -> occured while the previous transmission was still ongoing
    if (start_sent)
    {
        // Transmission is over
        state_machine = StateMachine::SlaveAddressing;
        for (auto& device : devices)
        {
            device.interface->start_sent = true;
        }
        return;
    }

    uint8_t data = 0xFF;
    bool ack_sent = false;
    switch (mode)
    {
        case TransactionMode::Write :
        case TransactionMode::GeneralCall :
            // Master process time
            devices[master_index].process(id);

            // Transfer data to slave and call slave.process() function
            for (auto& slave_index : slaves_indexes)
            {   
                devices[slave_index].interface->data = devices[master_index].interface->data;
                devices[slave_index].process(id);
                ack_sent |= devices[slave_index].interface->ack_sent;
            }
            devices[master_index].interface->ack_sent = ack_sent;
            break;

        case TransactionMode::Read :
            for (auto& slave_index : slaves_indexes)
            {
                // Applies a binary & to the original data as if all slaves
                // were to 'speak' at the same time. If only one slave is registered, output data will be the same as
                // the slave's data
                devices[slave_index].process(id);
                data &= devices[slave_index].interface->data;
            }
            // Then transfer data to master and have master to process those datas and post ack or nack
            devices[master_index].interface->data = data;
            devices[master_index].process(id);
            for (auto& slave_index : slaves_indexes)
            {
                devices[slave_index].interface->ack_sent = devices[master_index].interface->ack_sent;
            }
            break;
        default:
            break;
    }
}



void I2cBusSimulator::process(const uint8_t id)
{
    switch(state_machine)
    {
    case I2cBusSimulator::StateMachine::Idle :
        // loop on all devices and check if a start condition was thrown by someone
        // In case where only one device sent a start, it becomes the master
        // otherwise, all devices which has sent a Start condition will be stored as "potential master" until
        // arbitration is done when writing slave addresses + direction bit
        // Note : this is handled by hardware and arbitration can only occur at this time (Start condition being a SDA low, it pulls the whole SDA line to low, so there is no way
        // to identify at this time)
        
        // When master is found, set the master_index with the device which won the fight
        // Then go to SlaveAddressing on next call -> set state_machine to SlaveAddressing mode
        
        idle_process(id);
        break;
    case I2cBusSimulator::StateMachine::SlaveAddressing :
        // loop on potential slave devices (then excluding the master device) and check for slave address
        // recognition : check the current master byte of data and look for a matching address
        // if a slave responds to this address, set the slave_index to the found one
        // if no slave responds, write a NACK to master interface (master shall send a Stop condition on next loop)
        // If both master and slave are identified, note the transaction nature (read or write) and proceed to Active mode
        slave_addressing_process(id);
        break;
    case I2cBusSimulator::StateMachine::Active :
        // Transmission is ongoing, check for start condition and stop condition
        // if stop condition is sent, revert state back to idle and clear master and slave indexes
        // if start condition is sent by master device (repeated start), revert state back to SlaveAddressing mode
        // otherwise :
        //      depending on the transaction nature, transfer data from master to slave and vice-versa
        active_process(id);
        break;
    default:
        break;
    }
}


void I2cBusSimulator::register_device(i2c_interface_getter_function get_interface_function,
                                      i2c_process_function process_function)
{
    auto found_item = std::find_if(devices.begin(), devices.end(), [get_interface_function, process_function](i2c_device_stub_t &device)
    {
        return device.process == process_function;
    });

    // Only adds a new device if not already present on the bus
    if (found_item == devices.end())
    {
        i2c_device_interface_t * interface;
        get_interface_function(0U, &interface);
        i2c_device_stub_t device;
        device.interface = interface;
        device.process = process_function;
        devices.push_back(device);
    }
}