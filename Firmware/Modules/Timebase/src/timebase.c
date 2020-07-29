#include <stdbool.h>
#include <stddef.h>

#include "config.h"
#include "timebase.h"

#include "timer_8_bit.h"
#include "timer_16_bit.h"
#include "timer_8_bit_async.h"

#ifndef TIMEBASE_MAX_MODULES
    #error "TIMEBASE_MAX_MODULES define is missing, please set the maximum number of available timebase modules in your config.h"
#endif

typedef struct
{
    timebase_timer_t timer;
    uint8_t id;
} internal_config_t;

static internal_config_t internal_config[TIMEBASE_MAX_MODULES] = {0};


static inline bool is_index_valid(const uint8_t id)
{
    bool out = true;
    if (id >= TIMEBASE_MAX_MODULES)
    {
        out = false;
    }
    return out;
}

timebase_error_t timebase_init(const uint8_t id, timebase_config_t const * const config)
{
    timebase_error_t ret = TIMEBASE_ERROR_OK;
    if (false == is_index_valid(id))
    {
        return TIMEBASE_ERROR_INVALID_INDEX;
    }

    if (NULL == config)
    {
        return TIMEBASE_ERROR_NULL_POINTER;
    }

    internal_config[id].id = config->timer.index;
    internal_config[id].timer = config->timer.type;
    uint32_t target_freq = 0;

    /* Handle target frequency */
    switch (config->timescale)
    {
        case TIMEBASE_TIMESCALE_MICROSECONDS:
            target_freq = 1'000'000UL;
            break;
        case TIMEBASE_TIMESCALE_MILLISECONDS:
            target_freq = 1'000UL;
            break;
        case TIMEBASE_TIMESCALE_SECONDS:
            target_freq = 1UL;
            break;
        case TIMEBASE_TIMESCALE_CUSTOM:
            target_freq = config->custom_target_freq;
            break;
        default:
            return TIMEBASE_ERROR_UNSUPPORTED_TIMESCALE;
    }

    switch(config->timer.type)
    {
        case TIMEBASE_TIMER_8_BIT:
            {
                uint8_t ocra = 0;
                timer_8_bit_prescaler_selection_t prescaler;
                timer_8_bit_compute_matching_parameters(config->cpu_freq,
                                                        &target_freq,
                                                        &prescaler,
                                                        &ocra);
            }
            break;

        case TIMEBASE_TIMER_8_BIT_ASYNC:
            break;

        case TIMEBASE_TIMER_16_BIT:
            break;

        default:
            break;
    }


}