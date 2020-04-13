#ifndef TIMER_8_BIT_ASYNC_REG_HEADER
#define TIMER_8_BIT_ASYNC_REG_HEADER

#include "timer_generic.h"

/**
 * @brief prescaler selection bits for 8 bit timer with async mode
*/
typedef enum
{
    TIMERxBIT_CLK_NO_CLOCK       = 0, /**< No clock is fed to the timer : timer is stopped                      */
    TIMERxBIT_CLK_PRESCALER_1    = 1, /**< Clock is not prescaled : clock speed is the same as the main clock   */
    TIMERxBIT_CLK_PRESCALER_8    = 2, /**< Clock is divided by 8                                                */
    TIMERxBIT_CLK_PRESCALER_32   = 3, /**< Clock is divided by 32                                               */
    TIMERxBIT_CLK_PRESCALER_64   = 4, /**< Clock is divided by 64                                               */
    TIMERxBIT_CLK_PRESCALER_128  = 5, /**< Clock is divided by 128                                              */
    TIMERxBIT_CLK_PRESCALER_256  = 6, /**< Clock is divided by 256                                              */
    TIMERxBIT_CLK_PRESCALER_1024 = 7  /**< Clock is divided by 1024                                             */
} timer_8_bit_async_prescaler_selection_t;

#endif /* TIMER_8_BIT_ASYNC_REG_HEADER */