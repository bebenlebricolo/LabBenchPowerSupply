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
    uint8_t timer_id;
    struct
    {
        uint32_t programmed;
        uint32_t running;
    } accumulator;
    uint32_t tick;
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

static inline timebase_error_t setup_8_bit_timer(const uint8_t timebase_id, uint32_t const * const cpu_freq, uint32_t const * const target_freq)
{
    bool initialised = false;
    timer_error_t err = timer_8_bit_is_initialised(internal_config[timebase_id].timer_id, &initialised);
    if (TIMER_ERROR_OK != err)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    if(false == initialised)
    {
        return TIMEBASE_ERROR_TIMER_UNINITIALISED;
    }

    uint8_t ocra = 0;
    internal_config[timebase_id].accumulator.programmed =  0;
    timer_8_bit_prescaler_selection_t prescaler;
    timer_8_bit_compute_matching_parameters(cpu_freq,
                                            target_freq,
                                            &prescaler,
                                            &ocra,
                                            &internal_config[timebase_id].accumulator.programmed);

    timer_error_t ret = timer_8_bit_stop(internal_config[timebase_id].timer_id);
    timer_8_bit_handle_t handle = {0};
    ret = timer_8_bit_get_handle(internal_config[timebase_id].timer_id, &handle);

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

    ret = timer_8_bit_reconfigure(internal_config[timebase_id].timer_id, &config);
    if (TIMER_ERROR_OK != ret)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    return TIMEBASE_ERROR_OK;
}

static inline timebase_error_t setup_8_bit_async_timer(const uint8_t timebase_id, uint32_t const * const cpu_freq, uint32_t const * const target_freq)
{
    bool initialised = false;
    timer_error_t err = timer_8_bit_async_is_initialised(internal_config[timebase_id].timer_id, &initialised);
    if (TIMER_ERROR_OK != err)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    if(false == initialised)
    {
        return TIMEBASE_ERROR_TIMER_UNINITIALISED;
    }
    uint8_t ocra = 0;
    internal_config[timebase_id].accumulator.programmed =  0;
    timer_8_bit_async_prescaler_selection_t prescaler;
    timer_8_bit_async_compute_matching_parameters(cpu_freq,
                                                  target_freq,
                                                  &prescaler,
                                                  &ocra,
                                                  &internal_config[timebase_id].accumulator.programmed);
    return TIMEBASE_ERROR_OK;
}

static inline timebase_error_t setup_16_bit_timer(const uint8_t timebase_id, uint32_t const * const cpu_freq, uint32_t const * const target_freq)
{
    bool initialised = false;
    timer_error_t err = timer_16_bit_is_initialised(internal_config[timebase_id].timer_id, &initialised);
    if (TIMER_ERROR_OK != err)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    if(false == initialised)
    {
        return TIMEBASE_ERROR_TIMER_UNINITIALISED;
    }

    uint16_t ocra = 0;
    internal_config[timebase_id].accumulator.programmed =  0;
    timer_16_bit_prescaler_selection_t prescaler;
    timer_16_bit_compute_matching_parameters(cpu_freq,
                                             target_freq,
                                             &prescaler,
                                             &ocra,
                                             &internal_config[timebase_id].accumulator.programmed);
    return TIMEBASE_ERROR_OK;
}

static inline timebase_error_t convert_timescale_to_frequency(const timebase_config_t * const config, uint32_t * const target_frequency)
{
    /* Handle target frequency */
    switch (config->timescale)
    {
        case TIMEBASE_TIMESCALE_MICROSECONDS:
            *target_frequency = 1000000UL;
            break;
        case TIMEBASE_TIMESCALE_MILLISECONDS:
            *target_frequency = 1000UL;
            break;
        case TIMEBASE_TIMESCALE_SECONDS:
            *target_frequency = 1UL;
            break;
        case TIMEBASE_TIMESCALE_CUSTOM:
            *target_frequency = config->custom_target_freq;
            break;
        default:
            return TIMEBASE_ERROR_UNSUPPORTED_TIMESCALE;
    }
    return TIMEBASE_ERROR_OK;
}

timebase_error_t timebase_compute_timer_parameters(timebase_config_t const * const config, uint16_t * const prescaler_val, uint16_t * const ocr_value, uint32_t * const accumulator)
{
    timer_8_bit_prescaler_selection_t prescaler;
    uint32_t target_frequency = 0;
    timebase_error_t ret = convert_timescale_to_frequency(config, &target_frequency);
    if (TIMEBASE_ERROR_OK != ret)
    {
        return ret;
    }
    uint16_t ocra = 0;

    // Initialise each timer using the right parameters set
    switch(config->timer.type)
    {
        case TIMEBASE_TIMER_8_BIT:
            timer_8_bit_compute_matching_parameters(&config->cpu_freq,
                                                    &target_frequency,
                                                    &prescaler,
                                                    (uint8_t*) &ocra,
                                                    accumulator);
            *prescaler_val = timer_8_bit_prescaler_to_value(prescaler);
            *ocr_value = ocra;
            break;

        case TIMEBASE_TIMER_8_BIT_ASYNC:
            timer_8_bit_async_compute_matching_parameters(&config->cpu_freq,
                                                          &target_frequency,
                                                          (timer_8_bit_async_prescaler_selection_t *) &prescaler,
                                                          (uint8_t*) &ocra,
                                                          accumulator);
            *prescaler_val = timer_8_bit_async_prescaler_to_value((timer_8_bit_async_prescaler_selection_t) prescaler);
            *ocr_value = ocra;
            break;

        case TIMEBASE_TIMER_16_BIT:
            timer_16_bit_compute_matching_parameters(&config->cpu_freq,
                                                     &target_frequency,
                                                     (timer_16_bit_prescaler_selection_t *)&prescaler,
                                                     &ocra,
                                                     accumulator);
            *prescaler_val = timer_16_bit_prescaler_to_value((timer_16_bit_prescaler_selection_t) prescaler);
            *ocr_value = ocra;
            break;

        default:
            return TIMEBASE_ERROR_UNSUPPORTED_TIMER_TYPE;
    }
    return TIMEBASE_ERROR_OK;
}


timebase_error_t timebase_init(const uint8_t timebase_id, timebase_config_t const * const config)
{
    timebase_error_t ret = TIMEBASE_ERROR_OK;
    if (false == is_index_valid(timebase_id))
    {
        return TIMEBASE_ERROR_INVALID_INDEX;
    }

    if (NULL == config)
    {
        return TIMEBASE_ERROR_NULL_POINTER;
    }

    internal_config[timebase_id].timer_id = config->timer.index;
    internal_config[timebase_id].timer = config->timer.type;
    uint32_t target_freq = 0;

    ret = convert_timescale_to_frequency(config, &target_freq);
    if (TIMEBASE_ERROR_OK != ret)
    {
        return ret;
    }

    // Initialise each timer using the right parameters set
    switch(config->timer.type)
    {
        case TIMEBASE_TIMER_8_BIT:
            ret = setup_8_bit_timer(timebase_id, &(config->cpu_freq), &target_freq);
            break;

        case TIMEBASE_TIMER_8_BIT_ASYNC:
            ret = setup_8_bit_async_timer(timebase_id,  &(config->cpu_freq), &target_freq);
            break;

        case TIMEBASE_TIMER_16_BIT:
            ret = setup_16_bit_timer(timebase_id, &(config->cpu_freq), &target_freq);
            break;

        default:
            return TIMEBASE_ERROR_UNSUPPORTED_TIMER_TYPE;
    }

    return ret;
}

void timebase_interrupt_callback(const uint8_t timebase_id)
{
    if (false == is_index_valid(timebase_id))
    {
        return;
    }

    // If an accumulator value was set, increment it and when the running accumulator matches the programmed one,
    // This counts as a new tick.
    if (internal_config[timebase_id].accumulator.programmed != 0)
    {
        if (internal_config[timebase_id].accumulator.running < internal_config[timebase_id].accumulator.programmed)
        {
            internal_config[timebase_id].accumulator.running++;
        }
        else
        {
            internal_config[timebase_id].accumulator.running = 0;
            internal_config[timebase_id].tick++;
        }
    }
    // Otherwise, no accumulator is set, so an interrupt means direct tick increment.
    else
    {
        internal_config[timebase_id].tick++;
    }
}
