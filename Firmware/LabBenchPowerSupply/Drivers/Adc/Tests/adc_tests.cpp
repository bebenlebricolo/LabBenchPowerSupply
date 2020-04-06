#include "gtest/gtest.h"
#include "adc_stack.h"

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

TEST(adc_stack_tests, adc_stack_test_register_channels_until_full)
{
    adc_stack_t registered_channels;
    const auto clear_result = adc_stack_clear(&registered_channels);
    ASSERT_EQ(clear_result, ADC_STACK_ERROR_OK);
    for (uint8_t i = 0 ; i < ADC_MUX_COUNT ; i++)
    {
        const auto result = adc_stack_register_channel(&registered_channels, ADC_MUX_ADC0);
        EXPECT_EQ(result, ADC_STACK_ERROR_OK);
    }
    const auto result = adc_stack_register_channel(&registered_channels, ADC_MUX_ADC0);
    ASSERT_EQ(result, ADC_STACK_ERROR_FULL);
}

TEST(adc_stack_tests, adc_stack_test_remove_channels_from_the_end_until_empty)
{
    adc_stack_t registered_channels;
    const auto clear_result = adc_stack_clear(&registered_channels);
    ASSERT_EQ(clear_result, ADC_STACK_ERROR_OK);
    // First fill in registered_channels 
    for (uint8_t i = 0 ; i < ADC_MUX_COUNT ; i++)
    {
        adc_stack_register_channel(&registered_channels, mux_lookup_table[i]);
    }

    // Then sequentially remove them
    for (uint8_t i = 0; i < ADC_MUX_COUNT ; i++)
    {
        const auto result = adc_stack_unregister_channel(&registered_channels, mux_lookup_table[ADC_MUX_COUNT - i - 1]);
        EXPECT_EQ(result, ADC_STACK_ERROR_OK);
        EXPECT_EQ(registered_channels.count, ADC_MUX_COUNT - i - 1);
    }

    const auto result = adc_stack_unregister_channel(&registered_channels, ADC_MUX_ADC0);
    ASSERT_EQ(result, ADC_STACK_ERROR_EMPTY);
}

TEST(adc_stack_tests, adc_stack_test_remove_channels_in_the_middle)
{
    adc_stack_t registered_channels;
    const auto clear_result = adc_stack_clear(&registered_channels);
    ASSERT_EQ(clear_result, ADC_STACK_ERROR_OK);
    // First fill in registered_channels 
    for (uint8_t i = 0 ; i < ADC_MUX_COUNT ; i++)
    {
        adc_stack_register_channel(&registered_channels, mux_lookup_table[i]);
    }

    const auto result = adc_stack_unregister_channel(&registered_channels, ADC_MUX_ADC0);
    ASSERT_EQ(result, ADC_STACK_ERROR_OK);
    for (uint8_t i = 0 ; i < ADC_MUX_COUNT - 1 ; i++)
    {
        EXPECT_EQ(registered_channels.channels_pair[i].channel, mux_lookup_table[i+1]);
    }
    ASSERT_EQ(registered_channels.channels_pair[ADC_MUX_COUNT - 1].channel, ADC_MUX_GND);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}