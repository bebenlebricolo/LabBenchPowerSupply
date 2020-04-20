#ifndef ADC_REGISTER_STUB
#define ADC_REGISTER_STUB

#ifdef __cplusplus
extern "C" {
#endif

#include "adc.h"

/**
 * @brief mimics adc internal registers
 * Note: it is almost 100% the same structure as adc_handle_t, but contains actual values instead of
 * pointers
 * Note 2 : this structure does not represent by itself the actual behavior of ADC's registers, such as
 * read-only registers, reserved registers, reset register by writing a 1 to it, etc..
 * This might be handled by a dedicated module which continuously checks against a reference to detect changes
 * and react accordingly
*/
typedef struct
{
    volatile uint8_t mux_reg;
    volatile uint8_t adcsra_reg;
    volatile uint8_t  adcsrb_reg;
    struct {
        volatile uint8_t adclow_reg;
        volatile uint8_t adchigh_reg;
    } readings;
} adc_register_stub_t;

/* Implemented in source file */
extern adc_register_stub_t adc_register_stub;

/**
 * @brief resets the entire structure, just like a memset
 * @param[in]   stub : adc_register_stub object to be erased
*/
void adc_register_stub_erase(adc_register_stub_t * stub);

/**
 * @brief initialises adc_handle to point to actual registers which are stubbed ones
 * @param[in]   stub   :    input referenced stub
 * @param[out]  handle :    handle to be initialised
*/
void adc_register_stub_init_adc_handle(adc_handle_t * handle, adc_register_stub_t * const stub);

#ifdef __cplusplus
}
#endif

#endif /* ADC_REGISTER_STUB */