#ifndef TIMER_GENERIC_REG_HEADER
#define TIMER_GENERIC_REG_HEADER

/* #########################################################################################
   ############################## Generic types for timers #################################
   ######################################################################################### */

/**
 * @brief generic prescaler selection bits
*/
typedef enum
{
    TIMERxBIT_CLK_NO_CLOCK                  = 0, /**< No clock is fed to the timer : timer is stopped                      */
    TIMERxBIT_CLK_PRESCALER_1               = 1, /**< Clock is not prescaled : clock speed is the same as the main clock   */
    TIMERxBIT_CLK_PRESCALER_8               = 2, /**< Clock is divided by 8                                                */
    TIMERxBIT_CLK_PRESCALER_64              = 3, /**< Clock is divided by 64                                               */
    TIMERxBIT_CLK_PRESCALER_256             = 4, /**< Clock is divided by 256                                              */
    TIMERxBIT_CLK_PRESCALER_1024            = 5, /**< Clock is divided by 1024                                            */
    TIMERxBIT_CLK_EXTERNAL_CLK_FALLING_EDGE = 6, /**< External clock is used as source on pin T0, clock on falling edge    */
    TIMERxBIT_CLK_EXTERNAL_CLK_RISING_EDGE  = 7  /**< External clock is used as source on pin T0, clock on rising edge     */
} timer_x_bit_prescaler_selection_t;

/**
 * @brief describes a generic configuration for Force Output Compare A/B flags for
 * 8 bit and 16 bit timers, regardless of their actual architecture
*/
typedef struct
{
    bool force_comp_match_a; /**< Enables (or not) FOCnA flag (forces output compare A)                   */
    bool force_comp_match_b; /**< Enables (or not) FOCnB flag (forces output compare B)                   */
} timer_x_bit_force_compare_config_t;

#endif /* TIMER_GENERIC_REG_HEADER */