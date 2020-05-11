#include "I2cBusSimulator.hpp"
#include <algorithm>



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
        break;
    case I2cBusSimulator::StateMachine::SlaveAddressing :
        // loop on potential slave devices (then excluding the master device) and check for slave address
        // recognition : check the current master byte of data and look for a matching address
        // if a slave responds to this address, set the slave_index to the found one
        // if no slave responds, write a NACK to master interface (master shall send a Stop condition on next loop)
        // If both master and slave are identified, note the transaction nature (read or write) and proceed to Active mode
        break;
    case I2cBusSimulator::StateMachine::Active :
        // Transmission is ongoing, check for start condition and stop condition
        // if stop condition is sent, revert state back to idle and clear master and slave indexes
        // if start condition is sent by master device (repeated start), revert state back to SlaveAddressing mode
        // otherwise :
        //      depending on the transaction nature, transfer data from master to slave and vice-versa
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