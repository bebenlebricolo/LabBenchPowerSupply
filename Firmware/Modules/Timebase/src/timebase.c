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

#include <stdbool.h>
#include <stddef.h>
#include <limits.h>

#include "config.h"
#include "timebase.h"
#include "timebase_internal.h"

#include "timer_8_bit.h"
#include "timer_16_bit.h"
#include "timer_8_bit_async.h"

#ifndef TIMEBASE_MAX_MODULES
    #error "TIMEBASE_MAX_MODULES define is missing, please set the maximum number of available timebase modules in your config.h"
#endif

timebase_internal_config_t timebase_internal_config[TIMEBASE_MAX_MODULES] = {0};

static inline bool is_index_valid(const uint8_t id)
{
    bool out = true;
    if (id >= TIMEBASE_MAX_MODULES)
    {
        out = false;
    }
    return out;
}

static void reset_internal_config(const uint8_t id)
{
    timebase_internal_config[id].accumulator.programmed = 0;
    timebase_internal_config[id].accumulator.running = 0;
    timebase_internal_config[id].tick = 0;
    timebase_internal_config[id].timer = TIMEBASE_TIMER_UNDEFINED;
    timebase_internal_config[id].timer_id = 0;
    timebase_internal_config[id].initialised = false;
}

static inline timebase_error_t setup_8_bit_timer(const uint8_t timebase_id, uint32_t const * const cpu_freq, uint32_t const * const target_freq)
{
    bool initialised = false;
    timer_error_t err = timer_8_bit_is_initialised(timebase_internal_config[timebase_id].timer_id, &initialised);
    if (TIMER_ERROR_OK != err)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    if(false == initialised)
    {
        return TIMEBASE_ERROR_TIMER_UNINITIALISED;
    }

    uint8_t ocra = 0;
    timebase_internal_config[timebase_id].accumulator.programmed =  0;
    timer_8_bit_prescaler_selection_t prescaler;
    timer_8_bit_compute_matching_parameters(cpu_freq,
                                            target_freq,
                                            &prescaler,
                                            &ocra,
                                            &timebase_internal_config[timebase_id].accumulator.programmed);

    timer_error_t ret = timer_8_bit_stop(timebase_internal_config[timebase_id].timer_id);
    timer_8_bit_handle_t handle = {0};
    ret = timer_8_bit_get_handle(timebase_internal_config[timebase_id].timer_id, &handle);

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

    ret = timer_8_bit_reconfigure(timebase_internal_config[timebase_id].timer_id, &config);
    if (TIMER_ERROR_OK != ret)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    return TIMEBASE_ERROR_OK;
}

static inline timebase_error_t setup_8_bit_async_timer(const uint8_t timebase_id, uint32_t const * const cpu_freq, uint32_t const * const target_freq)
{
    bool initialised = false;
    timer_error_t err = timer_8_bit_async_is_initialised(timebase_internal_config[timebase_id].timer_id, &initialised);
    if (TIMER_ERROR_OK != err)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    if(false == initialised)
    {
        return TIMEBASE_ERROR_TIMER_UNINITIALISED;
    }

    uint8_t ocra = 0;
    timebase_internal_config[timebase_id].accumulator.programmed =  0;
    timer_8_bit_async_prescaler_selection_t prescaler;
    timer_8_bit_async_compute_matching_parameters(cpu_freq,
                                                  target_freq,
                                                  &prescaler,
                                                  &ocra,
                                                  &timebase_internal_config[timebase_id].accumulator.programmed);

    timer_error_t ret = timer_8_bit_async_stop(timebase_internal_config[timebase_id].timer_id);
    timer_8_bit_async_handle_t handle = {0};
    ret = timer_8_bit_async_get_handle(timebase_internal_config[timebase_id].timer_id, &handle);

    if (TIMER_ERROR_OK != ret)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    timer_8_bit_async_config_t config = {0};
    ret = timer_8_bit_async_get_default_config(&config);
    if (TIMER_ERROR_OK != ret)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    // Use old handle
    config.handle = handle;
    config.timing_config.comp_match_a = TIMER8BIT_ASYNC_CMOD_CLEAR_OCnX;
    config.timing_config.comp_match_b = TIMER8BIT_ASYNC_CMOD_NORMAL;
    config.timing_config.ocra_val = ocra;
    config.timing_config.prescaler = prescaler;
    config.interrupt_config.it_comp_match_a = true;

    ret = timer_8_bit_async_reconfigure(timebase_internal_config[timebase_id].timer_id, &config);
    if (TIMER_ERROR_OK != ret)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    return TIMEBASE_ERROR_OK;
}

static inline timebase_error_t setup_16_bit_timer(const uint8_t timebase_id, uint32_t const * const cpu_freq, uint32_t const * const target_freq)
{
    bool initialised = false;
    timer_error_t err = timer_16_bit_is_initialised(timebase_internal_config[timebase_id].timer_id, &initialised);
    if (TIMER_ERROR_OK != err)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    if(false == initialised)
    {
        return TIMEBASE_ERROR_TIMER_UNINITIALISED;
    }

    uint16_t ocra = 0;
    timebase_internal_config[timebase_id].accumulator.programmed =  0;
    timer_16_bit_prescaler_selection_t prescaler;
    timer_16_bit_compute_matching_parameters(cpu_freq,
                                            target_freq,
                                            &prescaler,
                                            &ocra,
                                            &timebase_internal_config[timebase_id].accumulator.programmed);

    timer_error_t ret = timer_16_bit_stop(timebase_internal_config[timebase_id].timer_id);
    timer_16_bit_handle_t handle = {0};
    ret = timer_16_bit_get_handle(timebase_internal_config[timebase_id].timer_id, &handle);

    if (TIMER_ERROR_OK != ret)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    timer_16_bit_config_t config = {0};
    ret = timer_16_bit_get_default_config(&config);
    if (TIMER_ERROR_OK != ret)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

    // Use old handle
    config.handle = handle;
    config.timing_config.comp_match_a = TIMER16BIT_CMOD_CLEAR_OCnX;
    config.timing_config.comp_match_b = TIMER16BIT_CMOD_NORMAL;
    config.timing_config.ocra_val = ocra;
    config.timing_config.prescaler = prescaler;
    config.interrupt_config.it_comp_match_a = true;

    ret = timer_16_bit_reconfigure(timebase_internal_config[timebase_id].timer_id, &config);
    if (TIMER_ERROR_OK != ret)
    {
        return TIMEBASE_ERROR_TIMER_ERROR;
    }

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

timebase_error_t timebase_compute_timer_parameters(timebase_config_t const * const config, uint16_t * const prescaler_val, uint16_t * const ocr_value, uint16_t * const accumulator)
{
    timer_8_bit_prescaler_selection_t prescaler;
    uint32_t target_frequency = 0;
    if (NULL == config || NULL == prescaler_val || NULL == ocr_value || NULL == accumulator)
    {
        return TIMEBASE_ERROR_NULL_POINTER;
    }

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

    timebase_internal_config[timebase_id].timer_id = config->timer.index;
    timebase_internal_config[timebase_id].timer = config->timer.type;
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

    timebase_internal_config[timebase_id].initialised = true;
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
    if (timebase_internal_config[timebase_id].accumulator.programmed != 0)
    {
        if (timebase_internal_config[timebase_id].accumulator.running < timebase_internal_config[timebase_id].accumulator.programmed)
        {
            timebase_internal_config[timebase_id].accumulator.running++;
        }
        else
        {
            timebase_internal_config[timebase_id].accumulator.running = 0;
            timebase_internal_config[timebase_id].tick++;
        }
    }
    // Otherwise, no accumulator is set, so an interrupt means direct tick increment.
    else
    {
        timebase_internal_config[timebase_id].tick++;
    }
}

timebase_error_t timebase_deinit(const uint8_t id)
{
    if (false == is_index_valid(id))
    {
        return TIMEBASE_ERROR_INVALID_INDEX;
    }

    if (false == timebase_internal_config[id].initialised)
    {
        return TIMEBASE_ERROR_UNINITIALISED;
    }

    reset_internal_config(id);
    return TIMEBASE_ERROR_OK;
}


timebase_error_t timebase_get_tick(const uint8_t id, uint16_t * const tick)
{
    if (false == is_index_valid(id))
    {
        return TIMEBASE_ERROR_INVALID_INDEX;
    }

    if (NULL == tick)
    {
        return TIMEBASE_ERROR_NULL_POINTER;
    }

    if (false == timebase_internal_config[id].initialised)
    {
        return TIMEBASE_ERROR_UNINITIALISED;
    }

    *tick = timebase_internal_config[id].tick;

    return TIMEBASE_ERROR_OK;
}

timebase_error_t timebase_get_duration(uint16_t const * const reference, uint16_t const * const new_tick, uint16_t * const duration)
{
    if( NULL == reference || NULL == new_tick || NULL == duration)
    {
        return TIMEBASE_ERROR_NULL_POINTER;
    }

    // Tick counter has overflowed ! As there is no particular event triggered by the overflowing of
    // any tick variable, we can assume that overflowing happened only once and compute the absolute distance
    // between the two variables
    if (*reference > *new_tick)
    {
        uint32_t offset_tick = *new_tick + USHRT_MAX;
        *duration = (offset_tick - (uint32_t) *reference);
    }
    else
    {
        *duration = *new_tick - *reference;
    }

    return TIMEBASE_ERROR_OK;
}

timebase_error_t timebase_get_duration_now(const uint8_t id, uint16_t const * const reference, uint16_t * const duration)
{
    if (false == is_index_valid(id))
    {
        return TIMEBASE_ERROR_INVALID_INDEX;
    }

    if (NULL == reference || NULL == duration)
    {
        return TIMEBASE_ERROR_NULL_POINTER;
    }

    uint16_t now = 0;
    timebase_error_t err = timebase_get_tick(id, &now);
    if (TIMEBASE_ERROR_OK != err)
    {
        return err;
    }

    return timebase_get_duration(reference, &now, duration);
}

timebase_error_t timebase_is_initialised(const uint8_t id, bool * const initialised)
{
    if (false == is_index_valid(id))
    {
        return TIMEBASE_ERROR_INVALID_INDEX;
    }

    if (NULL == initialised)
    {
        return TIMEBASE_ERROR_NULL_POINTER;
    }

    *initialised = timebase_internal_config[id].initialised;
    return TIMEBASE_ERROR_OK;
}
