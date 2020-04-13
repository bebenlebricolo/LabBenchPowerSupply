#ifndef TIMER_HEADER
#define TIMER_HEADER

#include "timer_reg.h"

/**
 * Generic structure to handle 8-bit atmel timers (might need additional tweaking for special timers such as async-capable ones)
*/
typedef struct
{
    timer_8_bit_timing_config_t        timing_config;    /**< Handles basic timing configuration for 8 bit timers                                */
    timer_8_bit_interrupt_config_t     interrupt_config; /**< Handles interrupt configuraitons for 8 bit timers                                  */
    timer_x_bit_force_compare_config_t force_compare;    /**< Handles force compare flags on output A and B, generic configuration among timers  */
} timer_8_bit_config_t;

/**
 * Generic structure to handle 8-bit atmel timers with async capabilities
*/
typedef struct
{
    timer_8_bit_timing_config_t        timing_config;    /**< Handles basic timing configuration for 8 bit timers                                */
    timer_8_bit_interrupt_config_t     interrupt_config; /**< Handles interrupt configuraitons for 8 bit timers                                  */
    timer_x_bit_force_compare_config_t force_compare;    /**< Handles force compare flags on output A and B, generic configuration among timers  */
} timer_8_bit_async_config_t;

/**
 * Generic structure to handle 16-bit atmel timers
*/
typedef struct
{
    timer_16_bit_timing_config_t                       timing_config;    /**< Handles basic timing configuration for 16 bit timers                               */
    timer_16_bit_interrupt_config_t                    interrupt_config; /**< Handles interrupt configuraitons for 16 bit timers                                 */
    timer_x_bit_force_compare_config_t                 force_compare;    /**< Handles force compare flags on output A and B, generic configuration among timers  */
    timer_16_bit_input_capture_noise_canceler_config_t input_capture;    /**< Handles input capture noise canceler configuration for 16-bit timers               */
} timer_16_bit_config_t;

#endif /* TIMER_HEADER */