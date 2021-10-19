/*

------------------
@<FreeMyCode>
FreeMyCode version : 1.0 RC alpha
    Author : bebenlebricolo
    License : 
        name : GPLv3
        url : https://www.gnu.org/licenses/quick-guide-gplv3.html
    Date : 12/02/2021
    Project : LabBenchPowerSupply
    Description : The Lab Bench Power Supply provides a simple design based around an Arduino Nano board to convert AC main voltage into
 smaller ones, ranging from 0V to 16V, with voltage and current regulations
<FreeMyCode>@
------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef TIMER_8_BIT_REG_HEADER
#define TIMER_8_BIT_REG_HEADER

#include <stdbool.h>
#include "timer_generic.h"

#ifdef __cplusplus
extern "C"
{
#endif

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

/* #########################################################################################
   ############################## 8 bits regular timers ####################################
   ######################################################################################### */

/**
 * @brief Describes 8 bits timers waveform generation modes
*/
typedef enum
{                                                      /**< |   Mode of operation       |   TOP   |  Update of OCRx at |  TOV Flag set on |*/
    TIMER8BIT_WG_NORMAL                         = 0U,  /**< | Normal operation mode     |   0xFF  |       Immediate    |      MAX         |*/
/* --------------------------------------------------------------------------------------------------------------------------------------- */
    TIMER8BIT_WG_PWM_FAST_FULL_RANGE            = 3U,  /**< | Fast PWM                  |   0xFF  |       BOTTOM       |      MAX         |*/
    TIMER8BIT_WG_PWM_FAST_OCRA_MAX              = 7U,  /**< | Fast PWM                  |   OCRA  |       BOTTOM       |      MAX         |*/
/* --------------------------------------------------------------------------------------------------------------------------------------- */
    TIMER8BIT_WG_PWM_PHASE_CORRECT_FULL_RANGE   = 1U,  /**< | PWM, phase correct        |   0xFF  |       TOP          |      BOTTOM      |*/
    TIMER8BIT_WG_PWM_PHASE_CORRECT_OCRA_MAX     = 5U,  /**< | PWM, phase correct        |   OCRA  |       TOP          |      BOTTOM      |*/
/* --------------------------------------------------------------------------------------------------------------------------------------- */
    TIMER8BIT_WG_CTC                            = 2U,  /**< | Clear Timer Compare match |   OCRA  |       Immediate    |      MAX         |*/
} timer_8_bit_waveform_generation_t;

/**
 * @brief Prescaler selection bits
*/
typedef enum
{
    TIMER8BIT_CLK_NO_CLOCK                  = 0, /**< No clock is fed to the timer : timer is stopped                      */
    TIMER8BIT_CLK_PRESCALER_1               = 1, /**< Clock is not prescaled : clock speed is the same as the main clock   */
    TIMER8BIT_CLK_PRESCALER_8               = 2, /**< Clock is divided by 8                                                */
    TIMER8BIT_CLK_PRESCALER_64              = 3, /**< Clock is divided by 64                                               */
    TIMER8BIT_CLK_PRESCALER_256             = 4, /**< Clock is divided by 256                                              */
    TIMER8BIT_CLK_PRESCALER_1024            = 5, /**< Clock is divided by 1024                                            */
    TIMER8BIT_CLK_EXTERNAL_CLK_FALLING_EDGE = 6, /**< External clock is used as source on pin T0, clock on falling edge    */
    TIMER8BIT_CLK_EXTERNAL_CLK_RISING_EDGE  = 7  /**< External clock is used as source on pin T0, clock on rising edge     */
} timer_8_bit_prescaler_selection_t;

/**
 * @brief describes a generic configuration for Force Output Compare A/B flags for
 * 8 bit and 16 bit timers, regardless of their actual architecture
*/
typedef struct
{
    bool force_comp_match_a; /**< Enables (or not) FOCnA flag (forces output compare A)                   */
    bool force_comp_match_b; /**< Enables (or not) FOCnB flag (forces output compare B)                   */
} timer_8_bit_force_compare_config_t;

/**
 * @brief Describes Compare output modes
 * Note : take a cautious look at the datasheet when using these values,
 * in some MCUs, the Toggle option is set as reserved on most of modes, except normal operation modes
*/
typedef enum
{
    TIMER8BIT_CMOD_NORMAL       = 0, /**< Normal port operation, OCnX disconnected  */
    TIMER8BIT_CMOD_TOGGLE_OCnX  = 1, /**< Toggle OCnX on compare match              */
    TIMER8BIT_CMOD_CLEAR_OCnX   = 2, /**< Clear OCnX on compare match               */
    TIMER8BIT_CMOD_SET_OCnX     = 3  /**< Set OCnX on compare match                 */
} timer_8_bit_compare_output_mode_t;

/**
 * @brief describes basic timing base configuration for 8 bit timers
*/
typedef struct
{
    uint8_t                           counter;          /**< Main Counter value used to start the timer                             */
    uint8_t                           ocra_val;         /**< Value to be set inside OCRA register to control PWM for instance       */
    uint8_t                           ocrb_val;         /**< Value to be set inside OCRB register to control PWM for instance       */
    timer_8_bit_compare_output_mode_t comp_match_a;     /**< Equivalent to TCCRnA COMnA0 and COMnA1 bits                            */
    timer_8_bit_compare_output_mode_t comp_match_b;     /**< Equivalent to TCCRnA COMnB0 and COMnB1 bits                            */
    timer_8_bit_waveform_generation_t waveform_mode;    /**< Selects the right waveform mode and dispatch it to the right registers */
    timer_8_bit_prescaler_selection_t prescaler;        /**< Selects the right prescaler to be fed in the timer                     */
} timer_8_bit_timing_config_t;

/**
 * @brief describes 8 bit timer interrupt configuration
*/
typedef struct
{
    bool it_comp_match_a;               /**< Enables (or not) the OCIEnA flag (interrupt on compare match A, or not)*/
    bool it_comp_match_b;               /**< Enables (or not) the OCIEnB flag (interrupt on compare match B, or not)*/
    bool it_timer_overflow;             /**< Enables (or not) the TOEIn flag (interrupt on timer overflow, or not)  */
} timer_8_bit_interrupt_config_t;

#ifdef __cplusplus
}
#endif

#endif /* TIMER_8_BIT_REG_HEADER */