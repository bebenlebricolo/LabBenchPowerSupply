#include "thermistor.h"
#include "thermistor_private.h"

#define RATIO_POSITIVE_LOWER_LIMIT 10U

static int8_t temperatures[THERMISTOR_MAX_THERMISTORS] = {0};

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


bool find_segment(thermistor_curve_discrete_t const * const curve,
                  const uint8_t ratio,
                  const ratio_coefficients_t coef,
                  thermistor_curve_discrete_pair_t const ** lower_bound,
                  thermistor_curve_discrete_pair_t const ** upper_bound)
{
    uint8_t index = 0;
    uint8_t upper_idx = curve->count - 1;
    uint8_t lower_idx = 0;
    uint8_t rcoef = 1;

    // Special case where ratio is expressed as 10x the real ratio, to prevent data loss
    // This only works for that particular value, interpolation is the same for the 1x and 100x cases however because data is directly
    // encoded like this in the curves.
    if (RATIO_COEF_10 == coef)
    {
        rcoef = (uint8_t) coef;
    }

    // Reject out of bounds values
    if (ratio > (curve->data[0].ratio * rcoef))
    {
        *lower_bound = &curve->data[0];
        *upper_bound = *lower_bound;
        return false;
    }

    if(ratio < (curve->data[curve->count - 1].ratio * rcoef))
    {
        *lower_bound = &curve->data[curve->count - 1];
        *upper_bound = *lower_bound;
        return false;
    }

    while((upper_idx - lower_idx) > 1)
    {
        index = ((upper_idx - lower_idx) / 2) + lower_idx;
        uint16_t current_ratio = curve->data[index].ratio * rcoef;

        // Exact match, we need to return the two exact same data pair then
        if (ratio == current_ratio)
        {
            lower_idx = index;
            upper_idx = index;
            break;
        }

        // Curves are written in reverse order (maximum ratio first, cold temperatures first)
        if (ratio > current_ratio)
        {
            upper_idx = index;
        }
        else
        {
            lower_idx = index;
        }
    }

    *lower_bound = &curve->data[lower_idx];
    *upper_bound = &curve->data[upper_idx];

    return true;
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
int8_t compute_temperature(const uint8_t index, adc_millivolts_t * reading)
{
    int8_t temperature = 0;
    uint8_t ratio = 0;

    ratio_coefficients_t coef = compute_ratio(&thermistor_driver_config[index].model, reading, &ratio);

    // Determine if we need to evaluate the cold side or not.
    // "Hot" side is characterized by a resistance lower than the calibration resistance, with ratios lower than 1 so we need
    // to scale up (multiply) the thermistor value by 100 in order to get the ratio from 0 to 100%
    //
    // On the cold side however, ratio always exceed 1 so we can perform a direct calculation with the parameters.
    // This time, ratio is used to express a value which is always greater than 100%, so we save some memory space by only using the integer portion of the number.
    if (thermistor_driver_config[index].model.curve.type == THERMISTOR_CURVE_DISCRETE)
    {
        thermistor_curve_discrete_pair_t const * lower_bound = NULL;
        thermistor_curve_discrete_pair_t const * upper_bound = NULL;
        bool found = true;
        thermistor_curve_discrete_t const * curve = NULL;
        if (coef == RATIO_COEF_100)
        {
            curve = thermistor_driver_config[index].model.curve.data.discrete.hot_side;
        }
        else
        {
            curve = thermistor_driver_config[index].model.curve.data.discrete.cold_side;
        }

        found = find_segment(curve, ratio, coef, &lower_bound, &upper_bound);

        // If not found, take the lowest temperature of the range as a default value
        if (!found)
        {
            temperature = lower_bound->temperature;
        }
        else
        {
            uint8_t rcoef = 1U;

            // Same remark as somewhere above : special case handling only for the 10x case where curves are not encoded with this 10x rule (because we'll lose data)
            // This case is an in-between case ; RATIO_COEF_1 and RATIO_COEF_100 work just fine as curves already contain correctly encoded data
            // Note: we would not have such an issue with uint16_t (up to ratios 65+), but this consumes a lot of FLASH/SRAM for nothing.
            if (RATIO_COEF_10 == coef)
            {
                rcoef = (uint8_t) coef;
            }

            uint16_t numerator_part = (ratio - (lower_bound->ratio * rcoef)) * (lower_bound->temperature - upper_bound->temperature);
            uint16_t denominator_part = rcoef * (lower_bound->ratio - upper_bound->ratio);
            temperature = (numerator_part / denominator_part) + lower_bound->temperature;
        }
    }
    else
    {
        // Do nothing for now (not yet implemented)
    }

    return temperature;
}

ratio_coefficients_t compute_ratio(thermistor_model_t const * const model, const adc_millivolts_t * reading, uint8_t * const ratio)
{
    ratio_coefficients_t coef = RATIO_COEF_1;

    // Vcc - Vout
    adc_millivolts_t potential_diff = (model->ref_voltage - *reading);

    // ((Vcc - Vout) * Rbridge) / Vout
    uint16_t thermistor_value = (potential_diff * model->bridge_res) / *reading;

    if (thermistor_value >= model->calibration.resistance )
    {
        // Perform upscaled calculation first, then decide if we need to downscale it or not
        // This is done this way to get the maximum resolution first (thermistor value * 100) * 10
        // Then we can decimate the result if need be.
        // Curves are still changing quite fast around ratios close to 1,2,3,4, so that's why we may use a bit more precise
        // ratio evaluation. For instance, if computed ratio equals 1.7, integer arithmetic will truncate it to 1 and we won't be able to interpolate between 1 and 2 ratio values.

        uint16_t tmp_ratio = (thermistor_value * 1000) / (uint16_t) model->calibration.resistance;
        if ((tmp_ratio / 1000U) < RATIO_POSITIVE_LOWER_LIMIT)
        {
            coef = RATIO_COEF_10;
            *ratio = tmp_ratio / 100U;
        }
        else
        {
            *ratio = tmp_ratio / 1000U;
        }
    }
    else
    {
        coef = RATIO_COEF_100;
        *ratio = thermistor_value * 100U / model->calibration.resistance;
    }

    return coef;
}