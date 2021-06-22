#include <stdbool.h>

#include "adc.h"
#include "adc_stub.h"

static struct
{
    adc_mux_t channel;
    adc_millivolts_t reading;
    bool registered;
} stub_registered_channels[ADC_MUX_COUNT];

static adc_error_t yielded_error = ADC_ERROR_OK;

adc_error_t adc_read_millivolt(const adc_mux_t channel, adc_millivolts_t * const reading)
{
    if (!stub_registered_channels[channel].registered)
    {
        return ADC_ERROR_CHANNEL_NOT_FOUND;
    }

    if (reading == NULL)
    {
        return ADC_ERROR_NULL_POINTER;
    }

    *reading = stub_registered_channels[channel].reading;

    return yielded_error;
}

void set_adc_readings(const adc_mux_t channel, const adc_millivolts_t reading)
{
    stub_registered_channels[channel].reading = reading;
}

void clear_readings(void)
{
    for (uint8_t i = 0 ; i < ADC_MUX_COUNT ; i++)
    {
        stub_registered_channels[i].reading = 0;
    }
}

void register_channel(const adc_mux_t channel)
{
    stub_registered_channels[channel].registered = true;
}

void clear_registered_channels(void)
{
    for (uint8_t i = 0 ; i < ADC_MUX_COUNT ; i++)
    {
        stub_registered_channels[i].registered = false;
    }
}

void clear_all(void)
{
    clear_registered_channels();
    clear_readings();
    yielded_error = ADC_ERROR_OK;
}

void set_error(const adc_error_t error)
{
    yielded_error = error;
}

