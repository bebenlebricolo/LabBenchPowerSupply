#include "adc.h"
#include "timer_8_bit.h"
#include "timer_8_bit_async.h"
#include "timer_16_bit.h"

#include "driver_setup.h"
#include <avr/interrupt.h>

#define MAX_MUX 5

static adc_mux_t mux_table[MAX_MUX] =
{
    ADC_MUX_ADC0,
    ADC_MUX_ADC1,
    ADC_MUX_ADC2,
    ADC_MUX_ADC3,
    ADC_MUX_ADC4,
};

ISR(ADC_vect)
{
    adc_isr_handler();
}

void error_handler(void)
{
    while(1)
    {
        asm("NOP");
    }
}

driver_setup_error_t adc_register_all_channels(void)
{
    for (uint8_t i = 0; i < MAX_MUX ; i++)
    {
        adc_error_t err = adc_register_channel(mux_table[i]);
        if (ADC_ERROR_OK != err)
        {
            return DRIVER_SETUP_ERROR_INIT_FAILED;
        }
    }
    return DRIVER_SETUP_ERROR_OK;
}

int main(void)
{
    driver_setup_error_t init_error = DRIVER_SETUP_ERROR_OK;

#ifdef USE_TIMER_DRIVER
    init_error = init_timer_0();
#else
    TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A0) | (1 << COM0B0) | (1 << COM0B1)  ;
    TCCR0B = (1 << CS01);
#endif

    if (DRIVER_SETUP_ERROR_OK != init_error)
    {
        error_handler();
    }

    init_error = init_adc();
    if (DRIVER_SETUP_ERROR_OK != init_error)
    {
        error_handler();
    }

    init_error = adc_register_all_channels();
    if (DRIVER_SETUP_ERROR_OK != init_error)
    {
        error_handler();
    }

    adc_start();
    sei();
#ifdef USE_TIMER_DRIVER
    timer_error_t timer_error = timer_8_bit_start(0);
    if (TIMER_ERROR_OK != timer_error)
    {
        error_handler();
    }
#endif

    while(true)
    {
        static uint8_t idx = 0;
        adc_result_t results[5];
        adc_read_raw(mux_table[idx % MAX_MUX], &results[idx]);

        idx++;
        idx %= MAX_MUX;
    }

    return 0;
}