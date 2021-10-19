#include "config.h"
#include "thermistor.h"

static const thermistor_curve_discrete_t hot_side_curve =
{
    .count = 5U,
    .data = {
        {100, 25},  /**< hot half is always less than 1, so we can retrieve percentages by multiplying by 100 to ease computations */
        {25, 39},
        {7, 53},
        {3, 64},
        {1, 77},
    }
};

static const thermistor_curve_discrete_t cold_side_curve =
{
    .count = 6U,
    .data = {
        {16, 1},    /**< cold half exceeds ratio exceeds 1 so we don't multiply the ratio by 100 to get correct computation */
        {12, 3},
        {8, 6},
        {4, 12},
        {2, 19},
        {1, 25}
    }
};

thermistor_config_t thermistor_driver_config[THERMISTOR_MAX_SENSORS] =
{
    {
        .adc_index = THERMISTOR_CONFIG_ADC_INDEX,
        .model =
        {
            .curve =
            {
                .data = {.discrete.hot_side = &hot_side_curve,
                         .discrete.cold_side = &cold_side_curve},
                .type = THERMISTOR_CURVE_DISCRETE
            },
            .bridge_res = 20U,
            .calibration.resistance = 20U,
            .calibration.temperature = 25U,
            .ref_voltage = 5000,
        }
    }
};