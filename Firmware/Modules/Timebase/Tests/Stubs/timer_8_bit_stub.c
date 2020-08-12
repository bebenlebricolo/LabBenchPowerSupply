#include "timer_8_bit_stub.h"


static struct
{
    timer_8_bit_prescaler_selection_t prescaler;
    uint8_t ocra;
    uint32_t accumulator;
} parameters = {0};

void timer_8_bit_stub_set_next_parameters(const timer_8_bit_prescaler_selection_t prescaler, const uint8_t ocra, uint32_t * const accumulator)
{
    parameters.prescaler = prescaler;
    parameters.ocra = ocra;
    parameters.accumulator = *accumulator;
}

void timer_8_bit_compute_matching_parameters(const uint32_t * const cpu_freq,
                                             const uint32_t * const target_freq,
                                             timer_8_bit_prescaler_selection_t * const prescaler,
                                             uint8_t * const ocra,
                                             uint32_t * const accumulator)
{
    (void) cpu_freq;
    (void) target_freq;
    *prescaler = parameters.prescaler;
    *ocra = parameters.ocra;
    *accumulator = parameters.accumulator;
}

uint16_t timer_8_bit_prescaler_to_value(const timer_8_bit_prescaler_selection_t prescaler)
{
    (void) prescaler;
    return 0U;
}

timer_8_bit_prescaler_selection_t timer_8_bit_prescaler_from_value(uint16_t const * const input_prescaler)
{
    (void) input_prescaler;
    return TIMER8BIT_CLK_NO_CLOCK;
}

timer_error_t timer_8_bit_get_default_config(timer_8_bit_config_t * config)
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
    config->timing_config.prescaler = TIMER8BIT_CLK_NO_CLOCK;
    config->timing_config.waveform_mode = TIMER8BIT_WG_NORMAL;
    config->timing_config.comp_match_a = TIMER8BIT_CMOD_NORMAL;
    config->timing_config.comp_match_b = TIMER8BIT_CMOD_NORMAL;

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
    return ret;
}

timer_error_t timer_8_bit_set_handle(uint8_t id, timer_8_bit_handle_t * const handle)
{
    (void) id;
    (void) handle;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_get_handle(uint8_t id, timer_8_bit_handle_t * const handle)
{
    (void) id;
    (void) handle;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_set_force_compare_config(uint8_t id, timer_8_bit_force_compare_config_t * const force_comp_config)
{
    (void) id;
    (void) force_comp_config;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_get_force_compare_config(uint8_t id, timer_8_bit_force_compare_config_t * force_comp_config)
{
    (void) id;
    (void) force_comp_config;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_set_interrupt_config(uint8_t id, timer_8_bit_interrupt_config_t * const it_config)
{
    (void) id;
    (void) it_config;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_get_interrupt_config(uint8_t id, timer_8_bit_interrupt_config_t * it_config)
{
    (void) id;
    (void) it_config;
    return TIMER_ERROR_OK;
}

#ifdef UNIT_TESTING
timer_error_t timer_8_bit_get_interrupt_flags(uint8_t id, timer_8_bit_interrupt_config_t * it_flags)
{
    (void) id;
    (void) it_flags;
    return TIMER_ERROR_OK;
}
#endif

timer_error_t timer_8_bit_set_prescaler(uint8_t id, const timer_8_bit_prescaler_selection_t prescaler)
{
    (void) id;
    (void) prescaler;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_get_prescaler(uint8_t id, timer_8_bit_prescaler_selection_t * prescaler)
{
    (void) id;
    (void) prescaler;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_set_compare_match_A(uint8_t id, const timer_8_bit_compare_output_mode_t compA)
{
    (void) id;
    (void) compA;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_get_compare_match_A(uint8_t id, timer_8_bit_compare_output_mode_t * compA)
{
    (void) id;
    (void) compA;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_set_compare_match_B(uint8_t id, timer_8_bit_compare_output_mode_t compB)
{
    (void) id;
    (void) compB;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_get_compare_match_B(uint8_t id, timer_8_bit_compare_output_mode_t * compB)
{
    (void) id;
    (void) compB;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_set_waveform_generation(uint8_t id, const timer_8_bit_waveform_generation_t waveform)
{
    (void) id;
    (void) waveform;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_get_waveform_generation(uint8_t id, timer_8_bit_waveform_generation_t * waveform)
{
    (void) id;
    (void) waveform;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_set_counter_value(uint8_t id, const uint8_t ticks)
{
    (void) id;
    (void) ticks;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_get_counter_value(uint8_t id, uint8_t * ticks)
{
    (void) id;
    (void) ticks;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_set_ocra_register_value(uint8_t id, uint8_t ocra)
{
    (void) id;
    (void) ocra;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_get_ocra_register_value(uint8_t id, uint8_t * ocra)
{
    (void) id;
    (void) ocra;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_set_ocrb_register_value(uint8_t id, uint8_t ocrb)
{
    (void) id;
    (void) ocrb;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_get_ocrb_register_value(uint8_t id, uint8_t * ocrb)
{
    (void) id;
    (void) ocrb;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_init(uint8_t id, timer_8_bit_config_t * const config)
{
    (void) id;
    (void) config;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_reconfigure(uint8_t id, timer_8_bit_config_t * const config)
{
    (void) id;
    (void) config;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_deinit(uint8_t id)
{
    (void) id;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_start(uint8_t id)
{
    (void) id;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_stop(uint8_t id)
{
    (void) id;
    return TIMER_ERROR_OK;
}

timer_error_t timer_8_bit_is_initialised(const uint8_t id, bool * const initialised)
{
    (void) id;
    (void) initialised;
    return TIMER_ERROR_OK;
}