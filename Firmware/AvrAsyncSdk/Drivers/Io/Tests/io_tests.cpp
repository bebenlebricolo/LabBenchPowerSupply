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

#include <gtest/gtest.h>
#include "io.h"
#include "config.h"

TEST(io_driver, test_init)
{
    io_init();
    auto stubs = get_stubbed_registers();
    ASSERT_EQ(stubs->st_mcucr, 0);

    ASSERT_EQ(stubs->st_porta.st_ddr_reg, 0);
    ASSERT_EQ(stubs->st_porta.st_pin_reg, 0);
    ASSERT_EQ(stubs->st_porta.st_port_reg, 0);

    ASSERT_EQ(stubs->st_portb.st_port_reg, 0x02);
    ASSERT_EQ(stubs->st_portb.st_pin_reg, 0);
    ASSERT_EQ(stubs->st_portb.st_ddr_reg, 0x3);

    ASSERT_EQ(stubs->st_portc.st_port_reg, 0);
    ASSERT_EQ(stubs->st_portc.st_pin_reg, 0);
    ASSERT_EQ(stubs->st_portc.st_ddr_reg, 0);

    ASSERT_EQ(stubs->st_portd.st_port_reg, (uint8_t)(1 << 5U));
    ASSERT_EQ(stubs->st_portd.st_pin_reg, 0);
    ASSERT_EQ(stubs->st_portd.st_ddr_reg, 1 << 5U | 1 << 1U);
}

TEST(io_driver, test_read)
{
    io_init();
    auto stubs = get_stubbed_registers();
    stubs->st_portc.st_pin_reg |= 0xF0;

    auto state = io_read(PIN_POWER_SWITCH);
    ASSERT_EQ(state, IO_STATE_LOW);

    stubs->st_portc.st_pin_reg |= (1 << 3U);
    state = io_read(PIN_POWER_SWITCH);
    ASSERT_EQ(state, IO_STATE_HIGH);

    stubs->st_portc.st_pin_reg &= ~(1 << 3U);
    state = io_read(PIN_POWER_SWITCH);
    ASSERT_EQ(state, IO_STATE_LOW);
}

TEST(io_driver, test_write)
{
    io_init();
    auto stubs = get_stubbed_registers();
    ASSERT_EQ(stubs->st_portb.st_port_reg, 0x2);

    io_write(PIN_FAN_RIGHT, IO_STATE_HIGH);
    ASSERT_EQ(stubs->st_portb.st_port_reg, 0x3);

    io_write(PIN_FAN_LEFT, IO_STATE_LOW);
    ASSERT_EQ(stubs->st_portb.st_port_reg, 0x1);

    ASSERT_EQ(stubs->st_portd.st_port_reg, 1 << 5U);
    io_write(PIN_MOSFET_LOW_SIDE, IO_STATE_LOW);
    ASSERT_EQ(stubs->st_portd.st_port_reg, 0x0);

    io_write(PIN_MOSFET_HIGH_SIDE, IO_STATE_HIGH);
    ASSERT_EQ(stubs->st_portd.st_port_reg, 0x2);

    io_write(PIN_MOSFET_LOW_SIDE, IO_STATE_HIGH);
    ASSERT_EQ(stubs->st_portd.st_port_reg, 1 << 1U | 1 << 5U);

    io_write(PIN_MOSFET_HIGH_SIDE, IO_STATE_LOW);
    ASSERT_EQ(stubs->st_portd.st_port_reg, 1 << 5U);

    io_write(PIN_MOSFET_HIGH_SIDE, IO_STATE_HIGH);
    ASSERT_EQ(stubs->st_portd.st_port_reg, 1 << 1U | 1 << 5U);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}