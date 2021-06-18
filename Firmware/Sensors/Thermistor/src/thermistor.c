#include "thermistor.h"
#include "thermistor_private.h"

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
    return THERMISTOR_ERR_OK;
}




/**
 *     Vcc
 *     ┌┴┐
 *     │/│ } Thermistor
 *     └┬┘
 *      ├───── > Vout
 *     ┌┴┐
 *     │ │ } Bridge resistor (low side configuration)
 *     └┬┘
 *      ╧
*/
static int8_t compute_temperature(const uint8_t index, adc_millivolts_t * reading)
{
    bool use_cold_side = false;
    int8_t temperature = 0;
    uint8_t ratio = 0;

    // Vcc - Vout
    uint8_t potential_diff = (thermistor_driver_config[index].model.ref_voltage - *reading);

    // ((Vcc - Vout) * Rbridge) / Vout
    uint8_t thermistor_value = (potential_diff * thermistor_driver_config[index].model.bridge_res) / *reading;

    // Determine if we need to evaluate the cold side or not.
    // "Hot" side is characterized by a resistance lower than the calibration resistance, with ratios lower than 1 so we need
    // to scale up (multiply) the thermistor value by 100 in order to get the ratio from 0 to 100%
    //
    // On the cold side however, ratio always exceed 1 so we can perform a direct calculation with the parameters.
    // This time, ratio is used to express a value which is always greater than 100%, so we save some memory space by only using the integer portion of the number.
    if (thermistor_value < thermistor_driver_config[index].model.calibration.resistance )
    {
        use_cold_side = false;
        ratio = (uint8_t) ((uint16_t) (thermistor_value * 100) / (uint16_t) thermistor_driver_config[index].model.calibration.resistance);
    }
    else
    {
        use_cold_side = true;
        ratio = thermistor_value / thermistor_driver_config[index].model.calibration.resistance;
    }

    if (thermistor_driver_config[index].model.curve.type == THERMISTOR_CURVE_DISCRETE)
    {
        thermistor_curve_discrete_pair_t const * lower_bound = NULL;
        thermistor_curve_discrete_pair_t const * upper_bound = NULL;
        if (use_cold_side)
        {
            bool found = find_segment(thermistor_driver_config[index].model.curve.data.discrete.hot_side,
                                      ratio, &lower_bound, &upper_bound);
            (void) found;
        }
        else
        {
            bool found = find_segment(thermistor_driver_config[index].model.curve.data.discrete.cold_side,
                                      ratio, &lower_bound, &upper_bound);
            (void) found;
        }
    }
    else
    {
        // Do nothing for now (not yet implemented)
    }

    return temperature;
}


