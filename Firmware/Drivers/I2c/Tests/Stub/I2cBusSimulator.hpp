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
    enum class TransactionMode
    {
        None,   /**< No transaction is ongoing      */
        Write,  /**< A write transaction is ongoing */
        Read,   /**< A read transaction is ongoing  */
    };

    // Collection of registered devices
    std::vector<i2c_device_stub_t> devices;

    // Used to connect a master to its slave when a transaction is ongoing
    uint8_t master_index;
    uint8_t slave_index;
    TransactionMode mode;
    StateMachine state_machine = StateMachine::Idle;
};

#endif /* I2C_BUS_SIMULATOR_HEADER */