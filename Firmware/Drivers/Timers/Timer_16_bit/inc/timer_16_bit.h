#ifndef TIMER_16_BIT_HEADER
#define TIMER_16_BIT_HEADER

#include <stdint.h>
#include "timer_16_bit_reg.h"
#include "generic_peripheral.h"

/* #########################################################################################
   ################################## Timer handle types ###################################
   ######################################################################################### */

/**
 * @brief handle for 16 bits timers
*/
typedef struct
{
    volatile uint8_t * TCCRA;   /**< Timer/Counter control register A               */
    volatile uint8_t * TCCRB;   /**< Timer/Counter control register B               */
    volatile uint8_t * TCCRC;   /**< Timer/Counter control register C               */
    volatile uint8_t * TCNT_H;  /**< Timer/Counter main register upper 8 bits       */
    volatile uint8_t * TCNT_L;  /**< Timer/Counter main register lower 8 bits       */
    volatile uint8_t * OCRA_H;  /**< Output compare control register A upper 8 bits */
    volatile uint8_t * OCRA_L;  /**< Output compare control register A lower 8 bits */
    volatile uint8_t * OCRB_H;  /**< Output compare control register B upper 8 bits */
    volatile uint8_t * OCRB_L;  /**< Output compare control register B lower 8 bits */
    volatile uint8_t * ICR_H;   /**< Input compare register upper 8 bits            */
    volatile uint8_t * ICR_L;   /**< Input compare register lower 8 bits            */
    volatile uint8_t * TIMSK;   /**< Timer interrupt mask register                  */
    volatile uint8_t * TIFR;    /**< Timer interrupt flags register                 */
} timer_16_bit_handle_t;


/* #########################################################################################
   ############################## Timer configuration types ################################
   ######################################################################################### */

/**
 * Generic structure to handle 16-bit atmel timers
*/
typedef struct
{
    timer_16_bit_timing_config_t                       timing_config;    /**< Handles basic timing configuration for 16 bit timers                               */
    timer_16_bit_interrupt_config_t                    interrupt_config; /**< Handles interrupt configuraitons for 16 bit timers                                 */
    timer_x_bit_force_compare_config_t                 force_compare;    /**< Handles force compare flags on output A and B, generic configuration among timers  */
    timer_16_bit_input_capture_noise_canceler_config_t input_capture;    /**< Handles input capture noise canceler configuration for 16-bit timers               */
    timer_16_bit_handle_t                              handle;           /**< Stores pointer locations to peripheral registers                                   */
} timer_16_bit_config_t;

/* #########################################################################################
   ################################ Timer API definition ###################################
   ######################################################################################### */

/**
 * @brief sets the input pointer to point to the internal configuration, without copying
 * @param[in]   id      : stores the id of the targeted timer
 * @param[in]   config  : container object which will receive internal device configuration
 * This allows to use it this way :
 * timer_16_bit_config_t config;
 * timer_16_bit_get_config(&config); // will copy internal configuration to input container
 * @return
 *      PERIPHERAL_ERROR_OK             :   operation succeeded
 *      PERIPHERAL_ERROR_UNKNOWN_TIMER  :   given id is out of range
 *      PERIPHERAL_ERROR_NULL_POINTER   :   given config parameter points to NULL
*/
peripheral_error_t timer_16_bit_get_config( uint8_t timer_id, timer_16_bit_config_t * const * const config);

/**
 * @brief returns a default configuration for 8 bit timer
 * @param[in]   config  : container object which will receive internal device configuration
 * @return
 *      PERIPHERAL_ERROR_OK             :   operation succeeded
 *      PERIPHERAL_ERROR_NULL_POINTER   :   given config parameter points to NULL
*/
peripheral_error_t timer_16_bit_get_default_config(timer_16_bit_config_t * config);

/**
 * @brief sets the given interrupt flags configuration object to targeted timer
 * @param[in]   id          : stores the id of the targeted timer
 * @param[in]   it_config   : container which holds the interrupt configuration
 * @return
 *      PERIPHERAL_ERROR_OK             :   operation succeeded
 *      PERIPHERAL_ERROR_UNKNOWN_TIMER  :   given id is out of range
 *      PERIPHERAL_ERROR_NULL_POINTER   :   given it_config parameter points to NULL
*/
peripheral_error_t timer_16_bit_set_interrupt_config(uint8_t id, timer_16_bit_interrupt_config_t * const it_config);

/**
 * @brief reads the actual interrupt configuration from internal memory and returns a copy of it
 * @param[in]   id          : stores the id of the targeted timer
 * @param[in]   it_config   : container which holds the interrupt configuration
 * @return
 *      PERIPHERAL_ERROR_OK             :   operation succeeded
 *      PERIPHERAL_ERROR_UNKNOWN_TIMER  :   given id is out of range
 *      PERIPHERAL_ERROR_NULL_POINTER   :   given it_config parameter points to NULL
*/
peripheral_error_t timer_16_bit_get_interrupt_config(uint8_t id, timer_16_bit_interrupt_config_t * it_config);

peripheral_error_t timer_16_bit_init(timer_16_bit_config_t * const config);
peripheral_error_t timer_16_bit_start(timer_16_bit_config_t * const config);
peripheral_error_t timer_16_bit_stop(timer_16_bit_config_t * const config);

peripheral_error_t timer_16_bit_read_counter(timer_16_bit_config_t * config, uint8_t * const ticks);
peripheral_error_t timer_16_bit_set_counter(timer_16_bit_config_t * config);



#endif /* TIMER_16_BIT_HEADER */