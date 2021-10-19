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

#include "timer_8_bit.h"
#include "timer_16_bit.h"
#include "timer_8_bit_async.h"

TEST(timer_generic_driver_tests, test_compute_parameters)
{
    const uint8_t max_size = 7U;
    const uint8_t array_size = 5U;
    timer_generic_prescaler_pair_t array[max_size] =
    {
        {1U,    1U},
        {8U,    2U},
        {64U,   3U},
        {256U,  4U},
        {1024U, 5U},
        {0,0},
        {0,0}
    };
    timer_generic_parameters_t parameters;
    parameters.input.cpu_frequency = 16'000'000U;
    parameters.input.target_frequency = 1'000'000U;
    parameters.input.resolution = TIMER_GENERIC_RESOLUTION_8_BIT;
    parameters.input.prescaler_lookup_array.array = array;
    parameters.input.prescaler_lookup_array.size = array_size;

    timer_generic_compute_parameters(&parameters);
    ASSERT_EQ(parameters.output.prescaler, 1U);
    ASSERT_EQ(parameters.output.ocra, 15U);
    ASSERT_EQ(parameters.output.accumulator, 0U);

    parameters.input.cpu_frequency = 16'000'000U;
    parameters.input.target_frequency = 1'000U;
    timer_generic_compute_parameters(&parameters);
    ASSERT_EQ(parameters.output.prescaler, 64U);
    ASSERT_EQ(parameters.output.ocra, 249U);
    ASSERT_EQ(parameters.output.accumulator, 0U);

    parameters.input.cpu_frequency = 16'000'000U;
    parameters.input.target_frequency = 1U;
    timer_generic_compute_parameters(&parameters);
    ASSERT_EQ(parameters.output.prescaler, 1024U);
    ASSERT_EQ(parameters.output.ocra, 124U);
    ASSERT_EQ(parameters.output.accumulator, 124U);

    parameters.input.cpu_frequency = 24'000'000U;
    parameters.input.target_frequency = 1U;
    timer_generic_compute_parameters(&parameters);
    ASSERT_EQ(parameters.output.prescaler, 1024U);
    ASSERT_EQ(parameters.output.ocra, 22U);
    ASSERT_EQ(parameters.output.accumulator, 1018U);

}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}