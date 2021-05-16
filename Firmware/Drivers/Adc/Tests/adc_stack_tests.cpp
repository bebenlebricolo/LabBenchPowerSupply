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

TEST(adc_stack_tests, register_channels_until_full)
{
    volatile adc_stack_t registered_channels;
    const auto clear_result = adc_stack_reset(&registered_channels);
    ASSERT_EQ(clear_result, ADC_STACK_ERROR_OK);
    for (uint8_t i = 0 ; i < ADC_MUX_COUNT ; i++)
    {
        const auto result = adc_stack_register_channel(&registered_channels, ADC_MUX_ADC0);
        EXPECT_EQ(result, ADC_STACK_ERROR_OK);
    }
    const auto result = adc_stack_register_channel(&registered_channels, ADC_MUX_ADC0);
    ASSERT_EQ(result, ADC_STACK_ERROR_FULL);
}

TEST(adc_stack_tests, remove_channels_from_the_end_until_empty)
{
    volatile adc_stack_t registered_channels;
    const auto clear_result = adc_stack_reset(&registered_channels);
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

TEST(adc_stack_tests, remove_channels_in_the_middle)
{
    volatile adc_stack_t registered_channels;
    const auto clear_result = adc_stack_reset(&registered_channels);
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

TEST(adc_stack_tests, adc_stack_guard_null)
{
    volatile adc_stack_t * registered_channels = NULL;
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
        volatile adc_channel_pair_t * pair = NULL;
        const auto result = adc_stack_get_next(registered_channels, &pair);
        ASSERT_EQ(result, ADC_STACK_ERROR_NULL_POINTER);
    }
    {
        volatile adc_channel_pair_t * pair = NULL;
        const auto result = adc_stack_get_next(registered_channels , &pair);
        ASSERT_EQ(result, ADC_STACK_ERROR_NULL_POINTER);
    }
}

TEST(adc_stack_tests, get_next)
{
    const uint8_t max_registered_values = 11U;
    volatile adc_stack_t registered_channels;
    const auto clear_result = adc_stack_reset(&registered_channels);
    ASSERT_EQ(clear_result, ADC_STACK_ERROR_OK);

    volatile adc_channel_pair_t * pair = NULL;
    const auto empty_next_result = adc_stack_get_next(&registered_channels, &pair);
    ASSERT_EQ(empty_next_result, ADC_STACK_ERROR_EMPTY);

    // First fill in registered_channels 
    for (uint8_t i = 0 ; i < max_registered_values ; i++)
    {
        adc_stack_register_channel(&registered_channels, mux_lookup_table[i]);
    }

    for (uint8_t i = 0; i < max_registered_values + 20 ; i++)
    {
        const auto result = adc_stack_get_next(&registered_channels, &pair);
        EXPECT_EQ(result, ADC_STACK_ERROR_OK);
        if ( (i % max_registered_values) < max_registered_values - 1)
        {
            EXPECT_EQ(pair, &registered_channels.channels_pair[ (i % max_registered_values) + 1]);
        }
        else 
        {
            EXPECT_EQ(pair, &registered_channels.channels_pair[0]);
        }
    }
}

TEST(adc_stack_tests, get_current)
{
    const uint8_t max_registered_values = 11U;
    volatile adc_stack_t registered_channels;
    const auto clear_result = adc_stack_reset(&registered_channels);
    ASSERT_EQ(clear_result, ADC_STACK_ERROR_OK);

    volatile adc_channel_pair_t * next_pair = NULL;
    const auto empty_next_result = adc_stack_get_next(&registered_channels, &next_pair);
    ASSERT_EQ(empty_next_result, ADC_STACK_ERROR_EMPTY);

    // First fill in registered_channels 
    for (uint8_t i = 0 ; i < max_registered_values ; i++)
    {
        adc_stack_register_channel(&registered_channels, mux_lookup_table[i]);
    }

    for (uint8_t i = 0; i < max_registered_values + 20 ; i++)
    {
        const auto& result = adc_stack_get_next(&registered_channels,&next_pair);
        EXPECT_EQ(result, ADC_STACK_ERROR_OK);
        volatile adc_channel_pair_t * current_pair = NULL;
        const auto& current_result = adc_stack_get_current(&registered_channels, &current_pair);
        EXPECT_EQ(current_result, ADC_STACK_ERROR_OK);
        EXPECT_EQ(next_pair, current_pair);
        EXPECT_EQ(current_pair, &registered_channels.channels_pair[registered_channels.index]);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}