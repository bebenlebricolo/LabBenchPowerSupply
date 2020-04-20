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
    timer_x_bit_prescaler_selection_t prescaler;
    bool is_initialised;
} internal_config[TIMER_16_BIT_COUNT] = {0};

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

    config->timing_config.counter = 0U;
    config->timing_config.ocra_val = 0U;
    config->timing_config.ocrb_val = 0U;
    config->timing_config.prescaler = TIMERxBIT_CLK_NO_CLOCK;
    config->timing_config.waveform_mode = TIMER16BIT_WG_NORMAL;
    config->timing_config.comp_match_a = TIMER16BIT_CMOD_NORMAL;
    config->timing_config.comp_match_b = TIMER16BIT_CMOD_NORMAL;

    config->force_compare.force_comp_match_a = false;
    config->force_compare.force_comp_match_b = false;

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
    config->handle.TIFR = NULL;
    config->handle.TIMSK = NULL;
    return ret;
}

timer_error_t timer_16_bit_set_force_compare_config(uint8_t id, timer_x_bit_force_compare_config_t * const force_comp_config)
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
        *(internal_config[id].handle.TCCRB) |= (1U << FOCA_BIT) ;
    }
    else
    {
        *(internal_config[id].handle.TCCRB) &=  ~(1U << FOCA_BIT) ;
    }

    /* Handles force output compare A flags */
    if (true == force_comp_config->force_comp_match_b)
    {
        *(internal_config[id].handle.TCCRB) |= (1U << FOCB_BIT) ;
    }
    else
    {
        *(internal_config[id].handle.TCCRB) &=  ~(1U << FOCB_BIT) ;
    }
    return ret;
}

timer_error_t timer_16_bit_get_force_compare_config(uint8_t id, timer_x_bit_force_compare_config_t * force_comp_config)
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
    if (0U == (*(internal_config[id].handle.TCCRB) & FOCA_MSK))
    {
        force_comp_config->force_comp_match_a = false;
    }
    else
    {
        force_comp_config->force_comp_match_a = true;
    }

    /* Handles Force Compare Output B flag */
    if (0U == (*(internal_config[id].handle.TCCRB) & FOCB_MSK))
    {
        force_comp_config->force_comp_match_b = false;
    }
    else
    {
        force_comp_config->force_comp_match_b = true;
    }
    return ret;
}

timer_error_t timer_16_bit_set_interrupt_config(uint8_t id, timer_8_bit_interrupt_config_t * const it_config)
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

    /* TIMSK register */
    if (true == it_config->it_comp_match_a)
    {
        *(internal_config[id].handle.TIMSK) |= 1U << OCIEA_BIT;
    }
    else
    {
        *(internal_config[id].handle.TIMSK) &= ~(1U << OCIEA_BIT);
    }

    if (true == it_config->it_comp_match_b)
    {
        *(internal_config[id].handle.TIMSK) |= 1U << OCIEB_BIT;
    }
    else
    {
        *(internal_config[id].handle.TIMSK) &= ~(1U << OCIEB_BIT);
    }

    /* TOIE interrupt flag is the first bit, no need to bitshift it */
    if (true == it_config->it_timer_overflow)
    {
        *(internal_config[id].handle.TIMSK) |= 1U;
    }
    else
    {
        *(internal_config[id].handle.TIMSK) &= ~1U;
    }
    return ret;
}


timer_error_t timer_16_bit_get_interrupt_config(uint8_t id, timer_8_bit_interrupt_config_t * it_config)
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
    if (0U == (*(internal_config[id].handle.TIMSK) & 1U))
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
timer_error_t timer_16_bit_get_interrupt_flags(uint8_t id, timer_8_bit_interrupt_config_t * it_flags)
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
    if (0U == (*(internal_config[id].handle.TIFR) & 1U))
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



timer_error_t timer_16_bit_set_prescaler(uint8_t id, const timer_x_bit_prescaler_selection_t prescaler)
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

timer_error_t timer_16_bit_get_prescaler(uint8_t id, timer_x_bit_prescaler_selection_t * prescaler)
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

timer_error_t timer_16_bit_set_compare_match_A(uint8_t id, const timer_8_bit_compare_output_mode_t compA)
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

    *(internal_config[id].handle.TCCRA) = (*(internal_config[id].handle.TCCRA) & ~COMA_MSK) | (compA << COMA_BIT);
    return ret;
}

timer_error_t timer_16_bit_get_compare_match_A(uint8_t id, timer_8_bit_compare_output_mode_t * compA)
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

    *compA = ((*(internal_config[id].handle.TCCRA) & COMA_MSK) >> COMA_BIT);
    return ret;
}

timer_error_t timer_16_bit_set_compare_match_B(uint8_t id, timer_8_bit_compare_output_mode_t compB)
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

    *(internal_config[id].handle.TCCRA) = (*(internal_config[id].handle.TCCRA) & ~COMB_MSK) | (compB << COMB_BIT);
    return ret;
}

timer_error_t timer_16_bit_get_compare_match_B(uint8_t id, timer_8_bit_compare_output_mode_t * compB)
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

    *compB = ((*(internal_config[id].handle.TCCRA) & COMB_MSK) >> COMB_BIT);
    return ret;
}

timer_error_t timer_16_bit_set_waveform_generation(uint8_t id, const timer_8_bit_waveform_generation_t waveform)
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
    /* Select bit index 2 of waveform mode (matches datasheet bit mapping) and store it to bit index 3 of TCCRB with one more bitshift */
    *(internal_config[id].handle.TCCRB) = (*(internal_config[id].handle.TCCRB) & ~WGM2_MSK) | (waveform & (1U << 2U) << 1U);
    return ret;
}

timer_error_t timer_16_bit_get_waveform_generation(uint8_t id, timer_8_bit_waveform_generation_t * waveform)
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

    *waveform = (timer_8_bit_waveform_generation_t)(0U);
    *waveform |= (*(internal_config[id].handle.TCCRA) & (WGM0_MSK | WGM1_MSK));
    *waveform |= (*(internal_config[id].handle.TCCRB) & (WGM2_MSK)) >> 1U;
    return ret;
}

timer_error_t timer_16_bit_set_counter_value(uint8_t id, const uint8_t ticks)
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
    *(internal_config[id].handle.TCNT) = ticks;
    return ret;
}

timer_error_t timer_16_bit_get_counter_value(uint8_t id, uint8_t * ticks)
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
    *ticks = *internal_config[id].handle.TCNT;
    return ret;
}

timer_error_t timer_16_bit_set_ocra_register_value(uint8_t id, uint8_t ocra)
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

    *internal_config[id].handle.OCRA = ocra;
    return ret;
}

timer_error_t timer_16_bit_get_ocra_register_value(uint8_t id, uint8_t * ocra)
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

    *ocra = *internal_config[id].handle.OCRA;
    return ret;
}

timer_error_t timer_16_bit_set_ocrb_register_value(uint8_t id, uint8_t ocrb)
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

    *internal_config[id].handle.OCRB = ocrb;
    return ret;
}

timer_error_t timer_16_bit_get_ocrb_register_value(uint8_t id, uint8_t * ocrb)
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

    *ocrb = *internal_config[id].handle.OCRB;
    return ret;
}

static timer_error_t timer_8_bit_write_config(uint8_t id, timer_16_bit_config_t * const config)
{
    timer_error_t ret = TIMER_ERROR_OK;
    /* Do not perform all usual parameter check because this function shall only be called
     * by already protected api (meaning, caller functions will have already performed parameter checking
     * so there is no need to perform it twice */
    timer_16_bit_handle_t * handle = &internal_config[id].handle;

    /* Initialise counter as well */
    *(handle->TCNT) = config->timing_config.counter;

    /* TCCRA register */
    *(handle->OCRA) = config->timing_config.ocra_val;
    *(handle->OCRB) = config->timing_config.ocrb_val;
    *(handle->TCCRA) = (*(handle->TCCRA) & ~COMA_MSK) | (config->timing_config.comp_match_a << COMA_BIT);
    *(handle->TCCRA) = (*(handle->TCCRA) & ~COMB_MSK) | (config->timing_config.comp_match_b << COMB_BIT);
    *(handle->TCCRA) = (*(handle->TCCRA) & ~(WGM0_MSK | WGM1_MSK)) | (config->timing_config.waveform_mode & (WGM0_MSK | WGM1_MSK));

    /* TCCRB register */
    /* Select bit index 2 of waveform mode (matches datasheet bit mapping) and store it to bit index 3 of TCCRB with one more bitshift */
    *(handle->TCCRB) = (*(handle->TCCRB) & ~WGM2_MSK) | ((config->timing_config.waveform_mode & (1U << 2U)) << 1U);

    /* Handles force output compare A flags */
    if (true == config->force_compare.force_comp_match_a)
    {
        *(handle->TCCRB) |= (1 << FOCA_BIT) ;
    }
    else
    {
        *(handle->TCCRB) &=  ~(1 << FOCA_BIT) ;
    }

    /* Handles force output compare A flags */
    if (true == config->force_compare.force_comp_match_b)
    {
        *(handle->TCCRB) |= (1 << FOCB_BIT) ;
    }
    else
    {
        *(handle->TCCRB) &=  ~(1 << FOCB_BIT) ;
    }

    /* NOTE : Do not handle prescaler until timer is manually started using timer_16_bit_start(id)*/

    /* TIMSK register */
    if (true == config->interrupt_config.it_comp_match_a)
    {
        *(handle->TIMSK) |= 1U << OCIEA_BIT;
    }
    else
    {
        *(handle->TIMSK) &= ~(1U << OCIEA_BIT);
    }

    if (true == config->interrupt_config.it_comp_match_b)
    {
        *(handle->TIMSK) |= 1U << OCIEB_BIT;
    }
    else
    {
        *(handle->TIMSK) &= ~(1U << OCIEB_BIT);
    }

    /* TOIE interrupt flag is the first bit, no need to bitshift it */
    if (true == config->interrupt_config.it_timer_overflow)
    {
        *(handle->TIMSK) |= 1U;
    }
    else
    {
        *(handle->TIMSK) &= ~1U;
    }
    return ret;
}

timer_error_t timer_8_bit_init(uint8_t id, timer_16_bit_config_t * const config)
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

    ret = timer_8_bit_write_config(id,config);
    if (TIMER_ERROR_OK == ret)
    {
        internal_config[id].is_initialised = true;
    }
    return ret;
}


timer_error_t timer_8_bit_deinit(uint8_t id)
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
        ret = timer_8_bit_write_config(id, &config);
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
    *(internal_config[id].handle.TCCRB) = (*(internal_config[id].handle.TCCRB) & ~CS_MSK) | TIMERxBIT_CLK_NO_CLOCK;
    return ret;
}