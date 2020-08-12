#ifndef TIMER_8_BIT_ASYNC_REG_HEADER
#define TIMER_8_BIT_ASYNC_REG_HEADER

#include "timer_generic.h"

/* TCCRA register bit mapping */
#define WGM0_BIT    0
#define WGM1_BIT    1
#define COMB_BIT    4
#define COMA_BIT    6

/* TCCRB register bit mapping */
#define CS_START_BIT 0
#define WGM2_BIT     3
#define FOCB_BIT     6
#define FOCA_BIT     7

/* TIMSK register bit mapping */
#define TOEI_BIT    0
#define OCIEA_BIT   1
#define OCIEB_BIT   2

/* TIFR register bit mapping */
#define TOF_BIT     0
#define OCFA_BIT    1
#define OCFB_BIT    2

/* TCCRA register masks */
#define WGM0_MSK   0x01
#define WGM1_MSK   0x02
#define COMB_MSK   0x30
#define COMA_MSK   0xC0

/* TCCRB register masks */
#define CS_MSK     0x07
#define WGM2_MSK   0x08
#define FOCB_MSK   0x40
#define FOCA_MSK   0x80

/* TIMSK register masks */
#define TOIE_MSK   0x01
#define OCIEA_MSK  0x02
#define OCIEB_MSK  0x04

/* TIMSK register masks */
#define TOV_MSK   0x01
#define OCFA_MSK  0x02
#define OCFB_MSK  0x04

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
 * @brief Describes 8 bits timers waveform generation modes
*/
typedef enum
{                                                      /**< |   Mode of operation       |   TOP   |  Update of OCRx at |  TOV Flag set on |*/
    TIMER8BIT_ASYNC_WG_NORMAL                         = 0U,  /**< | Normal operation mode     |   0xFF  |       Immediate    |      MAX         |*/
/* --------------------------------------------------------------------------------------------------------------------------------------- */
    TIMER8BIT_ASYNC_WG_PWM_FAST_FULL_RANGE            = 3U,  /**< | Fast PWM                  |   0xFF  |       BOTTOM       |      MAX         |*/
    TIMER8BIT_ASYNC_WG_PWM_FAST_OCRA_MAX              = 7U,  /**< | Fast PWM                  |   OCRA  |       BOTTOM       |      MAX         |*/
/* --------------------------------------------------------------------------------------------------------------------------------------- */
    TIMER8BIT_ASYNC_WG_PWM_PHASE_CORRECT_FULL_RANGE   = 1U,  /**< | PWM, phase correct        |   0xFF  |       TOP          |      BOTTOM      |*/
    TIMER8BIT_ASYNC_WG_PWM_PHASE_CORRECT_OCRA_MAX     = 5U,  /**< | PWM, phase correct        |   OCRA  |       TOP          |      BOTTOM      |*/
/* --------------------------------------------------------------------------------------------------------------------------------------- */
    TIMER8BIT_ASYNC_WG_CTC                            = 2U,  /**< | Clear Timer Compare match |   OCRA  |       Immediate    |      MAX         |*/
} timer_8_bit_async_waveform_generation_t;

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

/**
 * @brief describes a generic configuration for Force Output Compare A/B flags for
 * 8 bit and 16 bit timers, regardless of their actual architecture
*/
typedef struct
{
    bool force_comp_match_a; /**< Enables (or not) FOCnA flag (forces output compare A)                   */
    bool force_comp_match_b; /**< Enables (or not) FOCnB flag (forces output compare B)                   */
} timer_8_bit_async_force_compare_config_t;

/**
 * @brief Describes Compare output modes
 * Note : take a cautious look at the datasheet when using these values,
 * in some MCUs, the Toggle option is set as reserved on most of modes, except normal operation modes
*/
typedef enum
{
    TIMER8BIT_ASYNC_CMOD_NORMAL       = 0, /**< Normal port operation, OCnX disconnected  */
    TIMER8BIT_ASYNC_CMOD_TOGGLE_OCnX  = 1, /**< Toggle OCnX on compare match              */
    TIMER8BIT_ASYNC_CMOD_CLEAR_OCnX   = 2, /**< Clear OCnX on compare match               */
    TIMER8BIT_ASYNC_CMOD_SET_OCnX     = 3  /**< Set OCnX on compare match                 */
} timer_8_bit_async_compare_output_mode_t;

/**
 * @brief describes 8 bit timer interrupt configuration
*/
typedef struct
{
    bool it_comp_match_a;               /**< Enables (or not) the OCIEnA flag (interrupt on compare match A, or not)*/
    bool it_comp_match_b;               /**< Enables (or not) the OCIEnB flag (interrupt on compare match B, or not)*/
    bool it_timer_overflow;             /**< Enables (or not) the TOEIn flag (interrupt on timer overflow, or not)  */
    void (*interrupt_callback)(void);   /**< Interrupt callback used when the interrupt fires.                      */
} timer_8_bit_async_interrupt_config_t;

#endif /* TIMER_8_BIT_ASYNC_REG_HEADER */