#include "gtest/gtest.h"
#include "i2c.h"
#include "test_isr_stub.h"

/*
class I2cTestFixture : public ::testing::Test
{
public:
    i2c_config_t config;
protected:
    void SetUp() override
    {
    }
    void TearDown() override
    {
        i2c_deinit(0U);
    }
};
*/

TEST(i2c_driver_test, guard_null_pointer)
{
    {
        i2c_config_t * config = NULL;
        auto ret = i2c_get_default_config(config);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
    }
    {
        i2c_handle_t * handle = NULL;
        auto ret = i2c_set_handle(0U, handle);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
        ret = i2c_get_handle(0U, handle);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
    }
    {
        uint8_t * address = NULL;
        auto ret = i2c_get_slave_address(0U, address);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
    }
    {
        i2c_prescaler_t * prescaler = NULL;
        auto ret = i2c_get_prescaler(0U, prescaler);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
    }
    {
        uint8_t * baudrate = NULL;
        auto ret = i2c_get_baudrate(0U, baudrate);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
    }
    {
        bool * general_call_enabled = NULL;
        auto ret = i2c_get_general_call_enabled(0U, general_call_enabled);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
    }
    {
        bool * use_interrupt = NULL;
        auto ret = i2c_get_interrupt_mode(0U, use_interrupt);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
    }
    {
        uint8_t * status_code = NULL;
        auto ret = i2c_get_status_code(0U, status_code);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
    }
    {
        i2c_command_handler_t command = NULL;
        auto ret = i2c_slave_set_command_handler(0U, command);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
    }
    {
        i2c_config_t * config = NULL;
        auto ret = i2c_init(0U, config);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
    }
    {
        i2c_state_t * state = NULL;
        auto ret = i2c_get_state(0U, state);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
    }
    {
        uint8_t * const buffer = NULL;
        auto ret = i2c_write(0U, 0x00, buffer, 10, 0);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
    }
    {
        uint8_t * const buffer = NULL;
        auto ret = i2c_read(0U, 0x00, buffer, 10, 0);
        ASSERT_EQ(ret, I2C_ERROR_NULL_POINTER);
    }
}



int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}