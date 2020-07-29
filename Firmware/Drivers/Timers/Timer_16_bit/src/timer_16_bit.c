#include "config.h"
#include "timer_16_bit.h"

#include <stddef.h>
#include <string.h>

#ifndef TIMER_16_BIT_COUNT
    #error "TIMER_16_BIT_COUNT is not defined. Please add #define TIMER_16_BIT_COUNT n in config.h to use this timer"
#elif TIMER_16_BIT_COUNT == 0
    #warning "TIMER_16_BIT_COUNT is set to 0. If you don't project to use this timer, refer to not compile this file instead of setting this define to 0"
#endif

static struct
{
    timer_16_bit_handle_t handle;
    timer_16_bit_prescaler_selection_t prescaler;
    bool is_initialised;
} internal_config[TIMER_16_BIT_COUNT] = {0};

const timer_16_bit_prescaler_value_pair_t timer_16_bit_prescaler_table[TIMER_16_BIT_MAX_PRESCALER] =
{
    {.value = 1, .key = TIMER16BIT_CLK_PRESCALER_1},
    {.value = 8, .key = TIMER16BIT_CLK_PRESCALER_8},
    {.value = 64, .key = TIMER16BIT_CLK_PRESCALER_64},
    {.value = 256, .key = TIMER16BIT_CLK_PRESCALER_256},
    {.value = 1024, .key = TIMER16BIT_CLK_PRESCALER_1024},
};

static inline timer_error_t check_handle(timer_16_bit_handle_t * const handle)
{
    bool found_null = false;
    if (NULL == handle)
    {
        /* Not the use case we really want to check, but this is a case of error anyway
        which will generate segfaults errors if we let it propagate further ... */
        found_null = true;
    }
    else
    {
        found_null |= (NULL == handle->TCCRA);
        found_null |= (NULL == handle->TCCRB);
        found_null |= (NULL == handle->TCCRC);

        found_null |= (NULL == handle->OCRA_H);
        found_null |= (NULL == handle->OCRA_L);
        found_null |= (NULL == handle->OCRB_H);
        found_null |= (NULL == handle->OCRB_L);

        found_null |= (NULL == handle->TCNT_H);
        found_null |= (NULL == handle->TCNT_L);

        found_null |= (NULL == handle->ICR_H);
        found_null |= (NULL == handle->ICR_L);

        found_null |= (NULL == handle->TIMSK);
        found_null |= (NULL == handle->TIFR);
    }
    if (found_null)
    {
        return TIMER_ERROR_NULL_HANDLE;
    }
    return TIMER_ERROR_OK;;
}

static inline timer_error_t check_id(uint8_t id)
{
    if (id >= TIMER_16_BIT_COUNT)
    {
        return TIMER_ERROR_UNKNOWN_TIMER;
    }
    return TIMER_ERROR_OK;
}

timer_error_t timer_16_bit_set_handle(uint8_t id, timer_16_bit_handle_t * const handle)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (NULL == handle)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    memcpy(&internal_config[id].handle, handle, sizeof(timer_16_bit_handle_t));
    return ret;
}

timer_error_t timer_16_bit_get_default_config(timer_16_bit_config_t * config)
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
    config->interrupt_config.it_input_capture = false ;

    config->timing_config.counter = 0U;
    config->timing_config.ocra_val = 0U;
    config->timing_config.ocrb_val = 0U;
    config->timing_config.prescaler = TIMER16BIT_CLK_NO_CLOCK;
    config->timing_config.waveform_mode = TIMER16BIT_WG_NORMAL;
    config->timing_config.comp_match_a = TIMER16BIT_CMOD_NORMAL;
    config->timing_config.comp_match_b = TIMER16BIT_CMOD_NORMAL;

    config->force_compare.force_comp_match_a = false;
    config->force_compare.force_comp_match_b = false;
    config->input_capture.edge_select = TIMER16BIT_INPUT_CAPTURE_EDGE_FALLING_EDGE;
    config->input_capture.use_noise_canceler = false;

    /* Architecture and device dependent, must be set at configuration time */
    config->handle.OCRA_H = NULL;
    config->handle.OCRA_L = NULL;
    config->handle.OCRB_H = NULL;
    config->handle.OCRB_L = NULL;
    config->handle.TCCRA = NULL;
    config->handle.TCCRB = NULL;
    config->handle.TCCRC = NULL;
    config->handle.TCNT_H = NULL;
    config->handle.TCNT_L = NULL;
    config->handle.ICR_H = NULL;
    config->handle.ICR_L = NULL;
    config->handle.TIFR = NULL;
    config->handle.TIMSK = NULL;
    return ret;
}

timer_error_t timer_16_bit_set_force_compare_config(uint8_t id, timer_16_bit_force_compare_config_t * const force_comp_config)
{
    timer_error_t ret = check_id(id);

    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }
    if (NULL == force_comp_config)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    /* Not fully configured handle, do not attempt to write to it until configured !*/
    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    /* Handles force output compare A flags */
    if (true == force_comp_config->force_comp_match_a)
    {
        *(internal_config[id].handle.TCCRC) |= FOCA_MSK ;
    }
    else
    {
        *(internal_config[id].handle.TCCRC) &=  ~FOCA_MSK ;
    }

    /* Handles force output compare A flags */
    if (true == force_comp_config->force_comp_match_b)
    {
        *(internal_config[id].handle.TCCRC) |= FOCB_MSK ;
    }
    else
    {
        *(internal_config[id].handle.TCCRC) &=  ~FOCB_MSK ;
    }
    return ret;
}

timer_error_t timer_16_bit_get_force_compare_config(uint8_t id, timer_16_bit_force_compare_config_t * force_comp_config)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }
    if (NULL == force_comp_config)
    {
        return TIMER_ERROR_NULL_POINTER;
    }
    /* Not fully configured handle, do not attempt to write to it until configured !*/
    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    /* Handles Force Compare Output A flag */
    if (0U == (*(internal_config[id].handle.TCCRC) & FOCA_MSK))
    {
        force_comp_config->force_comp_match_a = false;
    }
    else
    {
        force_comp_config->force_comp_match_a = true;
    }

    /* Handles Force Compare Output B flag */
    if (0U == (*(internal_config[id].handle.TCCRC) & FOCB_MSK))
    {
        force_comp_config->force_comp_match_b = false;
    }
    else
    {
        force_comp_config->force_comp_match_b = true;
    }
    return ret;
}

timer_error_t timer_16_bit_set_interrupt_config(uint8_t id, timer_16_bit_interrupt_config_t * const it_config)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (NULL == it_config)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    /* Input capture interrupt enable bit */
    if (true == it_config->it_input_capture)
    {
        *(internal_config[id].handle.TIMSK) |= ICIE_MSK;
    }
    else
    {
        *(internal_config[id].handle.TIMSK) &= ~ICIE_MSK;
    }

    /* TIMSK register */
    if (true == it_config->it_comp_match_a)
    {
        *(internal_config[id].handle.TIMSK) |= OCIEA_MSK;
    }
    else
    {
        *(internal_config[id].handle.TIMSK) &= ~OCIEA_MSK;
    }

    if (true == it_config->it_comp_match_b)
    {
        *(internal_config[id].handle.TIMSK) |= OCIEB_MSK;
    }
    else
    {
        *(internal_config[id].handle.TIMSK) &= ~OCIEB_MSK;
    }

    /* TOIE interrupt flag is the first bit, no need to bitshift it */
    if (true == it_config->it_timer_overflow)
    {
        *(internal_config[id].handle.TIMSK) |= TOIE_MSK;
    }
    else
    {
        *(internal_config[id].handle.TIMSK) &= ~TOIE_MSK;
    }

    return ret;
}


timer_error_t timer_16_bit_get_interrupt_config(uint8_t id, timer_16_bit_interrupt_config_t * it_config)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (NULL == it_config)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    /* Input capture interrupt enable flag */
    if (0U == (*(internal_config[id].handle.TIMSK) & ICIE_MSK))
    {
        it_config->it_input_capture = false;
    }
    else
    {
        it_config->it_input_capture = true;
    }

    /* Output Compare Match A Interrupt Flag */
    if (0U == (*(internal_config[id].handle.TIMSK) & OCIEA_MSK))
    {
        it_config->it_comp_match_a = false;
    }
    else
    {
        it_config->it_comp_match_a = true;
    }

    /* Output Compare Match B Interrupt Flag */
    if (0U == (*(internal_config[id].handle.TIMSK) & OCIEB_MSK))
    {
        it_config->it_comp_match_b = false;
    }
    else
    {
        it_config->it_comp_match_b = true;
    }

    /* Timer Overflow Interrupt Flag */
    if (0U == (*(internal_config[id].handle.TIMSK) & TOV_MSK))
    {
        it_config->it_timer_overflow = false;
    }
    else
    {
        it_config->it_timer_overflow = true;
    }
    return ret;
}

#ifdef UNIT_TESTING
timer_error_t timer_16_bit_get_interrupt_flags(uint8_t id, timer_16_bit_interrupt_config_t * it_flags)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (NULL == it_flags)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    /* Input capture interrupt flag  */
    if (0U == (*(internal_config[id].handle.TIFR) & ICF_MSK))
    {
        it_flags->it_input_capture = false;
    }
    else
    {
        it_flags->it_input_capture = true;
    }

    /* Output Compare Match A Interrupt Flag */
    if (0U == (*(internal_config[id].handle.TIFR) & OCIEA_MSK))
    {
        it_flags->it_comp_match_a = false;
    }
    else
    {
        it_flags->it_comp_match_a = true;
    }

    /* Output Compare Match B Interrupt Flag */
    if (0U == (*(internal_config[id].handle.TIFR) & OCIEB_MSK))
    {
        it_flags->it_comp_match_b = false;
    }
    else
    {
        it_flags->it_comp_match_b = true;
    }

    /* Timer Overflow Interrupt Flag */
    if (0U == (*(internal_config[id].handle.TIFR) & TOV_MSK))
    {
        it_flags->it_timer_overflow = false;
    }
    else
    {
        it_flags->it_timer_overflow = true;
    }

    return ret;

}
#endif



timer_error_t timer_16_bit_set_prescaler(uint8_t id, const timer_16_bit_prescaler_selection_t prescaler)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *(internal_config[id].handle.TCCRB) = (*(internal_config[id].handle.TCCRB) & ~CS_MSK) | prescaler;
    return ret;
}

timer_error_t timer_16_bit_get_prescaler(uint8_t id, timer_16_bit_prescaler_selection_t * prescaler)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (NULL == prescaler)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *prescaler = (*(internal_config[id].handle.TCCRB) & CS_MSK);
    return ret;
}

timer_error_t timer_16_bit_set_compare_match_A(uint8_t id, const timer_16_bit_compare_output_mode_t compA)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *(internal_config[id].handle.TCCRA) = (*(internal_config[id].handle.TCCRA) & ~COMA_MSK) | (compA << COMA0_BIT);
    return ret;
}

timer_error_t timer_16_bit_get_compare_match_A(uint8_t id, timer_16_bit_compare_output_mode_t * compA)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (NULL == compA)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *compA = ((*(internal_config[id].handle.TCCRA) & COMA_MSK) >> COMA0_BIT);
    return ret;
}

timer_error_t timer_16_bit_set_compare_match_B(uint8_t id, timer_16_bit_compare_output_mode_t compB)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *(internal_config[id].handle.TCCRA) = (*(internal_config[id].handle.TCCRA) & ~COMB_MSK) | (compB << COMB0_BIT);
    return ret;
}

timer_error_t timer_16_bit_get_compare_match_B(uint8_t id, timer_16_bit_compare_output_mode_t * compB)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (NULL == compB)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *compB = ((*(internal_config[id].handle.TCCRA) & COMB_MSK) >> COMB0_BIT);
    return ret;
}

timer_error_t timer_16_bit_set_waveform_generation(uint8_t id, const timer_16_bit_waveform_generation_t waveform)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *(internal_config[id].handle.TCCRA) = (*(internal_config[id].handle.TCCRA) & ~(WGM0_MSK | WGM1_MSK)) | (waveform & (WGM0_MSK | WGM1_MSK));
    /* Select bit index 2 and 3 (0x3 -> 0b11) of waveform mode (matches datasheet bit mapping) and store it to bit index 3 of TCCRB with one more bitshift */
    *(internal_config[id].handle.TCCRB) = (*(internal_config[id].handle.TCCRB) & ~(WGM2_MSK | WGM3_MSK)) | ((waveform & 0x0C) << 1U);
    return ret;
}


timer_error_t timer_16_bit_set_input_compare_noise_canceler(uint8_t id, const bool enabled)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (enabled)
    {
        *(internal_config[id].handle.TCCRB) |= INC_MSK;
    }
    else
    {
        *(internal_config[id].handle.TCCRB) &= ~INC_MSK;
    }

    return ret;
}

timer_error_t timer_16_bit_get_input_compare_noise_canceler(uint8_t id, bool * const enabled)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (NULL == enabled)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *enabled = ((*(internal_config[id].handle.TCCRB) & INC_MSK) != 0U);
    return ret;
}

timer_error_t timer_16_bit_set_input_compare_edge_select(uint8_t id, const timer_16_bit_input_capture_edge_select_flag_t edge)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *(internal_config[id].handle.TCCRB) =  (*(internal_config[id].handle.TCCRB) & ~ICES_MSK) | (edge << ICES_BIT);
    return ret;
}

timer_error_t timer_16_bit_get_input_compare_edge_select(uint8_t id, timer_16_bit_input_capture_edge_select_flag_t * const edge)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (NULL == edge)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (0 !=  (*(internal_config[id].handle.TCCRB) & ICES_MSK))
    {
        *edge = TIMER16BIT_INPUT_CAPTURE_EDGE_RISING_EDGE;
    }
    else
    {
        *edge = TIMER16BIT_INPUT_CAPTURE_EDGE_FALLING_EDGE;
    }
    return ret;
}

timer_error_t timer_16_bit_get_input_capture_value(uint8_t id, uint16_t * ticks)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (NULL == ticks)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *ticks  = *(internal_config[id].handle.TCNT_L);
    *ticks |= *(internal_config[id].handle.TCNT_H) << 8U;
    return ret;
}


timer_error_t timer_16_bit_get_waveform_generation(uint8_t id, timer_16_bit_waveform_generation_t * waveform)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (NULL == waveform)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *waveform = (timer_16_bit_waveform_generation_t)(0U);
    *waveform |= (*(internal_config[id].handle.TCCRA) & (WGM0_MSK | WGM1_MSK));
    *waveform |= (*(internal_config[id].handle.TCCRB) & (WGM2_MSK | WGM3_MSK)) >> 1U;
    return ret;
}

timer_error_t timer_16_bit_set_counter_value(uint8_t id, const uint16_t * const ticks)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    /* Write new value to internal timer/counter register */
    *(internal_config[id].handle.TCNT_L) = *ticks & 0xFF;
    *(internal_config[id].handle.TCNT_H) = (*ticks & 0xFF00) >> 8U;

    return ret;
}

timer_error_t timer_16_bit_get_counter_value(uint8_t id, uint16_t * const ticks)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if ( NULL == ticks)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    /* Transfer data from internal device's timer/count main register */
    *ticks  = (*internal_config[id].handle.TCNT_L);
    *ticks |= (*internal_config[id].handle.TCNT_H) << 8U;
    return ret;
}

timer_error_t timer_16_bit_set_ocra_register_value(uint8_t id, const uint16_t * const ocra)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *(internal_config[id].handle.OCRA_H) = (*ocra & 0xFF00) >> 8U;
    *(internal_config[id].handle.OCRA_L) = (*ocra & 0xFF);
    return ret;
}

timer_error_t timer_16_bit_get_ocra_register_value(uint8_t id, uint16_t * const ocra)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if ( NULL == ocra)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *ocra  = (*internal_config[id].handle.OCRA_L);
    *ocra |= (*internal_config[id].handle.OCRA_H << 8U);
    return ret;
}

timer_error_t timer_16_bit_set_ocrb_register_value(uint8_t id, const uint16_t * const ocrb)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *internal_config[id].handle.OCRB_H = (*ocrb & 0xFF00) >> 8U;
    *internal_config[id].handle.OCRB_L = (*ocrb & 0xFF);;
    return ret;
}

timer_error_t timer_16_bit_get_ocrb_register_value(uint8_t id, uint16_t * const ocrb)
{
    timer_error_t ret = check_id(id);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if ( NULL == ocrb)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    *ocrb =  (*internal_config[id].handle.OCRB_L);
    *ocrb |= (*internal_config[id].handle.OCRB_H << 8U);
    return ret;
}

static timer_error_t timer_16_bit_write_config(uint8_t id, timer_16_bit_config_t * const config)
{
    timer_error_t ret = TIMER_ERROR_OK;
    timer_16_bit_handle_t * handle = &internal_config[id].handle;

    internal_config[id].prescaler = config->timing_config.prescaler;

    /* Clear all interrupts */
    *(handle->TIFR) = 0U;

    /* Initialise counter as well */
    *(handle->TCNT_H) = (config->timing_config.counter & 0xFF00) >> 8U;
    *(handle->TCNT_L) = (config->timing_config.counter & 0xFF);

    /* TCCRA register */
    *(handle->OCRA_H) = (config->timing_config.ocra_val & 0xFF00) >> 8U;
    *(handle->OCRA_L) = (config->timing_config.ocra_val & 0xFF);
    *(handle->OCRB_H) = (config->timing_config.ocrb_val & 0xFF00) >> 8U;
    *(handle->OCRB_L) = (config->timing_config.ocrb_val & 0xFF);
    *(handle->TCCRA) = (*(handle->TCCRA) & ~COMA_MSK) | (config->timing_config.comp_match_a << COMA0_BIT);
    *(handle->TCCRA) = (*(handle->TCCRA) & ~COMB_MSK) | (config->timing_config.comp_match_b << COMB0_BIT);
    *(handle->TCCRA) = (*(handle->TCCRA) & ~(WGM0_MSK | WGM1_MSK)) | (config->timing_config.waveform_mode & (WGM0_MSK | WGM1_MSK));

    /* TCCRB register */
    /* Select bit index 2 of waveform mode (matches datasheet bit mapping) and store it to bit index 3 of TCCRB with one more bitshift */
    *(handle->TCCRB) = (*(handle->TCCRB) & ~(WGM2_MSK | WGM3_MSK)) | ((config->timing_config.waveform_mode & (0x3U << 2U)) << 1U);
    *(handle->TCCRB) = (*(handle->TCCRB) & ~ICES_MSK) | (config->input_capture.edge_select << ICES_BIT);

    if(true == config->input_capture.use_noise_canceler)
    {
        *(handle->TCCRB) |= INC_MSK;
    }
    else
    {
        *(handle->TCCRB) &= ~INC_MSK;
    }

    /* Handles force output compare A flags */
    if (true == config->force_compare.force_comp_match_a)
    {
        *(handle->TCCRC) |= FOCA_MSK;
    }
    else
    {
        *(handle->TCCRC) &=  ~FOCA_MSK;
    }

    /* Handles force output compare A flags */
    if (true == config->force_compare.force_comp_match_b)
    {
        *(handle->TCCRC) |= FOCB_MSK ;
    }
    else
    {
        *(handle->TCCRC) &=  ~FOCB_MSK ;
    }

    /* NOTE : Do not handle prescaler until timer is manually started using timer_16_bit_start(id)*/

    /* TIMSK register */
    if (true == config->interrupt_config.it_comp_match_a)
    {
        *(handle->TIMSK) |= OCIEA_MSK;
    }
    else
    {
        *(handle->TIMSK) &= ~OCIEA_MSK;
    }

    if (true == config->interrupt_config.it_comp_match_b)
    {
        *(handle->TIMSK) |= OCIEB_MSK;
    }
    else
    {
        *(handle->TIMSK) &= ~OCIEB_MSK;
    }

    /* TOIE interrupt flag is the first bit, no need to bitshift it */
    if (true == config->interrupt_config.it_timer_overflow)
    {
        *(handle->TIMSK) |= TOV_MSK;
    }
    else
    {
        *(handle->TIMSK) &= ~TOV_MSK;
    }
    return ret;
}

timer_error_t timer_16_bit_init(uint8_t id, timer_16_bit_config_t * const config)
{
    timer_error_t ret = check_id(id);
    if(TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = check_handle(&config->handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = timer_16_bit_set_handle(id, &config->handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = timer_16_bit_write_config(id,config);
    if (TIMER_ERROR_OK == ret)
    {
        internal_config[id].is_initialised = true;
    }
    return ret;
}


timer_error_t timer_16_bit_deinit(uint8_t id)
{
    timer_error_t ret = check_id(id);
    if(TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    /* Retrieve a config object to write back default configuration into timer registers */
    timer_16_bit_config_t config;
    ret = timer_16_bit_stop(id);
    if (TIMER_ERROR_OK == ret)
    {
        ret = timer_16_bit_get_default_config(&config);
    }
    if (TIMER_ERROR_OK == ret)
    {
        ret = timer_16_bit_write_config(id, &config);
        if (TIMER_ERROR_OK == ret)
        {
            internal_config[id].is_initialised = false;
        }
    }
    return ret;
}


timer_error_t timer_16_bit_start(uint8_t id)
{
    timer_error_t ret = check_id(id);
    if(TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    ret = check_handle(&internal_config[id].handle);
    if (TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (false == internal_config[id].is_initialised)
    {
        return TIMER_ERROR_NOT_INITIALISED;
    }

    /* This time, set the prescaler to start the timer, unless prescaler is set to NO_CLOCK source */
    *(internal_config[id].handle.TCCRB) = (*(internal_config[id].handle.TCCRB) & ~CS_MSK) | internal_config[id].prescaler;
    return ret;
}

timer_error_t timer_16_bit_stop(uint8_t id)
{
    timer_error_t ret = check_id(id);
    if(TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (false == internal_config[id].is_initialised)
    {
        return TIMER_ERROR_NOT_INITIALISED;
    }

    /* Reset prescaler to NO_CLOCK*/
    *(internal_config[id].handle.TCCRB) = (*(internal_config[id].handle.TCCRB) & ~CS_MSK) | TIMER16BIT_CLK_NO_CLOCK;
    return ret;
}

timer_error_t timer_16_bit_is_initialised(uint8_t id, bool * const initialised)
{
    timer_error_t ret = check_id(id);
    if(TIMER_ERROR_OK != ret)
    {
        return ret;
    }

    if (NULL == initialised)
    {
        return TIMER_ERROR_NULL_POINTER;
    }

    *initialised = internal_config[id].is_initialised;

    return ret;
}

