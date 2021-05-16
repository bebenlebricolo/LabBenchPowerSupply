#include "I2cBusSimulator.hpp"
#include <algorithm>
#include <cstdlib>

#define GENERAL_CALL_ADDRESS (0x00)

I2cBusSimulator::I2cBusSimulator()
{

}


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

    // Propagate start condition to all devices
    if (0 != potential_masters_indexes.size())
    {
        // Let all non-master devices know that one of them became the master of the i2c bus
        for(uint8_t i = 0 ; i < devices.size(); i++)
        {
            auto found_item = std::find(potential_masters_indexes.begin(), potential_masters_indexes.end(), i);
            if (found_item == potential_masters_indexes.end())
            {
                // Reprocess target device as the slaves has to be aware a transaction has started
                devices[i].interface->start_sent = true;
                devices[i].process(id);
            }
        }
        states.current = I2cBusSimulator::StateMachine::SlaveAddressing;

    }
    // else, fallback to Idle mode, waiting for next change
}

void I2cBusSimulator::slave_addressing_process(const uint8_t id)
{
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
                devices[potential_masters_indexes[i]].process(id);
            }
        }
    }
    else
    {
        master_index = potential_masters_indexes[0];
    }

    // Only the master device needs to process its internal datas in the first place
    devices[master_index].process(id);
    uint8_t address = (devices[master_index].interface->data & 0xFE) >> 1U;

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
        states.previous = states.current;
        states.current = I2cBusSimulator::StateMachine::Idle;
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
            devices[slave_index].interface->data = devices[master_index].interface->data;
            devices[slave_index].process(id);
            slave_recognized |= devices[slave_index].interface->ack_sent;
        }

        // If a device is found
        if(slave_recognized)
        {
            devices[master_index].interface->ack_sent = true;
        }

        mode = transaction_mode;
        states.previous = states.current;
        states.current = I2cBusSimulator::StateMachine::Active;
    }
    else
    {
        // Inform the master no slave device was found
        // and loop back in SlaveAddressing mode until getting a Stop condition from master
        devices[master_index].interface->ack_sent = false;
    }
}


 I2cBusSimulator::StartStopConditions I2cBusSimulator::check_bus_for_start_stop_cond()
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
        states.previous = states.current;
        states.current = StateMachine::Idle;
        for (auto& device : devices)
        {
            device.interface->stop_sent = true;
            device.interface->bus_busy = false;
        }
        return StartStopConditions::Stop;
    }

    // Reverts back to SlaveAddressing state as this is a Repeated Start condition
    // -> occured while the previous transmission was still ongoing
    if (start_sent)
    {
        // Transmission is over
        states.previous = states.current;
        states.current = StateMachine::SlaveAddressing;
        for (auto& device : devices)
        {
            device.interface->start_sent = true;
        }
        return StartStopConditions::Start;
    }

    return StartStopConditions::None;
 }


void I2cBusSimulator::active_process(const uint8_t id)
{

    StartStopConditions bus_conditions = StartStopConditions::None;
    uint8_t data = 0xFF;
    bool ack_sent = false;
    switch (mode)
    {
        case TransactionMode::Write :
        case TransactionMode::GeneralCall :
            // Master process time
            devices[master_index].process(id);
            bus_conditions = check_bus_for_start_stop_cond();

            // Handle stop condition when master decides to break execution
            if (StartStopConditions::Stop == bus_conditions)
            {
                for (auto& device : devices)
                {
                    device.interface->stop_sent = true;
                }
                states.previous = states.current;
                states.current = StateMachine::Idle;
            }
            else if(StartStopConditions::Start == bus_conditions)
            {
                for (auto& device : devices)
                {
                    device.interface->start_sent = true;
                }
                states.previous = states.current;
                states.current = StateMachine::SlaveAddressing;
            }

            // Transfer data to slave and call slave.process() function
            for (auto& slave_index : slaves_indexes)
            {
                if (StartStopConditions::None == bus_conditions)
                {
                    devices[slave_index].interface->data = devices[master_index].interface->data;
                }
                devices[slave_index].process(id);
                ack_sent |= devices[slave_index].interface->ack_sent;
            }
            devices[master_index].interface->ack_sent = ack_sent;

            break;

        case TransactionMode::Read :
            if (StateMachine::SlaveAddressing == states.previous)
            {
                // We need the master to consume the slave's acknowledgment before starting the transmission to update its internal states
                devices[master_index].process(id);
                states.previous = states.current;
            }

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

            bus_conditions = check_bus_for_start_stop_cond();

            // Handle stop condition when master decides to break execution
            if (StartStopConditions::Stop == bus_conditions)
            {
                for (auto& device : devices)
                {
                    device.interface->stop_sent = true;
                }
                states.previous = states.current;
                states.current = StateMachine::Idle;
            }
            else if(StartStopConditions::Start == bus_conditions)
            {
                for (auto& device : devices)
                {
                    device.interface->start_sent = true;
                }
                states.previous = states.current;
                states.current = StateMachine::SlaveAddressing;
            }

            if(StartStopConditions::None != bus_conditions)
            {
                // Reprocess all devices except master for them to take into account new bus state (either start or stop)
                for (auto device : devices)
                {
                    device.process(id);
                }
            }

            break;
        default:
            break;
    }

}



void I2cBusSimulator::process(const uint8_t id)
{
    switch(states.current)
    {
    case I2cBusSimulator::StateMachine::Idle :
        // loop on all devices and check if a start condition was thrown by someone
        // In case where only one device sent a start, it becomes the master
        // otherwise, all devices which has sent a Start condition will be stored as "potential master" until
        // arbitration is done when writing slave addresses + direction bit
        // Note : this is handled by hardware and arbitration can only occur at this time (Start condition being a SDA low, it pulls the whole SDA line to low, so there is no way
        // to identify at this time)

        // When master is found, set the master_index with the device which won the fight
        // Then go to SlaveAddressing on next call -> set states.current to SlaveAddressing mode

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


uint8_t I2cBusSimulator::get_current_byte_on_bus()
{
    if( I2cBusSimulator::StateMachine::Active == states.current)
    {
        if( TransactionMode::Read == mode)
        {
            uint8_t data = 0xFF;
            if (0 != slaves_indexes.size())
            {
                for( auto slave : slaves_indexes)
                {
                    data &= devices[slave].interface->data;
                }
                return data;
            }
            else
            {
                return 0;
            }
        }
        else if(TransactionMode::Write == mode || TransactionMode::GeneralCall == mode)
        {
            return devices[master_index].interface->data;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}