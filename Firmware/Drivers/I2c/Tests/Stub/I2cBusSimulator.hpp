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
    I2cBusSimulator() = default;

    // Main processing interface of the I2C bus
    void process(const uint8_t id);

    // Allows to register a device which is implemeting an i2c_device_interface_t within the internal collection of devices of this class
    void register_device(i2c_interface_getter_function get_interface_function,
                         i2c_process_function process_function);
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

    // Collection of registered devices
    std::vector<i2c_device_stub_t> devices;

    // Used to connect a master to its slave when a transaction is ongoing
    uint8_t master_index = 0;
    // This vector is used to store all potential masters (all devices that has sent a Start condition at the exact same time)
    std::vector<uint8_t> potential_masters_indexes;
    std::vector<uint8_t> slaves_indexes;
    TransactionMode mode = TransactionMode::None;
    StateMachine state_machine = StateMachine::Idle;

    void idle_process(const uint8_t id);
    void slave_addressing_process(const uint8_t id);
    void active_process(const uint8_t id);
    void devices_process(const uint8_t id);
    void active_devices_process(const uint8_t id);
};

#endif /* I2C_BUS_SIMULATOR_HEADER */