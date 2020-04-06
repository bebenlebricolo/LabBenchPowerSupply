#ifndef ADC_STACK_HEADER
#define ADC_STACK_HEADER

#include <stdint.h>
#include "adc.h"
#include "adc_reg.h"

/*  ####################################################################################
    ############################ Data types declaration ################################
    #################################################################################### */

/**
 * @brief lists available error types for this ADC stack object
*/
typedef enum 
{   
    ADC_STACK_ERROR_OK,     /**< Action was successful          */
    ADC_STACK_ERROR_FULL,   /**< Given stack is full            */
    ADC_STACK_ERROR_EMPTY,  /**< Given stack is empty           */
    ADC_STACK_WRONG_POINTER /**< Given pointer not initialised  */
} adc_stack_error_t;

/**
 * @brief packs both a channel and its result
*/
typedef struct 
{    
    adc_mux_t    channel;   /**< Adc configured channel (uses a ADC_MUX type)                     */
    adc_result_t result;    /**< Adc result type, last value read by ADC                          */
    bool         locked;    /**< Pair lock value (to prevent ISR to post new value while reading) */
                            /**< ISR value posting will be discarded                              */
} adc_channel_pair_t;

/**
 * @brief Packs all registered channels alongside their values in one place
 * This structure helps manipulate adc results and provide a standardised access
 * interface to iterate over registered channels
*/
typedef struct
{
    uint8_t   count;
    uint8_t   index; 
    adc_channel_pair_t channels_pair[ADC_MUX_COUNT];    
} adc_registered_channels_t;


/*  ####################################################################################
    ############################ Functions declarations ################################
    #################################################################################### */

/**
 * @brief Initialises all data structure to 0. Similar to a clear() action
 * @param[in] stack to be initialised
*/
adc_stack_error_t adc_registered_channels_clear(adc_registered_channels_t * const stack);

/**
 * @brief Registers a channel in adc stack
 * @param[in] stack :   adc stack object
 * @param[in] mux   :   adc multiplexing value
 * @return 
 *      ADC_STACK_ERROR_OK      :   action performed ok
 *      ADC_STACK_ERROR_FULL    :   stack is full, could not add one more channel pair 
 *                                  (there might be several instances of the same channel inside) 
*/
adc_stack_error_t adc_registered_channels_add_channel(adc_registered_channels_t * const stack, const adc_mux_t mux);

/**
 * @brief Removes a channel from adc stack
 * @param[in] stack :   adc stack object
 * @param[in] mux   :   adc multiplexing value
 * @return 
 *      ADC_STACK_ERROR_OK      :   action performed ok
 *      ADC_STACK_ERROR_EMPTY   :   stack is empty, could not remove one more 
*/
adc_stack_error_t adc_registered_channels_remove_channel(adc_registered_channels_t * const stack, const adc_mux_t mux);

/**
 * @brief returns next channel to be scanned (mainly called either by ISR or asynchronous code)
 * @param[in] stack :   adc stack object
 * @param[out] pair :   pointer to next pair
 * @return 
 *      ADC_STACK_ERROR_OK      :   action performed ok
 *      ADC_STACK_ERROR_EMPTY   :   stack is empty, no next pair. Returned pair is unchanged 
*/
adc_stack_error_t adc_registered_channels_get_next(adc_registered_channels_t * const stack, adc_channel_pair_t * const pair);

#endif