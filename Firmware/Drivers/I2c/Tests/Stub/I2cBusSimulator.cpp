#include "I2cBusSimulator.hpp"
#include <algorithm>
#include <cstdlib>

void I2cBusSimulator::devices_process(const uint8_t id)
{
    for (auto& device : devices)
    {
        device.process(id);
    }
}

void I2cBusSimulator::active_devices_process(const uint8_t id)
{
    devices[master_index].process(id);
    for (auto& slave_index : slaves_indexes)
    {
        devices[slave_index].process(id);
    }
}


void I2cBusSimulator::idle_process(const uint8_t id)
{
    std::vector<uint8_t> indexes;
    for(uint8_t i = 0 ; i < devices.size() ; i++)
    {
        if (devices[i].interface->start_sent)
        {
            indexes.push_back(i);
        }
    }

    // Arbitration in case more than one master found
    if (indexes.size() != 0)
    {
        if (indexes.size() > 1)
        {
            master_index = indexes[rand() % indexes.size()];
            for (uint8_t i = 0 ; i < indexes.size() ; i++)
            {
                if (indexes[i] != master_index)
                {
                    // Let the other devices that they have lost the arbitration process and might
                    // switch back to their undaddressed slave mode
                    devices[indexes[i]].interface->lost_arbitration = true;
                }
            }
        }
        else
        {
            master_index = indexes[0];
        }

        // Let all devices know that one of them became the master of the i2c bus
        for (auto& device : devices)
        {
            device.interface->start_sent = true;
        }

        // Switch to next state
        state_machine = I2cBusSimulator::StateMachine::SlaveAddressing;
    }

    // Finally, call process() function for each registered device
    devices_process(id);
}

void I2cBusSimulator::slave_addressing_process(const uint8_t id)
{
    uint8_t address = (devices[master_index].interface->data & 0xFE) >> 1U;
    TransactionMode transaction_mode =  static_cast<TransactionMode>(devices[master_index].interface->data & 0x01);
    slaves_indexes.clear();

    // Handle stop conditions
    if (devices[master_index].interface->stop_sent)
    {
        // Post the stop action to all devices and go back to Idle mode
        for (auto& device : devices)
        {
            device.interface->stop_sent = true;
        }
        // Revert to Idle state
        state_machine = I2cBusSimulator::StateMachine::Idle;
        devices_process(id);
        return;
    }

    // Check for general calls
    if (address == 0x00)
    {
        transaction_mode = TransactionMode::GeneralCall;
    }

    // List available slaves for that transaction
    for (uint8_t i = 0 ; i < devices.size() ; i++)
    {
        if (i != master_index )
        {
            if ((address == devices[i].interface->address) 
            ||  (address == 0x00  && devices[i].interface->general_call_enabled))
            {
                slaves_indexes.push_back(i);
            }
        }
    }

    // Accept the transistion to next state        
    if (slaves_indexes.size() != 0)
    {
        mode = transaction_mode;
        state_machine = I2cBusSimulator::StateMachine::Active;
    }
    else
    {
        // Inform the master no slave device was found
        // and loop back in SlaveAddressing mode until getting a Stop condition from master
        devices[master_index].interface->ack_sent = I2C_ACKNOWLEDGMENT_NACK;
    }
    
    // Trigger process() function for each active device
    active_devices_process(id);
}

void I2cBusSimulator::active_process(const uint8_t id)
{
    // Check for start condition 
    bool start_sent = devices[master_index].interface->start_sent;
    bool stop_sent = devices[master_index].interface->stop_sent;

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
        }
        devices_process(id);
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
        devices_process(id);
        return;
    }

    uint8_t data = 0xFF;
    switch (mode)
    {
        case TransactionMode::Write :
        case TransactionMode::GeneralCall :
            for (auto& slave_index : slaves_indexes)
            {
                devices[slave_index].interface->data = devices[master_index].interface->data;
            }
            break;

        case TransactionMode::Read :
            for (auto& slave_index : slaves_indexes)
            {
                // Applies a binary & to the original data as if all slaves
                // were to 'speak' at the same time. If only one slave is registered, output data will be the same as
                // the slave's data
                data &= devices[slave_index].interface->data;
            }
            devices[master_index].interface->data = data;
            break;
        default:
            break;
    }
    active_devices_process(id);
}



void I2cBusSimulator::process(const uint8_t id)
{
    switch(state_machine)
    {
    case I2cBusSimulator::StateMachine::Idle :
        // loop on all devices and check if a start condition was thrown by someone
        // In case where only one device sent a start, it becomes the master
        // otherwise, arbitration should be done, only one master shall remain !
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
        get_interface_function(&interface);
        i2c_device_stub_t device;
        device.interface = interface;
        device.process = process_function;
        devices.push_back(device);
    }
}