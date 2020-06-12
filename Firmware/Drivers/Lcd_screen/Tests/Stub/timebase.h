#ifndef TIMEBASE_STUB_HEADER
#define TIMEBASE_STUB_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef enum
{
    TIMEBASE_ERROR_OK
} timebase_error_t;

timebase_error_t timebase_get_tick(uint8_t id, uint16_t * tick);
timebase_error_t timebase_get_duration(uint16_t reference, uint16_t new_tick, uint16_t * duration);
timebase_error_t timebase_get_duration_now(uint8_t id, uint16_t reference, uint16_t * duration);

#ifdef __cplusplus
}
#endif

#endif /* TIMEBASE_STUB_HEADER */