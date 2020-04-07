#ifndef ADC_STACK_HEADER
#define ADC_STACK_HEADER

/* Expose this API to C++ code without name mangling */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

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
    ADC_STACK_ERROR_OK,                 /**< Action was successful                  */
    ADC_STACK_ERROR_FULL,               /**< Given stack is full                    */
    ADC_STACK_ERROR_EMPTY,              /**< Given stack is empty                   */
    ADC_STACK_ERROR_NULL_POINTER,      /**< Given pointer not initialised          */
    ADC_STACK_ERROR_ELEMENT_NOT_FOUND,   /**< Targeted element not found in stack    */
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
} adc_stack_t;


/*  ####################################################################################
    ############################ Functions declarations ################################
    #################################################################################### */

/**
 * @brief copy a pair from source to destination
 * @param[out] dest : pointer to destination object
 * @param[in]  src  : pointer to source object
 *      ADC_STACK_ERROR_OK              : operation succeeded
 *      ADC_STACK_ERROR_NULL_POINTER   : given pointer is NULL
*/
adc_stack_error_t adc_channel_pair_copy(adc_channel_pair_t * dest, adc_channel_pair_t * const src);

/**
 * @brief initialises a newly created object with default data
 * @param[in]  pair  : pointer to object
 * @return 
 *      ADC_STACK_ERROR_OK              : operation succeeded
 *      ADC_STACK_ERROR_NULL_POINTER   : given pointer is NULL
*/
adc_stack_error_t adc_channel_pair_reset(adc_channel_pair_t * const pair);

/**
 * @brief Initialises all data structure to 0. Similar to a clear() action
 * @param[in] stack to be initialised
*/
adc_stack_error_t adc_stack_reset(adc_stack_t * const stack);

/**
 * @brief Registers a channel in adc stack
 * @param[in] stack :   adc stack object
 * @param[in] mux   :   adc multiplexing value
 * @return 
 *      ADC_STACK_ERROR_OK      :   action performed ok
 *      ADC_STACK_ERROR_FULL    :   stack is full, could not add one more channel pair 
 *                                  (there might be several instances of the same channel inside) 
*/
adc_stack_error_t adc_stack_register_channel(adc_stack_t * const stack, const adc_mux_t mux);

/**
 * @brief Removes a channel from adc stack
 * @param[in] stack :   adc stack object
 * @param[in] mux   :   adc multiplexing value
 * @return 
 *      ADC_STACK_ERROR_OK      :   action performed ok
 *      ADC_STACK_ERROR_EMPTY   :   stack is empty, could not remove one more 
*/
adc_stack_error_t adc_stack_unregister_channel(adc_stack_t * const stack, const adc_mux_t mux);


/**
 * @brief finds first matching channel in stack and returns a pointer to the channel_pair
 * @param[in] stack     :   adc stack object
 * @param[in] channel   :   adc channel value
 * @param[in] pair      :   pointer to found item, set to NULL if not found
 * @return 
 *      ADC_STACK_ERROR_OK      :   action performed ok
 *      ADC_STACK_ERROR_EMPTY   :   stack is empty, could not remove one more 
*/
adc_stack_error_t adc_stack_find_channel(adc_stack_t * const stack, const adc_mux_t channel, adc_channel_pair_t ** pair);

/**
 * @brief returns next channel to be scanned (mainly called either by ISR or asynchronous code)
 * @param[in] stack :   adc stack object
 * @param[out] pair :   pointer to next pair
 * @return 
 *      ADC_STACK_ERROR_OK      :   action performed ok
 *      ADC_STACK_ERROR_EMPTY   :   stack is empty, no next pair. Returned pair is unchanged 
*/
adc_stack_error_t adc_stack_get_next(adc_stack_t * const stack, adc_channel_pair_t ** pair);

/**
 * @brief returns currently scanned pair object
 * @param[in] stack :   adc stack object
 * @param[out] pair :   pointer to current pair
 * @return 
 *      ADC_STACK_ERROR_OK      :   action performed ok
 *      ADC_STACK_ERROR_EMPTY   :   stack is empty, no next pair. Returned pair is unchanged 
*/
adc_stack_error_t adc_stack_get_current(adc_stack_t * const stack, adc_channel_pair_t ** pair);

/* Expose this API to C++ code without name mangling */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif