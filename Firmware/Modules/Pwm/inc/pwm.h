#ifndef PWM_HEADER
#define PWM_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "io.h"
#include "config.h"
#include "timebase.h"

#ifndef PWM_MAX_INSTANCES
#define PWM_MAX_INSTANCES 1U
#warning "PWM_MAX_INSTANCES macro not defined in config.h, fallsback to 1U by default"
#endif

/**
 * @brief PWM module error types used to inform caller how operations performed.
*/
typedef enum
{
    PWM_ERR_OK,             /**< Operation succeeded, no issues                 */
    PWM_ERR_CONFIG,         /**< Something is wrong with the configuration      */
    PWM_ERR_TIMEBASE_ISSUE, /**< Software timebase has encountered some errors  */
    PWM_ERR_TIMER_ISSUE     /**< Timer drivers encountered some errors          */
} pwm_error_t;

/**
 * @brief defines the 2 types of pwm available in this pwm module
 * They are mainly used to type the compile-time configuration's internal union
*/
typedef enum
{
    PWM_TYPE_SOFTWARE_BASED,    /**< A software based pwm is required (timebase polling)     */
    PWM_TYPE_HARDWARE_BASED     /**< Hardware based pwm uses the hardware timers of the chip */
} pwm_type_t;

/**
 * @brief Compile-time configuration used to configure this Pwm module.
 * Note that a union is used, so soft and hard configurations occupy the same memory space.
*/
typedef struct
{
    pwm_type_t type;    /**< Distinguishes the kind of PWM initialization steps we want to perform, either soft-based or hard-based */
    uint32_t frequency; /**< Initial frequency of this Pwm module instance  */
    uint8_t duty_cycle; /**< Initial duty cycle of this Pwm module instance */
    union
    {
        struct
        {
            io_t io;    /**< Io structure used by the software configuration (ties an io to the pwm module) */
        } soft;         /**< Software based PWM configuration structure                                     */

        struct
        {
            timebase_timer_t targeted_timer;    /**< Tells which kind of timer is targeted by this PWM instance */
            uint8_t timer_index;                /**< Tells which timer we need to use for this PWM instance     */
        } hard;                                 /**< Hardware based PWM configuration structure                 */
    };
} pwm_config_t;

/**
 * @brief initialises this pwm driver using the static configuration written in config.c
 * @return pwm_error_t:
 *      PWM_ERR_OK              : operation succeeded
 *      PWM_ERR_TIMEBASE_ISSUE  : operation did not succeed because of timebase module errors
 *      PWM_ERR_TIMER_ISSUE     : operation did not succeed because of timer drivers errors
*/
pwm_error_t pwm_init(void);

/**
 * @brief processes data for the PWM module (essentially used for the software pwm)
 * @return pwm_error_t:
 *      PWM_ERR_OK              : operation succeeded
 *      PWM_ERR_TIMEBASE_ISSUE  : operation did not succeed because of timebase module errors
 *      PWM_ERR_TIMER_ISSUE     : operation did not succeed because of timer drivers errors
*/
pwm_error_t pwm_process(void);

/**
 * @brief Starts a single PWM. Has no effect if the PWM is already started
 * @return pwm_error_t:
 *      PWM_ERR_OK              : operation succeeded
 *      PWM_ERR_TIMEBASE_ISSUE  : operation did not succeed because of timebase module errors
 *      PWM_ERR_TIMER_ISSUE     : operation did not succeed because of timer drivers errors
*/
pwm_error_t pwm_start(const uint8_t index);

/**
 * @brief Starts all registered PWMs in a row
 * @return pwm_error_t:
 *      PWM_ERR_OK              : operation succeeded
 *      PWM_ERR_TIMEBASE_ISSUE  : operation did not succeed because of timebase module errors
 *      PWM_ERR_TIMER_ISSUE     : operation did not succeed because of timer drivers errors
*/
pwm_error_t pwm_start_all(void);

/**
 * @brief Stops a single PWM. Has no effect if the PWM is already stopped
 * @return pwm_error_t:
 *      PWM_ERR_OK              : operation succeeded
 *      PWM_ERR_TIMEBASE_ISSUE  : operation did not succeed because of timebase module errors
 *      PWM_ERR_TIMER_ISSUE     : operation did not succeed because of timer drivers errors
*/
pwm_error_t pwm_stop(const uint8_t index);

/**
 * @brief Stops all registered PWM instances in a row.
 * @return pwm_error_t:
 *      PWM_ERR_OK              : operation succeeded
 *      PWM_ERR_TIMEBASE_ISSUE  : operation did not succeed because of timebase module errors
 *      PWM_ERR_TIMER_ISSUE     : operation did not succeed because of timer drivers errors
*/
pwm_error_t pwm_stop_all(void);

/**
 * @brief Sets the frequency of a single PWM instance
 * @return pwm_error_t:
 *      PWM_ERR_OK              : operation succeeded
 *      PWM_ERR_TIMEBASE_ISSUE  : operation did not succeed because of timebase module errors
 *      PWM_ERR_TIMER_ISSUE     : operation did not succeed because of timer drivers errors
*/
pwm_error_t pwm_set_frequency(const uint8_t index, const uint32_t * frequency);

/**
 * @brief Sets the duty cycle for a single PWM instance
 * @return pwm_error_t:
 *      PWM_ERR_OK              : operation succeeded
 *      PWM_ERR_TIMEBASE_ISSUE  : operation did not succeed because of timebase module errors
 *      PWM_ERR_TIMER_ISSUE     : operation did not succeed because of timer drivers errors
*/
pwm_error_t pwm_set_duty_cycle(const uint8_t index, const uint8_t duty_cycle);


extern pwm_config_t pwm_config[PWM_MAX_INSTANCES];  /**< Static compile-time configuration for this driver (usually symbol is created in a config.c file) */


#ifdef __cplusplus
}
#endif

#endif /* PWM_HEADER */