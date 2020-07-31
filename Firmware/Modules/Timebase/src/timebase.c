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

static inline timebase_error_t setup_8_bit_timer(const uint8_t id, uint32_t const * const cpu_freq, uint32_t const * const target_freq)
{
    if(false == timer_8_bit_is_initialised(id))
    {
        return TIMEBASE_ERROR_TIMER_UNINITIALISED;
    }

    uint8_t ocra = 0;
    uint32_t accumulator=  0;
    timer_8_bit_prescaler_selection_t prescaler;
    timer_8_bit_compute_matching_parameters(cpu_freq,
                                            target_freq,
                                            &prescaler,
                                            &ocra,
                                            &accumulator);

    timer_error_t ret = timer_8_bit_stop(id);
    timer_8_bit_handle_t handle = {0};
    ret = timer_8_bit_get_handle(id, &handle);

    if (TIMER_ERROR_OK != ret)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    timer_8_bit_config_t config = {0};
    ret = timer_8_bit_get_default_config(&config);
    if (TIMER_ERROR_OK != ret)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    // Use old handle
    config.handle = handle;
    config.timing_config.comp_match_a = TIMER8BIT_CMOD_CLEAR_OCnX;
    config.timing_config.comp_match_b = TIMER8BIT_CMOD_NORMAL;
    config.timing_config.ocra_val = ocra;
    config.timing_config.prescaler = prescaler;
    config.interrupt_config.it_comp_match_a = true;


    return TIMEBASE_ERROR_OK;
}

static inline timebase_error_t setup_8_bit_async_timer(const uint8_t id, uint32_t const * const cpu_freq, uint32_t const * const target_freq)
{
    if(false == timer_8_bit_async_is_initialised(id))
    {
        return TIMEBASE_ERROR_TIMER_UNINITIALISED;
    }
    uint8_t ocra = 0;
    uint32_t accumulator=  0;
    timer_8_bit_async_prescaler_selection_t prescaler;
    timer_8_bit_async_compute_matching_parameters(cpu_freq,
                                                  target_freq,
                                                  &prescaler,
                                                  &ocra,
                                                  &accumulator);
    return TIMEBASE_ERROR_OK;
}

static inline timebase_error_t setup_16_bit_timer(const uint8_t id, uint32_t const * const cpu_freq, uint32_t const * const target_freq)
{
    if(false == timer_16_bit_is_initialised(id))
    {
        return TIMEBASE_ERROR_TIMER_UNINITIALISED;
    }

    uint16_t ocra = 0;
    uint32_t accumulator=  0;
    timer_16_bit_prescaler_selection_t prescaler;
    timer_16_bit_compute_matching_parameters(cpu_freq,
                                             target_freq,
                                             &prescaler,
                                             &ocra,
                                             &accumulator);
    return TIMEBASE_ERROR_OK;
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

    // Initialise each timer using the right parameters set
    switch(config->timer.type)
    {
        case TIMEBASE_TIMER_8_BIT:
            ret = setup_8_bit_timer(id, &(config->cpu_freq), &target_freq);
            break;

        case TIMEBASE_TIMER_8_BIT_ASYNC:
            ret = setup_8_bit_async_timer(id, &(config->cpu_freq), &target_freq);
            break;

        case TIMEBASE_TIMER_16_BIT:
            ret = setup_16_bit_timer(id, &(config->cpu_freq), &target_freq);
            break;

        default:
            return TIMEBASE_ERROR_UNSUPPORTED_TIMER_TYPE;
    }


}