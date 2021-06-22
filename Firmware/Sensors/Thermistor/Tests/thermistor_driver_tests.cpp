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
#include "thermistor.h"
#include "thermistor_private.h"
#include "config.h"

TEST(thermistor_tests, test_find_segment)
{
    uint8_t target_ratio = 11;
    thermistor_curve_discrete_pair_t const * lower_bound = NULL;
    thermistor_curve_discrete_pair_t const * upper_bound = NULL;
    bool ret = find_segment(thermistor_driver_config[0].model.curve.data.discrete.cold_side, target_ratio, RATIO_COEF_1, &lower_bound, &upper_bound );
    ASSERT_TRUE(ret);
    ASSERT_EQ(lower_bound->ratio, 12U);
    ASSERT_EQ(upper_bound->ratio, 8U);

    // Using the x10 representation, actual ratio is 8
    target_ratio = 80U;
    ret = find_segment(thermistor_driver_config[0].model.curve.data.discrete.cold_side, target_ratio,  RATIO_COEF_10, &lower_bound, &upper_bound );
    ASSERT_TRUE(ret);
    ASSERT_EQ(lower_bound->ratio, 8U);
    ASSERT_EQ(upper_bound->ratio, 8U);
    ASSERT_EQ(lower_bound, upper_bound);

    // Using the x10 representation, actual ratio is 8
    target_ratio = 63U;
    ret = find_segment(thermistor_driver_config[0].model.curve.data.discrete.cold_side, target_ratio,  RATIO_COEF_10, &lower_bound, &upper_bound );
    ASSERT_TRUE(ret);
    ASSERT_EQ(lower_bound->ratio, 8U);
    ASSERT_EQ(upper_bound->ratio, 4U);

    // Using the x100 representation, actual ratio is 0.53 (53%)
    target_ratio = 53U;
    ret = find_segment(thermistor_driver_config[0].model.curve.data.discrete.hot_side, target_ratio,  RATIO_COEF_100, &lower_bound, &upper_bound );
    ASSERT_TRUE(ret);
    ASSERT_EQ(lower_bound->ratio, 100U);
    ASSERT_EQ(upper_bound->ratio, 25U);

    // Using the x100 representation, actual ratio is 0.53 (53%)
    target_ratio = 0U;
    ret = find_segment(thermistor_driver_config[0].model.curve.data.discrete.hot_side, target_ratio,  RATIO_COEF_100, &lower_bound, &upper_bound );
    ASSERT_FALSE(ret);
    ASSERT_EQ(lower_bound->ratio, 1U);
    ASSERT_EQ(upper_bound->ratio, 1U);
}

TEST(thermistor_tests, test_compute_ratios)
{
    // Cold side first
    adc_millivolts_t reading = 2125U;
    uint8_t ratio = 0;
    ratio_coefficients_t coef = compute_ratio(&thermistor_driver_config[0].model, &reading, &ratio);
    ASSERT_EQ(coef, RATIO_COEF_10);
    ASSERT_EQ(ratio, 13U);

    reading = 1200U;
    coef = compute_ratio(&thermistor_driver_config[0].model, &reading, &ratio);
    ASSERT_EQ(coef, RATIO_COEF_10);
    ASSERT_EQ(ratio, 31U);

    reading = 250U;
    coef = compute_ratio(&thermistor_driver_config[0].model, &reading, &ratio);
    ASSERT_EQ(coef, RATIO_COEF_1);
    ASSERT_EQ(ratio, 19);

    reading = 120U;
    coef = compute_ratio(&thermistor_driver_config[0].model, &reading, &ratio);
    ASSERT_EQ(coef, RATIO_COEF_1);
    ASSERT_EQ(ratio, 40);

    // Hot side
    reading = 3000U;
    coef = compute_ratio(&thermistor_driver_config[0].model, &reading, &ratio);
    ASSERT_EQ(coef, RATIO_COEF_100);
    ASSERT_EQ(ratio, 65);

    reading = 4500U;
    coef = compute_ratio(&thermistor_driver_config[0].model, &reading, &ratio);
    ASSERT_EQ(coef, RATIO_COEF_100);
    ASSERT_EQ(ratio, 10U);
}

TEST(thermistor_tests, test_compute_temperature)
{
    adc_millivolts_t reading = 2125U;
    int8_t temp = compute_temperature(0, &reading);
    ASSERT_EQ(temp, 23U);

    reading = 2500U;
    temp = compute_temperature(0, &reading);
    ASSERT_EQ(temp, 25U);

    reading = 2700U;
    temp = compute_temperature(0, &reading);
    ASSERT_EQ(temp, 27U);

    reading = 3200U;
    temp = compute_temperature(0, &reading);
    ASSERT_EQ(temp, 33U);

    reading = 4123U;
    temp = compute_temperature(0, &reading);
    ASSERT_EQ(temp, 42U);

    reading = 4980U;
    temp = compute_temperature(0, &reading);
    ASSERT_EQ(temp, 77);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}