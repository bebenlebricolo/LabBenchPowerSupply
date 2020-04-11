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
#include <stddef.h>

/* Application drivers */
#include "adc_reg.h"
#include "generic_peripheral.h"

/* ############################################################################################
   ################################## Data types declaration ##################################
   ############################################################################################ */

typedef uint16_t adc_result_t;
typedef uint16_t adc_millivolts_t;

/**
 * @brief gives running modes of ADC driver
*/
typedef enum
{
    ADC_RUNNING_MODE_SINGLE_SHOT,   /**< If set, adc will require start() to be called repeateadly between each calls       */
    ADC_RUNNING_MODE_AUTOTRIGGERED  /**< ADC is controlled by a trigger event, which my be set to free-running (software)
                                         in this particular case, ADC will be continuously triggered when a conversion ends  */
}adc_running_mode_t;

/**
 * @brief this structure mimics ADC peripheral registers. It is passed when configuring ADC peripheral.
 */
typedef struct {
    peripheral_reg_t  *  mux_reg;    /**< Refers to adc multiplexing register (ADCMUX)   */
    peripheral_reg_t  *  adcsra_reg; /**< Refers to ADC Control and Status Register A    */
    peripheral_reg_t  *  adcsrb_reg; /**< Refers to ADC Control and Status Register B    */

    /* Packs result registers in one */
    struct {
        peripheral_reg_t *  adclow_reg;  /**< Refers to the low value register of ADC peripheral  (first 8 bits)*/
        peripheral_reg_t *  adchigh_reg; /**< Refers to the high value register of ADC peripheral (last 2 bits) */
    } readings;
} adc_handle_t;

/* Configuration structure of ADC module */
typedef struct {
    uint16_t                    supply_voltage_mv;  /**< MCU supply voltage in millivolts                       */
    adc_prescaler_t             prescaler;          /**< ADC internal prescaler                                 */
    adc_voltage_ref_t           ref;                /**< ADC reference voltage                                  */
    adc_result_align_t          alignment;          /**< ADC data alignment                                     */
    adc_running_mode_t          running_mode;       /**< Select the mode between continous or single shot       */
    adc_autotrigger_sources_t   trigger_sources;    /**< ADC trigger mode                                       */
    adc_handle_t                handle;             /**< Packs pointers to base ADC registers. Particularly
                                                         useful when performing Dependency Injection & testing  */
    bool_t using_interrupt;                /**< uses interrupts for data fetch process or not          */
} adc_config_hal_t;


/* ############################################################################################
   #################################### Types manipulators ####################################
   ############################################################################################ */

/**
 * @brief copies a configuration from a source pointer to a destination pointer
 * @param[in]   src     : pointer to source object
 * @param[out]  dest    : pointer to destination object
 * @return
 *      PERIPHERAL_ERROR_OK             : operation succeeded
 *      PERIPHERAL_ERROR_NULL_POINTER   : null pointer guarded
*/
peripheral_error_t adc_config_hal_copy(adc_config_hal_t * dest, adc_config_hal_t * const src);

/**
 * @brief resets config to default
 * @param[in]   config : pointer to configuration object
 * @return
 *      PERIPHERAL_ERROR_OK             : operation succeeded
 *      PERIPHERAL_ERROR_NULL_POINTER   : null pointer guarded
*/
peripheral_error_t adc_config_hal_reset(adc_config_hal_t * config);

/**
 * @brief returns a default configuration for the ADC driver
 * @param[out]  config : pointer to configuration object
 * @return
 *      PERIPHERAL_ERROR_OK             : operation succeeded
 *      PERIPHERAL_ERROR_NULL_POINTER   : null pointer guarded
*/
peripheral_error_t adc_config_hal_get_default(adc_config_hal_t * config);

/**
 * @brief copies handle data from a source object to destination object
 * @param[in]   src     : pointer to source object
 * @param[out]  dest    : pointer to destination object
 * @return
 *      PERIPHERAL_ERROR_OK             : operation succeeded
 *      PERIPHERAL_ERROR_NULL_POINTER   : null pointer guarded
*/
peripheral_error_t adc_handle_copy(adc_handle_t * const dest, const adc_handle_t * const src);

/**
 * @brief resets handle to default
 * @param[in]   handle : pointer to handle object
 * @return
 *      PERIPHERAL_ERROR_OK             : operation succeeded
 *      PERIPHERAL_ERROR_NULL_POINTER   : null pointer guarded
*/
peripheral_error_t adc_handle_reset(adc_handle_t * const handle);

/**
 * @brief returns a default handle, set to target avr registers
 * @param[in]   handle : pointer to handle object
 * @return
 *      PERIPHERAL_ERROR_OK             : operation succeeded
 *      PERIPHERAL_ERROR_NULL_POINTER   : null pointer guarded
*/
peripheral_error_t adc_handle_get_default(adc_handle_t * const handle);

/* ############################################################################################
   ################################## Function declarations ###################################
   ############################################################################################ */

/**
 * @brief adc module initialisation function
*/
peripheral_error_t adc_base_init(adc_config_hal_t * const config);

/**
 * @brief module deinitialisation
*/
void adc_base_deinit(void);

/**
 * @brief starts the adc in normal mode, no interrupt set
 * @see adc_state_t adc_process() to be able to fetch results from adc registers and post result to internal buffer
 * Note: one must use functions in this order :
 *    - adc_base_init() // done once
 *    - adc_start()     // starts peripheral
 *    - repeat :
 *       - if interrupt mode is not set : adc_process()   // fetches result from adc peripheral
 *       - adc_read_raw()       // return results to application
 *    OR - adc_read_millivolt() // instead to retrieve actual voltage instead of raw value
 *    - adc_stop()      // stops the adc
 * @return
 *      PERIPHERAL_ERROR_OK      : operation was successful
 *      PERIPHERAL_ERROR_CONFIG  : adc peripheral is not enabled (not initialised ?)
*/
peripheral_state_t adc_start(void);

/**
 * @brief stops the adc in normal mode, no interrupt
 * @return
 *      PERIPHERAL_ERROR_OK      : operation was successful
 *      PERIPHERAL_ERROR_CONFIG  : adc peripheral is not enabled (not initialised ?)
*/
peripheral_state_t adc_stop(void);

/**
 * @brief starts conversions and retrieves results (using asynchronous, non interrupting mode)
 * @return
 *      PERIPHERAL_ERROR_OK      : operation was successful
 *      PERIPHERAL_ERROR_CONFIG  : adc peripheral is not enabled (not initialised ?)
*/
peripheral_state_t adc_process(void);

/**
 * @brief isr handler that might be called from within an ISR function
*/
void adc_isr_handler(void);

/**
 * @brief explicitely adds a channel to scanned channels configuration
 * @param[in]   channel : channel to be configured and scanned */
peripheral_error_t adc_register_channel(const adc_mux_t channel);

/**
 * @brief explicitely removes channel from scanned channels configuration
 * @param[in]   channel : channel to be removed */
peripheral_error_t adc_unregister_channel(const adc_mux_t channel);

/**
 * @brief adc result getter function
 * @param[in]   channel  : targeted device index
 * @param[out]  result   : last fetched result from this device
 * @return
 *      PERIPHERAL_ERROR_OK             : everything's fine
 *      PERIPHERAL_ERROR_NULL_POINTER   : wrong pointer or out of bounds index
*/
peripheral_error_t adc_read_raw(const adc_mux_t channel, adc_result_t * const result);

/**
 * @brief adc raw reading getter
 * @param[in]   channel   : targeted device index
 * @param[out]  reading : last fetched result from this device
 * @return
 *      PERIPHERAL_ERROR_OK      : everything's fine
 *      PERIPHERAL_ERROR_NULL_POINTER  : wrong pointer or out of bounds index
*/
peripheral_error_t adc_read_millivolt(const adc_mux_t channel, adc_millivolts_t * const reading);


#ifdef __cplusplus
}
#endif

#endif /* ADC_HEADER */