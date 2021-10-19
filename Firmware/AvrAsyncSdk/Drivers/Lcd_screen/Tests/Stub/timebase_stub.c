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

#include "timebase.h"
#include "string.h"

typedef struct
{
    uint16_t values[TIMEBASE_STUB_MAX_STUBBED_TIMES];
    uint8_t index;
    uint8_t length;
} stubbed_data_fifo_t;

static stubbed_data_fifo_t durations_data;
static stubbed_data_fifo_t ticks_data;

static inline uint16_t get_stubbed_value(stubbed_data_fifo_t * const source)
{
    uint16_t returned_value = source->values[source->index];
    source->index++;
    // Loopback to 0 when reached the end of pre-programmed data array
    if(source->length == 0)
    {
        return 0;
    }
    source->index %= source->length;
    return returned_value;
}

timebase_error_t timebase_get_tick(uint8_t id, uint16_t * const tick)
{
    (void) id;
    *tick = get_stubbed_value(&ticks_data);
    return TIMEBASE_ERROR_OK;
}

timebase_error_t timebase_get_duration(uint16_t const * const reference, uint16_t * const new_tick, uint16_t * const duration)
{
    (void) reference;
    (void) new_tick;
    *duration = get_stubbed_value(&durations_data);
    return TIMEBASE_ERROR_OK;
}

timebase_error_t timebase_get_duration_now(uint8_t id, uint16_t const * const reference, uint16_t * const duration)
{
    (void) id;
    (void) reference;
    return timebase_get_duration(0,0,duration);
}

void timebase_stub_set_times(uint16_t const * const ticks, const uint8_t len)
{
    memset(&ticks_data, 0, sizeof(stubbed_data_fifo_t));
    uint8_t length = (len <= TIMEBASE_STUB_MAX_STUBBED_TIMES) ? len : TIMEBASE_STUB_MAX_STUBBED_TIMES;
    memcpy(ticks_data.values, ticks, length);
    ticks_data.length = length;
}

void timebase_stub_set_durations(uint16_t const * const durations, const uint8_t len)
{
    memset(&durations_data, 0, sizeof(stubbed_data_fifo_t));
    uint8_t length = (len <= TIMEBASE_STUB_MAX_STUBBED_TIMES) ? len : TIMEBASE_STUB_MAX_STUBBED_TIMES;
    memcpy(durations_data.values, durations, length * sizeof(uint16_t));
    durations_data.length = length;
}

void timebase_stub_clear(void)
{
    memset(&ticks_data, 0, sizeof(stubbed_data_fifo_t));
    memset(&durations_data, 0, sizeof(stubbed_data_fifo_t));
}