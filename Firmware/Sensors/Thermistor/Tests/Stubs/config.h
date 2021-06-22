#ifndef CONFIG_STUB_HEADER
#define CONFIG_STUB_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include "adc.h"

#define THERMISTOR_MAX_SAMPLES_COUNT 8U
#define THERMISTOR_MAX_SENSORS 1U

#define THERMISTOR_CONFIG_ADC_INDEX (ADC_MUX_ADC1)


#ifdef __cplusplus
}
#endif

#endif /* CONFIG_STUB_HEADER */