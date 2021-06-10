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

#ifndef CONFIG_HEADER
#define CONFIG_HEADER

/* Defines how many instances of timer/counters will be created */
#define TIMER_8_BIT_COUNT       1
#define TIMER_8_BIT_ASYNC_COUNT 1
#define TIMER_16_BIT_COUNT      1

#define TIMEBASE_MAX_MODULES 3U
#define I2C_DEVICES_COUNT 1U

// Only implement master tx driver
#define I2C_IMPLEM_MASTER_TX
//#define I2C_IMPLEM_FULL_DRIVER

#define IO_MAX_PINS 6U

#endif /* CONFIG_HEADER */