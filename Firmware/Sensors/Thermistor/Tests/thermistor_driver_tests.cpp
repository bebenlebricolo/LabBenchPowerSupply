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
    bool ret = find_segment(thermistor_driver_config[0].model.curve.data.discrete.cold_side, target_ratio, &lower_bound, &upper_bound );
    ASSERT_TRUE(ret);
    ASSERT_EQ(lower_bound->ratio, 12U);
    ASSERT_EQ(upper_bound->ratio, 8U);

    target_ratio = 8;
    ret = find_segment(thermistor_driver_config[0].model.curve.data.discrete.cold_side, target_ratio, &lower_bound, &upper_bound );
    ASSERT_TRUE(ret);
    ASSERT_EQ(lower_bound->ratio, 8U);
    ASSERT_EQ(upper_bound->ratio, 8U);
    ASSERT_EQ(lower_bound, upper_bound);

}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}