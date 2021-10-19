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

#include "timer_8_bit_async_stub.h"
#include "string.h"

typedef struct
{
    timer_8_bit_async_prescaler_selection_t prescaler;
    uint8_t ocra;
    uint32_t accumulator;
    bool initialised;
} configuration_t;

static configuration_t configuration = {0};

static inline bool id_is_valid(const uint8_t id)
{
    return (id < TIMER_8_BIT_ASYNC_STUB_MAX_INSTANCES);
}

void timer_8_bit_async_stub_set_next_parameters(const timer_8_bit_async_prescaler_selection_t prescaler, const uint8_t ocra, const uint32_t accumulator)
{
    configuration.prescaler = prescaler;
    configuration.ocra = ocra;
    configuration.accumulator = accumulator;
}

void timer_8_bit_async_stub_set_initialised(const bool initialised)
{
    configuration.initialised = initialised;
}

void timer_8_bit_async_stub_reset(void)
{
    memset(&configuration, 0, sizeof(configuration_t));
}

void timer_8_bit_async_compute_matching_parameters( const uint32_t * const cpu_freq,
                                                    const uint32_t * const target_freq,
                                                    timer_8_bit_async_prescaler_selection_t * const prescaler,
                                                    uint8_t * const ocra,
                                                    uint16_t * const accumulator)
{
    (void) cpu_freq;
    (void) target_freq;
    *prescaler = configuration.prescaler;
    *ocra = configuration.ocra;
    *accumulator = configuration.accumulator;
}

const timer_generic_prescaler_pair_t timer_8_bit_async_prescaler_table[TIMER_8_BIT_ASYNC_MAX_PRESCALER_COUNT] =
{
    {.value = 1,    .type = (uint8_t) TIMER8BIT_ASYNC_CLK_PRESCALER_1      },
    {.value = 8,    .type = (uint8_t) TIMER8BIT_ASYNC_CLK_PRESCALER_8      },
    {.value = 32,   .type = (uint8_t) TIMER8BIT_ASYNC_CLK_PRESCALER_32     },
    {.value = 64,   .type = (uint8_t) TIMER8BIT_ASYNC_CLK_PRESCALER_64     },
    {.value = 128,  .type = (uint8_t) TIMER8BIT_ASYNC_CLK_PRESCALER_128    },
    {.value = 256,  .type = (uint8_t) TIMER8BIT_ASYNC_CLK_PRESCALER_256    },
    {.value = 1024, .type = (uint8_t) TIMER8BIT_ASYNC_CLK_PRESCALER_1024   },
};

timer_8_bit_async_prescaler_selection_t timer_8_bit_async_prescaler_from_value(uint16_t const * const input_prescaler)
{
    for (uint8_t i = 0 ; i < TIMER_8_BIT_ASYNC_MAX_PRESCALER_COUNT ; i++)
    {
        if (*input_prescaler == timer_8_bit_async_prescaler_table[i].value)
        {
            return (timer_8_bit_async_prescaler_selection_t) timer_8_bit_async_prescaler_table[i].type;
        }
    }
    return TIMER8BIT_ASYNC_CLK_NO_CLOCK;
}

uint16_t timer_8_bit_async_prescaler_to_value(const timer_8_bit_async_prescaler_selection_t prescaler)
{
    for (uint8_t i = 0 ; i < TIMER_8_BIT_ASYNC_MAX_PRESCALER_COUNT ; i++)
    {
        if (prescaler == timer_8_bit_async_prescaler_table[i].type)
        {
            return timer_8_bit_async_prescaler_table[i].value;
        }
    }
    return 0;
}

timer_error_t timer_8_bit_async_get_default_config(timer_8_bit_async_config_t * config)
{
    timer_error_t ret = TIMER_ERROR_OK;
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (NULL == config)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    /* Resets everything */
    config->interrupt_config.it_comp_match_a = false;
    config->interrupt_config.it_comp_match_b = false;
    config->interrupt_config.it_timer_overflow = false;

    config->timing_config.counter = 0U;
    config->timing_config.ocra_val = 0U;
    config->timing_config.ocrb_val = 0U;
    config->timing_config.prescaler = TIMER8BIT_ASYNC_CLK_NO_CLOCK;
    config->timing_config.waveform_mode = TIMER8BIT_ASYNC_WG_NORMAL;
    config->timing_config.comp_match_a = TIMER8BIT_ASYNC_CMOD_NORMAL;
    config->timing_config.comp_match_b = TIMER8BIT_ASYNC_CMOD_NORMAL;
    config->timing_config.clock_source = TIMER8BIT_ASYNC_CLK_SOURCE_INTERNAL;

    config->force_compare.force_comp_match_a = false;
    config->force_compare.force_comp_match_b = false;

    /* Architecture and device dependent, must be set at configuration time */
    config->handle.OCRA = NULL;
    config->handle.OCRB = NULL;
    config->handle.TCCRA = NULL;
    config->handle.TCCRB = NULL;
    config->handle.TCNT = NULL;
    config->handle.TIFR = NULL;
    config->handle.TIMSK = NULL;
    config->handle.ASSR_REG = NULL;
    return ret;
}

timer_error_t timer_8_bit_async_set_handle(uint8_t id, timer_8_bit_async_handle_t * const handle)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) handle;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_get_handle(uint8_t id, timer_8_bit_async_handle_t * const handle)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) handle;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_set_force_compare_config(uint8_t id, timer_8_bit_async_force_compare_config_t * const force_comp_config)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) force_comp_config;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_get_force_compare_config(uint8_t id, timer_8_bit_async_force_compare_config_t * force_comp_config)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) force_comp_config;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_set_interrupt_config(uint8_t id, timer_8_bit_async_interrupt_config_t * const it_config)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) it_config;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_get_interrupt_config(uint8_t id, timer_8_bit_async_interrupt_config_t * it_config)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) it_config;
    return TIMER_ERROR_OK;
}

#ifdef UNIT_TESTING
timer_error_t timer_8_bit_async_get_interrupt_flags(uint8_t id, timer_8_bit_async_interrupt_config_t * it_flags)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) it_flags;
    return TIMER_ERROR_OK;
}
#endif

timer_error_t timer_8_bit_async_set_prescaler(uint8_t id, const timer_8_bit_async_prescaler_selection_t prescaler)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) prescaler;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_get_prescaler(uint8_t id, timer_8_bit_async_prescaler_selection_t * prescaler)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) prescaler;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_set_compare_match_A(uint8_t id, const timer_8_bit_async_compare_output_mode_t compA)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) compA;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_get_compare_match_A(uint8_t id, timer_8_bit_async_compare_output_mode_t * compA)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) compA;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_set_compare_match_B(uint8_t id, timer_8_bit_async_compare_output_mode_t compB)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) compB;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_get_compare_match_B(uint8_t id, timer_8_bit_async_compare_output_mode_t * compB)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) compB;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_set_waveform_generation(uint8_t id, const timer_8_bit_async_waveform_generation_t waveform)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) waveform;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_get_waveform_generation(uint8_t id, timer_8_bit_async_waveform_generation_t * const waveform)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) waveform;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_set_counter_value(uint8_t id, const uint8_t ticks)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) ticks;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_get_counter_value(uint8_t id, uint8_t * ticks)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) ticks;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_set_ocra_register_value(uint8_t id, uint8_t ocra)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) ocra;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_get_ocra_register_value(uint8_t id, uint8_t * ocra)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) ocra;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_set_ocrb_register_value(uint8_t id, uint8_t ocrb)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) ocrb;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_get_ocrb_register_value(uint8_t id, uint8_t * ocrb)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) ocrb;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_init(uint8_t id, timer_8_bit_async_config_t * const config)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) config;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_reconfigure(uint8_t id, timer_8_bit_async_config_t * const config)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    (void) config;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_deinit(uint8_t id)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_start(uint8_t id)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_stop(uint8_t id)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_async_is_initialised(const uint8_t id, bool * const initialised)
{
    if (!id_is_valid(id))
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    };
    *initialised = configuration.initialised;
    return TIMER_ERROR_OK;
}