#include "gtest/gtest.h"
#include "adc.h"
#include "adc_register_stub.h"

static adc_mux_t mux_lookup_table[ADC_MUX_COUNT] = 
{
    ADC_MUX_ADC0,
    ADC_MUX_ADC1,
    ADC_MUX_ADC2,
    ADC_MUX_ADC3,
    ADC_MUX_ADC4,
    ADC_MUX_ADC5,
    ADC_MUX_ADC6,
    ADC_MUX_ADC7,
    ADC_MUX_GND,
    ADC_MUX_INTERNAL_TEMPERATURE,
    ADC_MUX_1v1_REF
};

TEST(adc_driver_tests, check_config_copy_ok)
{
    adc_config_hal_t config_default;
    adc_config_hal_t config_copy;
    {
        const auto& ret2 = adc_handle_reset(&config_copy.handle);
        ASSERT_EQ(PERIPHERAL_ERROR_OK, ret2);
    }
    adc_register_stub_erase(&adc_register_stub);
    /* dummy configuration */
    config_default.alignment = ADC_LEFT_ALIGNED_RESULT;
    config_default.prescaler = ADC_PRESCALER_4;
    config_default.ref = ADC_VOLTAGE_REF_AREF_PIN;
    config_default.running_mode = ADC_RUNNING_MODE_SINGLE_SHOT;
    config_default.supply_voltage_mv = 1203;
    config_default.trigger_sources = ADC_TRIGGER_EXT_INT_REQUEST_0;
    config_default.using_interrupt = true;
    adc_register_stub_init_adc_handle(&config_default.handle, &adc_register_stub);
    const auto& copy_result = adc_config_hal_copy(&config_copy, &config_default);
    ASSERT_EQ(0,memcmp(&config_copy, &config_default, sizeof(adc_config_hal_t)));
}

TEST(adc_driver_tests, adc_base_init_check0)
{
    adc_config_hal_t config_default;
    adc_register_stub_erase(&adc_register_stub);
    adc_register_stub_init_adc_handle(&config_default.handle, &adc_register_stub);
    /* dummy configuration */
    config_default.alignment = ADC_LEFT_ALIGNED_RESULT;
    config_default.prescaler = ADC_PRESCALER_4;
    config_default.ref = ADC_VOLTAGE_REF_AREF_PIN;
    config_default.running_mode = ADC_RUNNING_MODE_SINGLE_SHOT;
    config_default.supply_voltage_mv = 1203;
    config_default.trigger_sources = ADC_TRIGGER_EXT_INT_REQUEST_0;
    config_default.using_interrupt = true;

    /* test initialisation of registers */
    const auto& init_result = adc_base_init(&config_default);
    ASSERT_EQ(init_result, PERIPHERAL_ERROR_OK);
    ASSERT_NE(0 , adc_register_stub.adcsra_reg & (config_default.alignment << ADLAR));
    ASSERT_EQ(adc_register_stub.adcsra_reg & 0b111, config_default.prescaler);
    ASSERT_NE(0 , adc_register_stub.mux_reg & (config_default.ref << REFS0));
    ASSERT_EQ(adc_register_stub.adcsrb_reg & 0xF , config_default.trigger_sources );
}
/*
TEST(adc_stack_tests, adc_stack_guard_null)
{
    adc_stack_t * registered_channels = NULL;
    {
        const auto result = adc_stack_reset(registered_channels);
        ASSERT_EQ(result, ADC_STACK_ERROR_NULL_POINTER);
    }
    {
        const auto result = adc_stack_register_channel(registered_channels, ADC_MUX_GND);
        ASSERT_EQ(result, ADC_STACK_ERROR_NULL_POINTER);
    }
    {
        const auto result = adc_stack_unregister_channel(registered_channels, ADC_MUX_GND);
        ASSERT_EQ(result, ADC_STACK_ERROR_NULL_POINTER);
    }
    {
        adc_channel_pair_t * pair = NULL;
        const auto result = adc_stack_get_next(registered_channels, &pair);
        ASSERT_EQ(result, ADC_STACK_ERROR_NULL_POINTER);
    }
    {
        adc_channel_pair_t * pair = NULL;
        const auto result = adc_stack_get_next(registered_channels , &pair);
        ASSERT_EQ(result, ADC_STACK_ERROR_NULL_POINTER);
    }
}
*/
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}