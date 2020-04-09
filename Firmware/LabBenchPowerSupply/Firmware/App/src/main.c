#include "adc.h"
#include "adc_reg.h"
#include <stdbool.h>
#include <avr/interrupt.h>

int main(void)
{
    adc_config_hal_t config;
    adc_config_hal_reset(&config);
    config.prescaler = ADC_PRESCALER_64;
    config.ref = ADC_VOLTAGE_REF_AVCC;
    config.running_mode = ADC_RUNNING_MODE_SINGLE_SHOT;
    config.supply_voltage_mv = 5000;
    config.using_interrupt = true;

    sei();


    while(true)
    {

    }

    return 0;
}