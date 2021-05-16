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

#ifndef TIMEBASE_INTERNAL_HEADER
#define TIMEBASE_INTERNAL_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include "config.h"
#include "timebase.h"

typedef struct
{
    timebase_timer_t timer;
    uint8_t timer_id;
    struct
    {
        uint16_t programmed;
        uint16_t running;
    } accumulator;
    uint16_t tick;
    bool initialised;
} timebase_internal_config_t;

extern timebase_internal_config_t timebase_internal_config[TIMEBASE_MAX_MODULES];

#ifdef __cplusplus
}
#endif

#endif /* TIMEBASE_INTERNAL_HEADER */