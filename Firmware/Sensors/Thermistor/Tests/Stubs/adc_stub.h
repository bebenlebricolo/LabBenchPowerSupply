#ifndef ADC_STUB_HEADER
#define ADC_STUB_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include "adc.h"

#define ADC_STUB_MAX_READINGS 5U

/**
 * @brief updates internal data storage for selected adc channel
*/
void stub_adc_set_readings(const adc_mux_t channel, const adc_millivolts_t reading);

/**
 * @brief clears all fakes adc readings
*/
void stub_adc_clear_readings(void);

/**
 * @brief performs a fake channel registration, as if it has effectively been registered by application program
*/
void stub_adc_register_channel(const adc_mux_t channel);

/**
 * @brief clears registered channels
*/
void stub_adc_clear_registered_channels(void);

/**
 * @brief resets this fake driver
*/
void stub_adc_clear_all(void);

/**
 * @brief sets the adc_error_t that functions will return
*/
void stub_adc_set_error(const adc_error_t error);

#ifdef __cplusplus
}
#endif

#endif /* ADC_STUB_HEADER */