/**
 * @brief ADC custom driver
*/

#ifndef ADC_HEADER
#define ADC_HEADER

/* Generic includes */
#include <stdbool.h>

/* Standard avr drivers */
#include <avr/io.h>
#include <avr/common.h>

/* Application drivers */
#include "generic_peripheral.h"
#include "adc_reg.h"

typedef uint16_t adc_result_t;
typedef uint16_t adc_millivolts_t;

/* Protect a bit initialisation array */
#ifndef ANALOG_DEVICES_CNT
    #warning "ANALOG_DEVICES_CNT undefined. To use adc, please define it first and give it the number of channels you want to use"
    #define ANALOG_DEVICES_CNT ADC_MUX_COUNT
#elif ANALOG_DEVICES_CNT > ADC_MUX_COUNT
    #error "More than available adc channels were specified, check ANALOG_DEVICE_CNT"
#endif

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
    struct {
        adc_mux_t   devices[ANALOG_DEVICES_CNT]; /**< Channels to be scanned            */
        uint8_t     configured_channels;         /**< How many channels are registered  */
    } channels_config;
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
 * @brief adc result getter function
 * @param[in]   channel  : targeted device index
 * @param[out]  result   : last fetched result from this device
 * @return
 *      ERROR_NONE      : everything's fine
 *      ERROR_ARGUMENT  : wrong pointer or out of bounds index
*/
peripheral_error_t adc_read(const uint8_t channel, adc_result_t * const result);

/**
 * @brief adc raw reading getter
 * @param[in]   channel   : targeted device index
 * @param[out]  reading : last fetched result from this device
 * @return
 *      ERROR_NONE      : everything's fine
 *      ERROR_ARGUMENT  : wrong pointer or out of bounds index
*/
peripheral_error_t adc_read_millivolt(const uint8_t channel, adc_millivolts_t * const reading);


#endif /* ADC_HEADER */