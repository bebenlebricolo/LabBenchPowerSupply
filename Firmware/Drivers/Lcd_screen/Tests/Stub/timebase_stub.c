#include "timebase.h"

timebase_error_t timebase_get_tick(uint8_t id, uint16_t * tick)
{
    return TIMEBASE_ERROR_OK;
}

timebase_error_t timebase_get_duration(uint16_t reference, uint16_t new_tick, uint16_t * duration)
{
    return TIMEBASE_ERROR_OK;
}

timebase_error_t timebase_get_duration_now(uint8_t id, uint16_t reference, uint16_t * duration)
{
    return TIMEBASE_ERROR_OK;
}
