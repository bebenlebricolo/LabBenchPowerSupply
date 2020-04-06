/**
 * @brief ADC custom driver
*/
#ifndef ADC_HEADER
#define ADC_HEADER

/* Expose this API to C++ code without name mangling */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Generic includes */
#include <stdbool.h>
#include "adc_reg.h"


/* Application drivers */
#include "generic_peripheral.h"

typedef uint16_t adc_result_t;
typedef uint16_t adc_millivolts_t;

/**
 * @brief this structure mimics ADC peripheral registers. It is passed when configuring ADC peripheral.
 */
typedef struct {
    peripheral_addr_t * adc_address;    /**< Refers to adc base peripheral address          */
    peripheral_reg_t *  adc_mux;        /**< Refers to adc multiplexing register (ADCMUX)   */
    peripheral_reg_t *  adcsra;         /**< Refers to ADC Control and Status Register A    */
    peripheral_reg_t *  adcsrb;         /**< Refers to ADC Control and Status Register B    */
    
    /* Packs result registers in one */
    struct {
        peripheral_reg_t *  adclow;  /**< Refers to the low value register of ADC peripheral  (first 8 bits)*/
        peripheral_reg_t *  adchigh; /**< Refers to the high value register of ADC peripheral (last 2 bits) */
    } readings;
} adc_handle_t;

/* Configuration structure of ADC module */
typedef struct {
    adc_prescaler_t     prescaler;               /**< ADC internal prescaler            */
    adc_voltage_ref_t   ref;                     /**< ADC reference voltage             */
    adc_result_align_t  alignment;               /**< ADC data alignment                */
    bool                use_interrupt;           /**< Using interrupts or not           */
    adc_handle_t        handle;                  /**< Packs pointers to base ADC registers.
                                                      Particularly useful when performing 
                                                      Dependency Injection & testing */
} adc_config_hal_t;


/* ############################################################################################
   ################################## Function declarations ###################################
   ############################################################################################ */

/**
 * @brief adc module initialisation function
*/
peripheral_error_t adc_init(adc_config_hal_t * const config, adc_handle_t const * const handle);

/**
 * @brief starts conversions and retrieves results (using asynchronous, non interrupting mode)
 * @return
 *      ERROR_NONE      : operation was successful
 *      ERROR_CONFIG    : adc peripheral is not enabled (not initialised ?)
*/
peripheral_error_t adc_process(void);

/**
* @brief explicitely adds a channel to scanned channels configuration
 * @param[in]   channel : channel to be configured and scanned */
void adc_register_channel(const adc_mux_t channel);

/**
 * @brief explicitely removes channel from scanned channels configuration
 * @param[in]   channel : channel to be removed */
void adc_unregister_channel(const adc_mux_t channel);

/**
 * @brief adc result getter function
 * @param[in]   channel  : targeted device index
 * @param[out]  result   : last fetched result from this device
 * @return
 *      ERROR_NONE      : everything's fine
 *      ERROR_ARGUMENT  : wrong pointer or out of bounds index
*/
peripheral_error_t adc_get(const uint8_t channel, adc_result_t * const result);

/**
 * @brief adc raw reading getter
 * @param[in]   channel   : targeted device index
 * @param[out]  reading : last fetched result from this device
 * @return
 *      ERROR_NONE      : everything's fine
 *      ERROR_ARGUMENT  : wrong pointer or out of bounds index
*/
peripheral_error_t adc_read_millivolt(const uint8_t channel, adc_millivolts_t * const reading);


#ifdef __cplusplus
}
#endif

#endif /* ADC_HEADER */