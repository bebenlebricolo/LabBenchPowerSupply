#ifndef TIMER_8_BIT_ASYNC_HEADER
#define TIMER_8_BIT_ASYNC_HEADER

#include <stdint.h>
#include "generic_peripheral.h"
#include "timer_8_bit.h"
#include "timer_8_bit_async_reg.h"

/* #########################################################################################
   ################################## Timer handle types ###################################
   ######################################################################################### */

/**
 * @brief handle for 8 bits timers with asynchronous functionalities
*/
typedef struct
{
    volatile uint8_t * TCCRA; /**< Timer/Counter control register A  */
    volatile uint8_t * TCCRB; /**< Timer/Counter control register B  */
    volatile uint8_t * TCNT;  /**< Timer/Counter main register       */
    volatile uint8_t * OCRA;  /**< Output compare control register A */
    volatile uint8_t * OCRB;  /**< Output compare control register B */
    volatile uint8_t * OCRB;  /**< Output compare control register B */
    volatile uint8_t * TIMSK; /**< Timer interrupt mask register     */
    volatile uint8_t * TIFR;  /**< Timer interrupt flags register    */
    volatile uint8_t * ASSR;  /**< Asynchronous status register      */
    volatile uint8_t * GTCCR; /**< General Control register          */

} timer_8_bit_async_handle_t;

/* #########################################################################################
   ############################## Timer configuration types ################################
   ######################################################################################### */

/**
 * Generic structure to handle 8-bit atmel timers with async capabilities
*/
typedef struct
{
    timer_8_bit_timing_config_t        timing_config;    /**< Handles basic timing configuration for 8 bit timers                                */
    timer_8_bit_interrupt_config_t     interrupt_config; /**< Handles interrupt configuraitons for 8 bit timers                                  */
    timer_x_bit_force_compare_config_t force_compare;    /**< Handles force compare flags on output A and B, generic configuration among timers  */
    timer_8_bit_async_handle_t         handle;           /**< Stores pointer locations to peripheral registers                                   */
} timer_8_bit_async_config_t;

/* #########################################################################################
   ################################ Timer API definition ###################################
   ######################################################################################### */

/**
 * @brief sets the input pointer to point to the internal configuration, without copying
 * @param[in]   id      : stores the id of the targeted timer
 * @param[in]   config  : container object which will receive internal device configuration
 * This allows to use it this way :
 * timer_8_bit_config_t config;
 * timer_8_bit_get_config(&config); // will copy internal configuration to input container
 * @return
 *      PERIPHERAL_ERROR_OK             :   operation succeeded
 *      PERIPHERAL_ERROR_UNKNOWN_TIMER  :   given id is out of range
 *      PERIPHERAL_ERROR_NULL_POINTER   :   given config parameter points to NULL
*/
peripheral_error_t timer_8_bit_async_get_config( uint8_t timer_id, timer_8_bit_config_t * const * const config);

/**
 * @brief returns a default configuration for 8 bit timer
 * @param[in]   config  : container object which will receive internal device configuration
 * @return
 *      PERIPHERAL_ERROR_OK             :   operation succeeded
 *      PERIPHERAL_ERROR_NULL_POINTER   :   given config parameter points to NULL
*/
peripheral_error_t timer_8_bit_async_get_default_config(timer_8_bit_config_t * config);

/**
 * @brief sets the given interrupt flags configuration object to targeted timer
 * @param[in]   id          : stores the id of the targeted timer
 * @param[in]   it_config   : container which holds the interrupt configuration
 * @return
 *      PERIPHERAL_ERROR_OK             :   operation succeeded
 *      PERIPHERAL_ERROR_UNKNOWN_TIMER  :   given id is out of range
 *      PERIPHERAL_ERROR_NULL_POINTER   :   given it_config parameter points to NULL
*/
peripheral_error_t timer_8_bit_async_set_interrupt_config(uint8_t id, timer_8_bit_interrupt_config_t * const it_config);

/**
 * @brief reads the actual interrupt configuration from internal memory and returns a copy of it
 * @param[in]   id          : stores the id of the targeted timer
 * @param[in]   it_config   : container which holds the interrupt configuration
 * @return
 *      PERIPHERAL_ERROR_OK             :   operation succeeded
 *      PERIPHERAL_ERROR_UNKNOWN_TIMER  :   given id is out of range
 *      PERIPHERAL_ERROR_NULL_POINTER   :   given it_config parameter points to NULL
*/
peripheral_error_t timer_8_bit_async_get_interrupt_config(uint8_t id, timer_8_bit_interrupt_config_t * it_config);

peripheral_error_t timer_8_bit_async_init(timer_8_bit_config_t * const config);
peripheral_error_t timer_8_bit_async_start(timer_8_bit_config_t * const config);
peripheral_error_t timer_8_bit_async_stop(timer_8_bit_config_t * const config);

peripheral_error_t timer_8_bit_async_read_counter(timer_8_bit_config_t * config, uint8_t * const ticks);
peripheral_error_t timer_8_bit_async_set_counter(timer_8_bit_config_t * config);



#endif /* TIMER_8_BIT_ASYNC_HEADER */