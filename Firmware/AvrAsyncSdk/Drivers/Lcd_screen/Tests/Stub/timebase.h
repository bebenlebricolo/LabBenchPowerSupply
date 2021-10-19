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

#ifndef TIMEBASE_STUB_HEADER
#define TIMEBASE_STUB_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#define TIMEBASE_STUB_MAX_STUBBED_TIMES 40U

typedef enum
{
    TIMEBASE_ERROR_OK
} timebase_error_t;

timebase_error_t timebase_get_tick(uint8_t id, uint16_t * const tick);
timebase_error_t timebase_get_duration(uint16_t const * const reference, uint16_t * const new_tick, uint16_t * const duration);
timebase_error_t timebase_get_duration_now(uint8_t id, uint16_t const * const reference, uint16_t * const duration);

/* Unit testing specificities */
void timebase_stub_set_times(uint16_t const * const ticks, const uint8_t len);
void timebase_stub_set_durations(uint16_t const * const durations, const uint8_t len);
void timebase_stub_clear(void);

#ifdef __cplusplus
}
#endif

#endif /* TIMEBASE_STUB_HEADER */