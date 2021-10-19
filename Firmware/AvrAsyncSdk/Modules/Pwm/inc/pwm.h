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

#ifndef PWM_MAX_SOFT_INSTANCES
#define PWM_MAX_SOFT_INSTANCES 1U
#warning "PWM_MAX_SOFT_INSTANCES macro not defined in config.h, fallsback to 1U by default"
#endif

#ifndef PWM_MAX_HARD_INSTANCES
#define PWM_MAX_HARD_INSTANCES 1U
#warning "PWM_MAX_HARD_INSTANCES macro not defined in config.h, fallsback to 1U by default"
#endif

/**
 * @brief PWM module error types used to inform caller how operations performed.
*/
typedef enum
{
    PWM_ERR_OK = 0,         /**< Operation succeeded, no issues                 */
    PWM_ERR_CONFIG,         /**< Something is wrong with the configuration      */
    PWM_ERR_TIMEBASE_ISSUE, /**< Software timebase has encountered some errors  */
    PWM_ERR_TIMER_ISSUE     /**< Timer drivers encountered some errors          */
} pwm_error_t;


/**
 * @brief Compile-time configuration used to configure this Pwm module.
 * Note that a union is used, so soft and hard configurations occupy the same memory space.
*/
typedef struct
{
    timebase_timer_t targeted_timer;    /**< Tells which kind of timer is targeted by this PWM instance */
    uint8_t timer_index;                /**< Tells which timer we need to use for this PWM instance     */
} pwm_hard_static_config_t;

/**
 * @brief Compile-time configuration used to configure this Pwm module.
 * Note that a union is used, so soft and hard configurations occupy the same memory space.
*/
typedef struct
{
    uint8_t io_index;  /**< IO index from the Io lookup table @see static io configuration */
} pwm_soft_static_config_t;

typedef enum
{
    PWM_TYPE_SOFTWARE,
    PWM_TYPE_HARDWARE
} pwm_type_t;

/**
 * @brief initialises this pwm driver using the static configuration written in config.c
 * Note : this functions does not take care about initialising other modules it depends on, this
 * should be handled by application software before this function is called.
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
pwm_error_t pwm_start(const pwm_type_t type, const uint8_t index);

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
pwm_error_t pwm_stop(const pwm_type_t type, const uint8_t index);

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
pwm_error_t pwm_set_frequency(const pwm_type_t type, const uint8_t index, const uint32_t * frequency, const uint32_t * cpu_freq);

/**
 * @brief Sets the duty cycle for a single PWM instance
 * @return pwm_error_t:
 *      PWM_ERR_OK              : operation succeeded
 *      PWM_ERR_TIMEBASE_ISSUE  : operation did not succeed because of timebase module errors
 *      PWM_ERR_TIMER_ISSUE     : operation did not succeed because of timer drivers errors
*/
pwm_error_t pwm_set_duty_cycle(const pwm_type_t type, const uint8_t index, const uint8_t duty_cycle);


// Static configuration for both software based and hardware based pwms
extern pwm_hard_static_config_t pwm_hard_config[PWM_MAX_HARD_INSTANCES];  /**< Static compile-time configuration for this driver (usually symbol is created in a config.c file) */
extern pwm_soft_static_config_t pwm_soft_config[PWM_MAX_SOFT_INSTANCES];  /**< Static compile-time configuration for this driver (usually symbol is created in a config.c file) */


#ifdef __cplusplus
}
#endif

#endif /* PWM_HEADER */