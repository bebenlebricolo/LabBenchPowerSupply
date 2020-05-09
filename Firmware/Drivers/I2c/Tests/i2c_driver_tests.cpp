#include "gtest/gtest.h"
#include "i2c.h"
#include "i2c_register_stub.h"
#include "test_isr_stub.h"


class I2cTestFixture : public ::testing::Test
{
public:
    i2c_config_t config;
protected:
    void SetUp() override
    {
        for (uint8_t i = 0; i < I2C_DEVICES_COUNT ; i++)
        {
            i2c_register_stub_erase(i);
        }
        auto ret = i2c_get_default_config(&config);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        config.baudrate = 100;
        config.general_call_enabled = false;
        config.interrupt_enabled = true;
        config.prescaler = I2C_PRESCALER_16;
        config.slave_address = 0x23;

        ret = i2c_init(0U, &config);
        ASSERT_EQ(ret, I2C_ERROR_OK);
    }

    void TearDown() override
    {
        auto ret = i2c_deinit(0U);
        ASSERT_EQ(ret, I2C_ERROR_OK);
    }
};

i2c_slave_handler_error_t stubbed_command_handler(volatile i2c_command_handling_buffers_t * buffer, uint8_t byte)
{
    (void) byte;
    (void) buffer;
    return I2C_SLAVE_HANDLER_ERROR_OK;
}

TEST(i2c_driver_tests, guard_null_pointer)
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
        uint8_t * address_mask = NULL;
        auto ret = i2c_get_slave_address_mask(0U, address_mask);
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

TEST(i2c_driver_tests, guard_null_handle)
{
    i2c_register_stub_t *stub = &i2c_register_stub[0];
    {
        uint8_t address = 0x12;
        uint8_t old_address = 0x33;
        stub->twar_reg = (stub->twar_reg & ~0xFE) | old_address << 1U;
        auto ret = i2c_set_slave_address(0U, address);

        /* Check that nothing has been modified under the hood : addresses should remain the same */
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_EQ(old_address, (stub->twar_reg & 0xFE) >> 1U);

        /* Same goes with get slave address function */
        ret = i2c_get_slave_address(0U, &address);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_EQ(address, 0x12);
        ASSERT_EQ(old_address, (stub->twar_reg & 0xFE) >> 1U);
    }
    {
        uint8_t address_mask = 0x07U;
        uint8_t old_address_mask = 0x0F;
        stub->twar_reg = (stub->twar_reg & ~0xFE) | old_address_mask << 1U;
        auto ret = i2c_set_slave_address_mask(0U, address_mask);

        /* Check that nothing has been modified under the hood : addresses should remain the same */
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_EQ(old_address_mask, (stub->twar_reg & 0xFE) >> 1U);

        /* Same goes with get slave address mask function */
        ret = i2c_get_slave_address_mask(0U, &address_mask);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_EQ(address_mask, 0x07U);
        ASSERT_EQ(old_address_mask, (stub->twar_reg & 0xFE) >> 1U);
    }
    {
        i2c_prescaler_t prescaler = I2C_PRESCALER_16;
        uint8_t old_prescaler = stub->twsr_reg & 0x3;
        auto ret = i2c_set_prescaler(0U, prescaler);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);

        /* Check that nothing has been modified under the hood : registers shall still have the same value */
        ASSERT_EQ(old_prescaler, stub->twsr_reg & 0x3);
        ret = i2c_get_prescaler(0U, &prescaler);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_EQ(prescaler, I2C_PRESCALER_16);
    }
    {
        uint8_t baudrate = 123;
        uint8_t old_baudrate = 28;
        stub->twbr_reg = old_baudrate;
        auto ret = i2c_set_baudrate(0, baudrate);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_EQ(stub->twbr_reg, old_baudrate);

        ret = i2c_get_baudrate(0U, &baudrate);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_EQ(baudrate, 123);
        ASSERT_EQ(stub->twbr_reg, old_baudrate);
    }
    {
        bool gcenabled = true;
        stub->twar_reg &= ~TWGCE_MSK;
        auto ret = i2c_set_general_call_enabled(0U, gcenabled);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_EQ(gcenabled, true);
        ASSERT_EQ(stub->twar_reg & TWGCE_MSK, 0U);

        ret = i2c_get_general_call_enabled(0, &gcenabled);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_EQ(gcenabled, true);
        ASSERT_EQ(stub->twar_reg & TWGCE_MSK, 0U);
    }
    {
        bool use_interrupt = true;
        stub->twcr_reg &= ~TWIE_MSK;
        auto ret = i2c_set_interrupt_mode(0U, use_interrupt);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_EQ(true, use_interrupt);
        ASSERT_EQ(stub->twcr_reg & TWIE_MSK, 0U);

        ret = i2c_get_interrupt_mode(0U, &use_interrupt);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_EQ(true, use_interrupt);
        ASSERT_EQ(stub->twcr_reg & TWIE_MSK, 0U);
    }
    {
        uint8_t status_code = 154;
        stub->twsr_reg &= ~TWS_MSK;
        auto ret = i2c_get_status_code(0U, &status_code);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_EQ(154, status_code);
        ASSERT_EQ(stub->twsr_reg & TWS_MSK, 0U);
    }
    {
        // Unset TWEN bit and check it hasn't been set afterwards ('enable' shall fail)
        stub->twcr_reg &= ~TWEN_MSK;
        auto ret = i2c_enable(0U);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_EQ(stub->twcr_reg & TWEN_MSK, 0U);
    }
    {
        // Set TWEN bit and check it hasn't been flipped by the disable function (shall fail)
        stub->twcr_reg |= TWEN_MSK;
        auto ret = i2c_disable(0U);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
        ASSERT_NE(stub->twcr_reg & TWEN_MSK, 0U);
    }
    {
        i2c_config_t config;
        auto ret = i2c_get_default_config(&config);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ret = i2c_init(0U, &config);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
    }
    {
        auto ret = i2c_deinit(0U);
        ASSERT_EQ(ret, I2C_ERROR_NULL_HANDLE);
    }
}

TEST(i2c_driver_tests, guard_out_of_range)
{
    // Should break on every function
    uint8_t id = I2C_DEVICES_COUNT;
    {
        i2c_handle_t handle;
        auto ret = i2c_set_handle(id, &handle);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
        ret = i2c_get_handle(id, &handle);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
    {
        uint8_t address = 33;
        auto ret = i2c_set_slave_address(id, address);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
        ret = i2c_get_slave_address(id, &address);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
    {
        uint8_t address_mask = 0x0FU;
        auto ret = i2c_set_slave_address_mask(id, address_mask);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
        ret = i2c_get_slave_address_mask(id, &address_mask);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
    {
        i2c_prescaler_t prescaler;
        auto ret = i2c_set_prescaler(id, prescaler);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
        ret = i2c_get_prescaler(id, &prescaler);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
    {
        uint8_t baudrate;
        auto ret = i2c_set_baudrate(id, baudrate);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
        ret = i2c_get_baudrate(id, &baudrate);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
    {
        bool gcenabled;
        auto ret = i2c_set_general_call_enabled(id, gcenabled);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
        ret = i2c_get_general_call_enabled(id, &gcenabled);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
    {
        bool use_interrupt;
        auto ret = i2c_set_interrupt_mode(id, use_interrupt);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
        ret = i2c_get_interrupt_mode(id, &use_interrupt);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
    {
        uint8_t status_code;
        auto ret = i2c_get_status_code(id, &status_code);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
    {
        auto ret = i2c_enable(id);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
        ret = i2c_disable(id);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
    {
        i2c_command_handler_t handler = stubbed_command_handler;

        auto ret = i2c_slave_set_command_handler(id, handler);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
    {
        i2c_config_t config;
        auto ret = i2c_init(id, &config);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
        ret = i2c_deinit(id);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
    {
        i2c_state_t state;
        auto ret = i2c_get_state(id, &state);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
    {
        auto ret = i2c_process(id);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
    {
        uint8_t address = 33;
        uint8_t buffer;
        uint8_t length = 2;
        auto ret = i2c_read(id, address, &buffer, length, 0);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
        ret = i2c_write(id, address, &buffer, length, 0);
        ASSERT_EQ(ret , I2C_ERROR_DEVICE_NOT_FOUND);
    }
}

TEST(i2c_driver_tests, guard_uninitialised_device)
{
    i2c_config_t config;
    auto ret = i2c_get_default_config(&config);
    ASSERT_EQ(ret, I2C_ERROR_OK);

    i2c_register_stub_init_handle(0U, &config.handle);
    ret = i2c_set_handle(0U, &config.handle);
    ASSERT_EQ(ret, I2C_ERROR_OK);

    ret = i2c_process(0U);
    ASSERT_EQ(ret, I2C_ERROR_NOT_INITIALISED);

    {
        uint8_t address = 33;
        uint8_t buffer = 8;
        uint8_t length = 2;
        ret = i2c_write(0U, address, &buffer, length, 0U);
        ASSERT_EQ(ret, I2C_ERROR_NOT_INITIALISED);

        ret = i2c_read(0U, address, &buffer, length, 0U);
        ASSERT_EQ(ret, I2C_ERROR_NOT_INITIALISED);
    }
}

TEST(i2c_driver_tests, test_api_accessors_get_set)
{
    i2c_config_t config;
    auto ret = i2c_get_default_config(&config);
    ASSERT_EQ(ret, I2C_ERROR_OK);

    i2c_register_stub_init_handle(0U, &config.handle);
    // Do not use the 'init' function yet because I want to test each accessors api before
    ret = i2c_set_handle(0U, &config.handle);
    ASSERT_EQ(ret, I2C_ERROR_OK);

    i2c_register_stub_t * stub = &i2c_register_stub[0U];

    /* slave device address get/set api */
    {
        uint8_t address = 33;
        stub->twar_reg = 0U;
        ret = i2c_set_slave_address(0U, address);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(stub->twar_reg & 0xFE, address << 1U);

        stub->twar_reg = 28U << 1U;
        ret = i2c_get_slave_address(0U, &address);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(address, 28U);
    }

    /* slave device address mask get/set api */
    {
        uint8_t address_mask = 0x07;
        stub->twamr_reg = 0U;
        ret = i2c_set_slave_address_mask(0U, address_mask);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(stub->twamr_reg & 0xFE, address_mask << 1U);

        stub->twamr_reg = 0x0FU << 1U;
        ret = i2c_get_slave_address_mask(0U, &address_mask);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(address_mask, 0x0FU);
    }

    /* Prescaler get/set api */
    {
        i2c_prescaler_t prescaler = I2C_PRESCALER_16;
        stub->twsr_reg &= ~TWPS_MSK;
        ret = i2c_set_prescaler(0U, prescaler);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(prescaler, (stub->twsr_reg & TWPS_MSK));

        stub->twsr_reg = (stub->twsr_reg & ~TWPS_MSK) | I2C_PRESCALER_64;
        ret = i2c_get_prescaler(0U, &prescaler);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(prescaler, I2C_PRESCALER_64);
    }

    /* Baudrate get/set api */
    {
        uint8_t baudrate = 28;
        stub->twbr_reg = 0;
        ret = i2c_set_baudrate(0U, baudrate);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(stub->twbr_reg, baudrate);

        stub->twbr_reg = 124;
        ret = i2c_get_baudrate(0U, &baudrate);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(baudrate, 124);
    }

    /* General call get/set api */
    {
        bool gcenabled = true;
        stub->twar_reg &= ~TWGCE_MSK;
        ret = i2c_set_general_call_enabled(0U, gcenabled);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(stub->twar_reg & TWGCE_MSK, 1U);

        stub->twar_reg &= ~TWGCE_MSK;
        ret = i2c_get_general_call_enabled(0U, &gcenabled);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(gcenabled, false);
    }

    /* Interrupt mode get/set api */
    {
        bool use_interrupt = true;
        stub->twcr_reg &= ~TWEN_MSK;
        ret = i2c_set_interrupt_mode(0U, use_interrupt);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(stub->twcr_reg & TWEN_MSK, 1U);

        stub->twcr_reg &= ~TWEN_MSK;
        ret = i2c_get_interrupt_mode(0U, &use_interrupt);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(use_interrupt, false);
    }

    /* I2C status code get api */
    {
        uint8_t status_code = 0;
        stub->twsr_reg &= ~TWS_MSK;
        stub->twsr_reg |= (uint8_t) MAS_TX_SLAVE_WRITE_ACK << TWS3_BIT;

        ret = i2c_get_status_code(0U, &status_code);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(status_code, (uint8_t) MAS_TX_SLAVE_WRITE_ACK);
    }

    /* I2C command handler set api */
    {
        i2c_command_handler_t handler = stubbed_command_handler;
        ret = i2c_slave_set_command_handler(0U, handler);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        auto registered_handler = i2c_slave_get_command_handler(0U);
        ASSERT_EQ((volatile size_t) registered_handler, (volatile size_t) stubbed_command_handler);
    }

    /* I2C get state api */
    {
        i2c_state_t state = I2C_STATE_DISABLED;

        i2c_set_state(0U, I2C_STATE_MASTER_RECEIVING);
        ret = i2c_get_state(0U, &state);
        ASSERT_EQ(ret, I2C_ERROR_OK);
        ASSERT_EQ(state, I2C_STATE_MASTER_RECEIVING);
    }
}

//TEST_F(I2cTestFixture, )



int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}