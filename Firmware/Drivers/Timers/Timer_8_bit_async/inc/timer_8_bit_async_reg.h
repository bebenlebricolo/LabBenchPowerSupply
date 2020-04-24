#ifndef TIMER_8_BIT_ASYNC_REG_HEADER
#define TIMER_8_BIT_ASYNC_REG_HEADER

#include "timer_generic.h"

/* ASSR register bit mapping */
#define TCRBUB_BIT 0
#define TCRAUB_BIT 1
#define OCRBUB_BIT 2
#define OCRAUB_BIT 3
#define TCNUB_BIT  4
#define AS2_BIT    5
#define EXCLK_BIT  6

/* ASSR register masks */
#define TCRBUB_MSK 0x01
#define TCRAUB_MSK 0x02
#define OCRBUB_MSK 0x04
#define OCRAUB_MSK 0x08
#define TCNUB_MSK  0x10
#define AS2_MSK    0x20
#define EXCLK_MSK  0x40

/* GTCCR register bit mapping */
#define PSRSYNC_BIT 0
#define PSRASY_BIT  1
#define TSM_BIT     7

/* GTCCR register masks */
#define PSRSYNC_MSK 0x01
#define PSRASY_MSK  0x02
#define TSM_MSK     0x80

/**
 * @brief prescaler selection bits for 8 bit timer with async mode
*/
typedef enum
{
    TIMER8BIT_ASYNC_CLK_NO_CLOCK       = 0, /**< No clock is fed to the timer : timer is stopped                      */
    TIMER8BIT_ASYNC_CLK_PRESCALER_1    = 1, /**< Clock is not prescaled : clock speed is the same as the main clock   */
    TIMER8BIT_ASYNC_CLK_PRESCALER_8    = 2, /**< Clock is divided by 8                                                */
    TIMER8BIT_ASYNC_CLK_PRESCALER_32   = 3, /**< Clock is divided by 32                                               */
    TIMER8BIT_ASYNC_CLK_PRESCALER_64   = 4, /**< Clock is divided by 64                                               */
    TIMER8BIT_ASYNC_CLK_PRESCALER_128  = 5, /**< Clock is divided by 128                                              */
    TIMER8BIT_ASYNC_CLK_PRESCALER_256  = 6, /**< Clock is divided by 256                                              */
    TIMER8BIT_ASYNC_CLK_PRESCALER_1024 = 7  /**< Clock is divided by 1024                                             */
} timer_8_bit_async_prescaler_selection_t;

/**
 * @brief short enum which describes which clock source is used by the timer
*/
typedef enum
{
    TIMER8BIT_ASYNC_CLK_SOURCE_EXTERNAL, /**< Uses external clock source connected to TOSC1 of device (external oscillator pins) */
    TIMER8BIT_ASYNC_CLK_SOURCE_INTERNAL, /**< Uses MCU main clock (CLKi/o) to feed the timer                                     */
} timer_8_bit_async_clock_source_t;

#endif /* TIMER_8_BIT_ASYNC_REG_HEADER */