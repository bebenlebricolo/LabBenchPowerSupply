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
    source->index %= source->length;
    return returned_value;
}

timebase_error_t timebase_get_tick(uint8_t id, uint16_t * tick)
{
    (void) id;
    *tick = get_stubbed_value(&ticks_data);
    return TIMEBASE_ERROR_OK;
}

timebase_error_t timebase_get_duration(uint16_t reference, uint16_t new_tick, uint16_t * duration)
{
    (void) reference;
    (void) new_tick;
    *duration = get_stubbed_value(&durations_data);
    return TIMEBASE_ERROR_OK;
}

timebase_error_t timebase_get_duration_now(uint8_t id, uint16_t reference, uint16_t * duration)
{
    (void) id;
    return timebase_get_duration(0,0,duration);
}

void timebase_stub_set_times(uint16_t const * const ticks, const uint8_t len)
{
    memset(&ticks_data, 0, sizeof(stubbed_data_fifo_t));
    uint8_t length = (len <= TIMEBASE_STUB_MAX_STUBBED_TIMES) ? len : TIMEBASE_STUB_MAX_STUBBED_TIMES;
    memcpy(ticks_data.values, ticks, length);
}

void timebase_stub_set_durations(uint16_t const * const durations, const uint8_t len)
{
    memset(&durations_data, 0, sizeof(stubbed_data_fifo_t));
    uint8_t length = (len <= TIMEBASE_STUB_MAX_STUBBED_TIMES) ? len : TIMEBASE_STUB_MAX_STUBBED_TIMES;
    memcpy(durations_data.values, durations, length);
}

void timebase_stub_clear(void)
{
    memset(&ticks_data, 0, sizeof(stubbed_data_fifo_t));
    memset(&durations_data, 0, sizeof(stubbed_data_fifo_t));
}