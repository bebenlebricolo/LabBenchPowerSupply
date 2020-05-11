#ifndef I2C_BUS_SIMULATOR_HEADER
#define I2C_BUS_SIMULATOR_HEADER

#include <cstdint>
#include "i2c.h"
#include "twi_hardware_stub.h"
#include "i2c_fake_device.h"

struct I2cBusSimulator
{
    enum class TransactionMode
    {
        write,
        read
    };
    I2cBusSimulator();
    uint8_t * master_data;
    uint8_t * slave_data;
    TransactionMode mode;
    void process(const uint8_t id)
    {
        i2c_process(id);
    }
};

#endif /* I2C_BUS_SIMULATOR_HEADER */