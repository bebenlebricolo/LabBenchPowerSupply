#ifndef ADC_STUB_HEADER
#define ADC_STUB_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#define ADC_STUB_MAX_READINGS 5U

/**
 * @brief updates internal data storage for selected adc channel
*/
void set_adc_readings(const adc_mux_t channel, const adc_millivolts_t reading);

/**
 * @brief clears all fakes adc readings
*/
void clear_readings(void);

/**
 * @brief performs a fake channel registration, as if it has effectively been registered by application program
*/
void register_channel(const adc_mux_t channel);

/**
 * @brief clears registered channels
*/
void clear_registered_channels(void);

/**
 * @brief resets this fake driver
*/
void clear_all(void);

#ifdef __cplusplus
}
#endif

#endif /* ADC_STUB_HEADER */