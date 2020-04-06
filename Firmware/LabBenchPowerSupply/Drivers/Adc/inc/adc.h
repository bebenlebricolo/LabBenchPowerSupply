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

/* ############################################################################################
   ################################## Data types declaration ##################################
   ############################################################################################ */

typedef uint16_t adc_result_t;
typedef uint16_t adc_millivolts_t;


/**
 * @brief adc peripheral state
*/
typedef enum
{
    ADC_READY,              /**< Adc is initialised and is ready                */
    ADC_BUSY,               /**< A conversion is ongoing, peripheral is busy    */
    ADC_NOT_INITIALISED,    /**< Driver is not initialised                      */
    ADC_WRONG_POINTER       /**< A Null pointer was given                       */
} adc_state_t;

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
adc_state_t adc_base_init(adc_config_hal_t * const config, adc_handle_t const * const handle);


/**
 * @brief starts the adc in normal mode, no interrupt set
 * @see adc_state_t adc_process() to be able to fetch results from adc registers and post result to internal buffer
 * Note: one must use functions in this order :
 *    - adc_base_init() // done once
 *    - adc_start()     // starts peripheral
 *    - adc_process()   // fetches result from adc peripheral
 *    - adc_read()      // return results to application
 *    // alternatively run adc_read_millivolt() instead to retrieve actual voltage instead of raw value
 *    - adc_stop()      // stops the adc
 * @return
 *      ERROR_NONE      : operation was successful
 *      ERROR_CONFIG    : adc peripheral is not enabled (not initialised ?)
*/
adc_state_t adc_start();

/**
 * @brief starts the adc in interrupt mode (once a new value is ready, data is posted into internal buffer)
 * @return
 *      ERROR_NONE      : operation was successful
 *      ERROR_CONFIG    : adc peripheral is not enabled (not initialised ?)
*/
adc_state_t adc_start_it();

/**
 * @brief stops the adc in normal mode, no interrupt
 * @return
 *      ERROR_NONE      : operation was successful
 *      ERROR_CONFIG    : adc peripheral is not enabled (not initialised ?)
*/
adc_state_t adc_stop();

/**
 * @brief stops the adc when running in interrupt mode
 * @return
 *      ERROR_NONE      : operation was successful
 *      ERROR_CONFIG    : adc peripheral is not enabled (not initialised ?)
*/
adc_state_t adc_stop_it();

/**
 * @brief starts conversions and retrieves results (using asynchronous, non interrupting mode)
 * @return
 *      ERROR_NONE      : operation was successful
 *      ERROR_CONFIG    : adc peripheral is not enabled (not initialised ?)
*/
adc_state_t adc_process(void);

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
adc_state_t adc_read_raw(const uint8_t channel, adc_result_t * const result);

/**
 * @brief adc raw reading getter
 * @param[in]   channel   : targeted device index
 * @param[out]  reading : last fetched result from this device
 * @return
 *      ERROR_NONE      : everything's fine
 *      ERROR_ARGUMENT  : wrong pointer or out of bounds index
*/
adc_state_t adc_read_millivolt(const uint8_t channel, adc_millivolts_t * const reading);


#ifdef __cplusplus
}
#endif

#endif /* ADC_HEADER */