#ifndef TIMER_REG_HEADER
#define TIMER_REG_HEADER


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

/**
 * @brief describes basic timing base configuration for 8 bit timers
*/
typedef struct
{
    timer_8_bit_compare_output_mode_t comp_match_a;                /**< Equivalent to TCCRnA COMnA0 and COMnA1 bits                            */
    timer_8_bit_compare_output_mode_t comp_match_b;                /**< Equivalent to TCCRnA COMnB0 and COMnB1 bits                            */
    timer_8_bit_waveform_generation_t waveform_mode;            /**< Selects the right waveform mode and dispatch it to the right registers */
    timer_x_bit_prescaler_selection_t prescaler;                /**< Selects the right prescaler to be fed in the timer                     */
} timer_8_bit_timing_config_t;

/**
 * @brief describes 8 bit timer interrupt configuration
*/
typedef struct
{
    bool it_comp_match_a;   /**< Enables (or not) the OCIEnA flag (interrupt on compare match A, or not)*/
    bool it_comp_match_b;   /**< Enables (or not) the OCIEnB flag (interrupt on compare match B, or not)*/
    bool it_timer_overflow; /**< Enables (or not) the TOEIn flag (interrupt on timer overflow, or not)  */
} timer_8_bit_interrupt_config_t;


/* #########################################################################################
   ############################## 16 bits regular timers ###################################
   ######################################################################################### */

/**
 * @brief Describes 16 bits timers waveform generation modes
*/
typedef enum
{                                                               /**< |   Mode of operation        |   TOP       |  Update of OCRx at |  TOV Flag set on |*/
    TIMER16BIT_WG_NORMAL                                = 0U,   /**< | Normal operation mode      |   0xFFFF    |       Immediate    |      MAX         |*/
/* ----------------------------------------------------------------------------------------------------------------------------------------------------- */
    TIMER16BIT_WG_PWM_PHASE_CORRECT_8_bit_FULL_RANGE    = 1U,   /**< | PWM, phase correct 8 bits  |   0x00FF    |       TOP          |      BOTTOM      |*/
    TIMER16BIT_WG_PWM_PHASE_CORRECT_9_bit_FULL_RANGE    = 2U,   /**< | PWM, phase correct 9 bits  |   0x01FF    |       TOP          |      BOTTOM      |*/
    TIMER16BIT_WG_PWM_PHASE_CORRECT_10_bit_FULL_RANGE   = 3U,   /**< | PWM, phase correct 10 bits |   0x03FF    |       TOP          |      BOTTOM      |*/
/* ----------------------------------------------------------------------------------------------------------------------------------------------------- */
    TIMER16BIT_WG_PWM_FAST_8_bit_FULL_RANGE             = 5U,   /**< | Fast PWM 8 bits            |   0x00FF    |       BOTTOM       |      TOP         |*/
    TIMER16BIT_WG_PWM_FAST_9_bit_FULL_RANGE             = 6U,   /**< | Fast PWM 9 bits            |   0x01FF    |       BOTTOM       |      TOP         |*/
    TIMER16BIT_WG_PWM_FAST_10_bit_FULL_RANGE            = 7U,   /**< | Fast PWM 10 bits           |   0x03FF    |       BOTTOM       |      TOP         |*/
    TIMER16BIT_WG_PWM_FAST_ICR_MAX                      = 14U,  /**< | Fast PWM                   |   ICR       |       BOTTOM       |      TOP         |*/
    TIMER16BIT_WG_PWM_FAST_OCRA_MAX                     = 15U,  /**< | Fast PWM                   |   OCRA      |       BOTTOM       |      TOP         |*/
/* ----------------------------------------------------------------------------------------------------------------------------------------------------- */
    TIMER16BIT_WG_PWM_PHASE_AND_FREQ_CORRECT_ICR_MAX    = 8U,   /**< | PWM, phase and freq        |   ICR       |       BOTTOM       |      BOTTOM      |*/
    TIMER16BIT_WG_PWM_PHASE_AND_FREQ_CORRECT_OCRA_MAX   = 9U,   /**< | PWM, phase and freq        |   OCRA      |       BOTTOM       |      BOTTOM      |*/
    TIMER16BIT_WG_PWM_PHASE_CORRECT_ICR_MAX             = 10U,  /**< | PWM, phase correct         |   ICR       |       TOP          |      BOTTOM      |*/
    TIMER16BIT_WG_PWM_PHASE_CORRECT_OCRA_MAX            = 11U,  /**< | PWM, phase correct         |   OCRA      |       TOP          |      BOTTOM      |*/
/* ----------------------------------------------------------------------------------------------------------------------------------------------------- */
    TIMER16BIT_WG_CTC_ICR_MAX                           = 12U,  /**< | Clear Timer Compare match  |   ICR       |       Immediate    |      MAX         |*/
    TIMER16BIT_WG_CTC_OCRA_MAX                          = 4U,   /**< | Clear Timer Compare match  |   OCRA      |       Immediate    |      MAX         |*/
} timer_16_bit_waveform_generation_t;

/**
 * @brief describes 16 bit timers output compare modes
 * Note : cautiously read device's datasheet to implement the exact mode you need,
 * this 'generic' interface does not represent all use cases, despite being very close to the
 * actual behavior of the device.
*/
typedef enum
{
    TIMER16BIT_CMOD_NORMAL       = 0, /**< Normal operation mode, no automatic output */
    TIMER16BIT_CMOD_TOGGLE_OCnX  = 1, /**< Toggle OCnX on compare match               */
    TIMER16BIT_CMOD_CLEAR_OCnX   = 2, /**< Clear OCnX on compare match                */
    TIMER16BIT_CMOD_SET_OCnX     = 3  /**< Set OCnX on compare match                  */
} timer_16_bit_compare_output_mode_t;

/**
 * @brief describes basic timing base configuration for 16 bit timers
*/
typedef struct
{
    timer_16_bit_compare_output_mode_t comp_match_a;  /**< Equivalent to TCCRnA COMnA0 and COMnA1 bits                            */
    timer_16_bit_compare_output_mode_t comp_match_b;  /**< Equivalent to TCCRnA COMnB0 and COMnB1 bits                            */
    timer_16_bit_waveform_generation_t waveform_mode; /**< Selects the right waveform mode and dispatch it to the right registers */
    timer_x_bit_prescaler_selection_t  prescaler;     /**< Selects the right prescaler to be fed in the timer                     */
} timer_16_bit_timing_config_t;

/**
 * @brief describes the input capture edge selection flag state
 */
typedef enum
{
    TIMER16BIT_INPUT_CAPTURE_EDGE_RISING_EDGE    = 1,
    TIMER16BIT_INPUT_CAPTURE_EDGE_FALLING_EDGE   = 0
} timer_16_bit_input_capture_edge_select_flag_t;

/**
 * @brief describes how the input noise canceler is configured
*/
typedef struct
{
    timer_16_bit_input_capture_edge_select_flag_t edge_select;  /**< Selects which edge to be used when          */
    bool use;                                                   /**< Triggers input capture noise canceler usage */
} timer_16_bit_input_capture_noise_canceler_config_t;

/**
 * @brief describes 16 bit timer interrupt configuration
*/
typedef struct
{
    bool it_input_capture;  /**< Enables (or not) the TOEIn flag (interrupt on timer overflow, or not)   */
    bool it_comp_match_a,;  /**< Enables (or not) the OCIEnA flag (interrupt on compare match A, or not) */
    bool it_comp_match_b;   /**< Enables (or not) the OCIEnB flag (interrupt on compare match B, or not) */
    bool it_timer_overflow; /**< Enables (or not) the TOEIn flag (interrupt on timer overflow, or not)   */
} timer_16_bit_interrupt_config_t;



#endif /* TIMER_REG_HEADER */