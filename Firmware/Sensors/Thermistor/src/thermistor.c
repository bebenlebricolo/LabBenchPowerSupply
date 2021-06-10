#include "thermistor.h"

static int8_t temperatures[THERMISTOR_MAX_THERMISTORS] = {0};

static int8_t compute_temperature(const uint8_t index, adc_millivolts_t * reading);

thermistor_error_t thermistor_read(const uint8_t index, int8_t * temp)
{
    if (index >= THERMISTOR_MAX_THERMISTORS)
    {
        return THERMISTOR_ERR_INDEX_OUT_OF_BOUNDS;
    }

    *temp = temperatures[index];
    return THERMISTOR_ERR_OK;
}

thermistor_error_t thermistor_init(void)
{
    return THERMISTOR_ERR_OK;
}

thermistor_error_t thermistor_process(void)
{
    adc_millivolts_t reading = 0;
    adc_error_t err = ADC_ERROR_OK;
    for (uint8_t i = 0 ; i < THERMISTOR_MAX_THERMISTORS ; i++)
    {
        err = adc_read_millivolt(thermistor_driver_config[i].adc_index, &reading);
        if (err == ADC_ERROR_OK)
        {
            temperatures[i] = compute_temperature(i, &reading);
        }
    }
}


static int8_t compute_temperature(const uint8_t index, adc_millivolts_t * reading)
{
    int8_t temperature = 0;
    uint8_t ratio = 0;

    //if (thermistor_driver_config[index].model.curve.type == THERMISTOR_CURVE_DISCRETE)
    //{
//
    //}
    //else
    //{
//
    //}

    return temperature;
}
