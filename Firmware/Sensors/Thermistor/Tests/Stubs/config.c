#include "config.h"
#include "thermistor.h"

static const thermistor_curve_t log_curve =
{
    .discrete = {
        .count = 8U,
        .samples = {269,238,211,186,165,146,130,115,102}
    }
};

thermistor_config_t thermistor_driver_config[THERMISTOR_MAX_SENSORS] =
{
    {
        .adc_index = THERMISTOR_CONFIG_ADC_INDEX,
        .model =
        {
            .curve = {
                .type = THERMISTOR_CURVE_DISCRETE,
                .curve = &log_curve
            },
            .bridge_res = 20000U,
            .calibration.resistance = 20000U,
            .calibration.temperature = 293,
            .ref_voltage = 5000,
        }
    }
};