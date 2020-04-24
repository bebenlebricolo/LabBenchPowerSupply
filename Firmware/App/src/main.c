#include "adc.h"
#include "timer_8_bit.h"
#include "timer_8_bit_async.h"
#include "timer_16_bit.h"

#include "driver_setup.h"
#include <avr/interrupt.h>

#ifdef USE_SIMAVR_VCD_TRACE
    #include <simavr/avr/avr_mcu_section.h>
    #include "pin_mapping.h"
    AVR_MCU(16000000, "atmega328p");
    AVR_MCU_VCD_FILE("my_trace_file.vcd", 1000);

    const struct avr_mmcu_vcd_trace_t _mytrace[] _MMCU_ =
    {
        { AVR_MCU_VCD_SYMBOL("OC0A"), .mask = (1 << OC0A_PIN_NUMBER), .what = (void*)&OC0A_PORT_REG },
        { AVR_MCU_VCD_SYMBOL("OC0B"), .mask = (1 << OC0B_PIN_NUMBER), .what = (void*)&OC0B_PORT_REG },
        { AVR_MCU_VCD_SYMBOL("OC1A"), .mask = (1 << OC1A_PIN_NUMBER), .what = (void*)&OC1A_PORT_REG },
        { AVR_MCU_VCD_SYMBOL("OC1A"), .mask = (1 << OC1B_PIN_NUMBER), .what = (void*)&OC1B_PORT_REG },
    };
#endif

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

    /* Set up 8 bit timer 0 as 8 bit FAST PWM generator */
    init_error = init_timer_0();
    if (DRIVER_SETUP_ERROR_OK != init_error)
    {
        error_handler();
    }

    /* Set up 16 bit timer 1 as 10 bit FAST PWM generator */
    init_error = init_timer_1();
    if (DRIVER_SETUP_ERROR_OK != init_error)
    {
        error_handler();
    }

    /* Set up 8 bit timer 2 as 8 bit FAST PWM generator */
    init_error = init_timer_2();
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

    /* Start both timers */
    timer_error_t timer_error = timer_8_bit_start(0);
    if (TIMER_ERROR_OK != timer_error)
    {
        error_handler();
    }
    timer_error = timer_16_bit_start(0);
    if (TIMER_ERROR_OK != timer_error)
    {
        error_handler();
    }
    timer_error = timer_8_bit_async_start(0);
    if (TIMER_ERROR_OK != timer_error)
    {
        error_handler();
    }

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