#include "adc.h"
#include "adc_reg.h"
#include <avr/interrupt.h>

#define MAX_MUX 5
ISR(ADC_vect)
{
    adc_isr_handler();
}

static adc_mux_t mux_table[MAX_MUX] =
{
    ADC_MUX_ADC0,
    ADC_MUX_ADC1,
    ADC_MUX_ADC2,
    ADC_MUX_ADC3,
    ADC_MUX_ADC4,
};

int main(void)
{
    adc_config_hal_t config;
    adc_config_hal_get_default(&config);
    config.prescaler = ADC_PRESCALER_64;
    config.ref = ADC_VOLTAGE_REF_AVCC;
    config.running_mode = ADC_RUNNING_MODE_SINGLE_SHOT;
    config.supply_voltage_mv = 5000;
    config.using_interrupt = true;

    adc_base_init(&config);
    for (uint8_t i = 0; i < MAX_MUX ; i++)
    {
        peripheral_error_t ret = adc_register_channel(mux_table[i]);
        (void)ret;
    }

    adc_start();
    sei();


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