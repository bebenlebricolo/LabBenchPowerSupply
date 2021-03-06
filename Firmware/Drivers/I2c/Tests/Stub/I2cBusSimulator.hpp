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

#ifndef I2C_BUS_SIMULATOR_HEADER
#define I2C_BUS_SIMULATOR_HEADER

#include "i2c.h"
#include "twi_hardware_stub.h"
#include "i2c_fake_device.h"

#include <cstdint>
#include <vector>

class I2cBusSimulator
{
public:
    I2cBusSimulator();

    // Main processing interface of the I2C bus
    void process(const uint8_t id);

    // Allows to register a device which is implemeting an i2c_device_interface_t within the internal collection of devices of this class
    void register_device(i2c_interface_getter_function get_interface_function,
                         i2c_process_function process_function);
    /**
     * @brief Gives the current byte being transmitted on the bus by the transmitter.
     * @return uint8_t byte
     *      0 if simulator is in Idle state or no master was detected
     *      X otherwise
     * */
    uint8_t get_current_byte_on_bus();

private:
    enum class StateMachine
    {
        Idle,
        SlaveAddressing,
        Active
    };

    /**
     * @brief depicts the current transaction mode the bus is actually in
    */
    enum class TransactionMode : uint8_t
    {
        Write       = I2C_CMD_WRITE_BIT,  /**< A write transaction is ongoing       */
        Read        = I2C_CMD_READ_BIT,   /**< A read transaction is ongoing        */
        GeneralCall,                      /**< General call (write only) was sent   */
        None,                             /**< No transaction is ongoing            */
    };

    enum class StartStopConditions : uint8_t
    {
        None,
        Start = 0x1,
        Stop = 0x2
    };

    // Collection of registered devices
    std::vector<i2c_device_stub_t> devices;

    // Used to connect a master to its slave when a transaction is ongoing
    uint8_t master_index = 0;
    // This vector is used to store all potential masters (all devices that has sent a Start condition at the exact same time)
    std::vector<uint8_t> potential_masters_indexes;
    std::vector<uint8_t> slaves_indexes;
    TransactionMode mode = TransactionMode::None;

    struct
    {
        StateMachine previous;
        StateMachine current;
    } states {StateMachine::Idle, StateMachine::Idle};

    void idle_process(const uint8_t id);
    void slave_addressing_process(const uint8_t id);
    void active_process(const uint8_t id);
    void devices_process(const uint8_t id);
    void active_devices_process(const uint8_t id);
    StartStopConditions check_bus_for_start_stop_cond();

};

#endif /* I2C_BUS_SIMULATOR_HEADER */