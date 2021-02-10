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