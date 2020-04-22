#include "driver_setup.h"

#include <avr/io.h>
#include "timer_8_bit.h"
#include "timer_8_bit_async.h"
#include "timer_16_bit.h"

driver_setup_error_t init_timer_0(void)
{
    timer_8_bit_config_t config;
    timer_error_t local_error;
    {
        local_error = timer_8_bit_get_default_config(&config);
        if (TIMER_ERROR_OK != local_error)
        {
            return DRIVER_SETUP_ERROR_INIT_FAILED;
        }
    }

    /* Configuring handle */
    config.handle.OCRA = &OCR0A;
    config.handle.OCRB = &OCR0B;
    config.handle.TCCRA = &TCCR0A;
    config.handle.TCCRB = &TCCR0B;
    config.handle.TCNT = &TCNT0;
    config.handle.TIFR = &TIFR0;
    config.handle.TIMSK = &TIMSK0;

	/* Enable OC0A and OC0B as outputs */
    /* PD6 = OC0A on arduino nano */
    /* PD5 = OC0B on arduino nano */
    DDRD |= (1 << PORTD5) | (1 << PORTD6);

    /* Set original port state of OC0A/OC0B pins before any OCRx overrides them */
    /* This is done in opposing polarities to respect mosfet push-pull driver pair starting point*/
    PORTD |= (1 << PORTD5);    // Pulled to Vcc (low side of mosfet driver pulls mosfet's gate to ground)
    PORTD &= ~(1 << PORTD6);   // Pulled to ground (high side of mosfet driver is depleted)

    /* Configuring timer */
    /* F_CPU = 16'000'000 Hz ; F_TIMER0 2'000'000 Hz*/
    /* 8-bit wide -> 256 values => F_PWM_0 = 7812 Hz*/
    config.timing_config.prescaler = TIMERxBIT_CLK_PRESCALER_8;
    config.timing_config.waveform_mode = TIMER8BIT_WG_PWM_FAST_FULL_RANGE;
    /* Invert OC0x behavior to provide a correct PWM for a push-pull driver pair */
    config.timing_config.comp_match_a = TIMER8BIT_CMOD_CLEAR_OCnX;
    config.timing_config.comp_match_b = TIMER8BIT_CMOD_SET_OCnX;
    /* Duty cycle : 39 % */
    config.timing_config.ocra_val = 99;
    config.timing_config.ocrb_val = 99;

    {
        local_error = timer_8_bit_init(0, &config);
        if (TIMER_ERROR_OK != local_error)
        {
            return DRIVER_SETUP_ERROR_INIT_FAILED;
        }
    }
    return DRIVER_SETUP_ERROR_OK;
}