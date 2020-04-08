#include "gtest/gtest.h"
#include "adc.h"
#include "adc_reg.h"
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

class AdcTestFixture : public ::testing::Test
{
public:
    adc_config_hal_t config;
protected:
    void SetUp() override
    {
        adc_register_stub_erase(&adc_register_stub);
        config.alignment = ADC_LEFT_ALIGNED_RESULT;
        config.prescaler = ADC_PRESCALER_4;
        config.ref = ADC_VOLTAGE_REF_AREF_PIN;
        config.running_mode = ADC_RUNNING_MODE_SINGLE_SHOT;
        config.supply_voltage_mv = 5000;
        config.trigger_sources = ADC_TRIGGER_EXT_INT_REQUEST_0;
        config.using_interrupt = true;
        adc_register_stub_init_adc_handle(&config.handle, &adc_register_stub);
    }
    void TearDown() override
    {
        adc_base_deinit();
    }
};

TEST(adc_driver_tests, guard_null)
{
    adc_config_hal_t * config_default = NULL;
    adc_register_stub_erase(&adc_register_stub);
    {
        const auto& result = adc_base_init(config_default);
        ASSERT_EQ(PERIPHERAL_ERROR_NULL_POINTER, result);
    }
    {
        const auto& result = adc_config_hal_copy(config_default, config_default);
        ASSERT_EQ(PERIPHERAL_ERROR_NULL_POINTER, result);
    }
    {
        const auto& result = adc_config_hal_reset(config_default);
        ASSERT_EQ(PERIPHERAL_ERROR_NULL_POINTER, result);
    }
    {
        adc_handle_t * null_handle = NULL;
        const auto& result = adc_handle_copy(null_handle, null_handle);
        ASSERT_EQ(PERIPHERAL_ERROR_NULL_POINTER, result);
    }
    {
        adc_handle_t * null_handle = NULL;
        const auto& result = adc_handle_reset(null_handle);
        ASSERT_EQ(PERIPHERAL_ERROR_NULL_POINTER, result);
    }
    {
        adc_result_t * null_result = NULL;
        const auto& result = adc_read_raw(ADC_MUX_GND, null_result);
        ASSERT_EQ(PERIPHERAL_ERROR_NULL_POINTER, result);
    }
    {
        adc_millivolts_t * null_result = NULL;
        const auto& result = adc_read_millivolt(ADC_MUX_GND, null_result);
        ASSERT_EQ(PERIPHERAL_ERROR_NULL_POINTER, result);
    }
}

TEST_F(AdcTestFixture, check_config_copy_ok)
{
    adc_config_hal_t config_copy;
    {
        const auto& ret2 = adc_handle_reset(&config_copy.handle);
        ASSERT_EQ(PERIPHERAL_ERROR_OK, ret2);
    }
    adc_register_stub_erase(&adc_register_stub);
    const auto& copy_result = adc_config_hal_copy(&config_copy, &config);
    ASSERT_EQ(0,memcmp(&config_copy, &config, sizeof(adc_config_hal_t)));
}

TEST_F(AdcTestFixture, adc_base_init_check)
{
    /* test initialisation of registers */
    const auto& init_result = adc_base_init(&config);
    ASSERT_EQ(init_result, PERIPHERAL_ERROR_OK);
    
    // No channels should be set at this moment
    ASSERT_EQ(0, adc_register_stub.mux_reg & MUX_MSK);
    ASSERT_EQ(config.alignment       , (adc_register_stub.mux_reg & ADLAR_MSK) >> ADLAR);
    ASSERT_EQ(config.ref             , (adc_register_stub.mux_reg & REF_MSK) >> REFS0);
    ASSERT_EQ(config.prescaler       , (adc_register_stub.adcsra_reg & ADPS_MSK) >> ADPS0);
    ASSERT_EQ(config.trigger_sources , (adc_register_stub.adcsrb_reg & ADTS_MSK) >> ADTS0);
}

TEST_F(AdcTestFixture, adc_base_init_check_2)
{
    const auto& unitialised_result = adc_process();
    ASSERT_EQ(unitialised_result, PERIPHERAL_STATE_NOT_INITIALISED);
    /* test initialisation of registers */
    const auto& init_result = adc_base_init(&config);
    ASSERT_EQ(init_result, PERIPHERAL_ERROR_OK);

    // Shall be ok now
    const auto& process_result = adc_process();
    ASSERT_EQ(process_result, PERIPHERAL_STATE_READY);
}

TEST_F(AdcTestFixture, adc_process_test)
{
    const uint8_t channels = 3U;
    const uint8_t max_readings = 4U;
    const uint8_t max_values = channels * max_readings;
    const uint16_t values[max_values] = {856, 412, 12,
                                         547, 985, 11,
                                         25 , 14 , 54,
                                         412, 23 , 18};
    /* test initialisation of registers */
    const auto& init_result = adc_base_init(&config);
    ASSERT_EQ(init_result, PERIPHERAL_ERROR_OK);

    /* Will fail because no channels has been registered yet */
    {
        adc_result_t adcresult;
        const auto& result = adc_read_raw(ADC_MUX_ADC0, &adcresult);
        ASSERT_EQ(result, PERIPHERAL_ERROR_FAILED);
    }

    // Register 3 channels
    ASSERT_EQ(adc_register_channel(ADC_MUX_ADC0), PERIPHERAL_ERROR_OK);
    ASSERT_EQ(adc_register_channel(ADC_MUX_ADC1), PERIPHERAL_ERROR_OK);
    ASSERT_EQ(adc_register_channel(ADC_MUX_ADC2), PERIPHERAL_ERROR_OK);

    /* This read shall not fail */
    {
        adc_result_t adcresult;
        const auto& result = adc_read_raw(ADC_MUX_ADC0, &adcresult);
        ASSERT_EQ(result, PERIPHERAL_ERROR_OK);
        ASSERT_EQ(0, adcresult);
    }

    /* Simulate the adc's internal register thanks to the stub object */
    for (uint8_t i = 0; i < max_values ; i++)
    {
        adc_register_stub.readings.adclow_reg = (uint8_t) values[i] & 0xFF;
        adc_register_stub.readings.adchigh_reg = (uint8_t) ((values[i] & 0x0300) >> 8U);
        
        /* set the conversion finished flag and "stop" the adc manually */
        adc_register_stub.adcsra_reg |= (1 << ADIF);
        adc_register_stub.adcsra_reg &= ~(1 << ADSC);
        
        // Shall be ok now
        const auto& process_result = adc_process();
        EXPECT_EQ(process_result, PERIPHERAL_STATE_READY);

        adc_result_t result;
        const auto& read_result = adc_read_raw(mux_lookup_table[i % channels], &result);
        EXPECT_EQ(read_result, PERIPHERAL_ERROR_OK);
        EXPECT_EQ(values[i], result);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}