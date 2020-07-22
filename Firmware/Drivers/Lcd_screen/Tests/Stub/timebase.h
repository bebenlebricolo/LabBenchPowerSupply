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