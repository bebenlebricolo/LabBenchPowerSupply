#include "io.h"
#include "pwm.h"
#include "timebase.h"
#include "timer_8_bit.h"
#include "timer_16_bit.h"
#include "timer_8_bit_async.h"

/**
 * @brief Static storage for pwm configuration
 * This is done this way to enhance the dynamic nature of PWMs, so that we can
 * still override their config at anytime (especially frequencies and duty cycles, not)
*/
static struct
{
    uint32_t frequency;     /**< Initial frequency of this Pwm module instance          */
    uint8_t duty_cycle;     /**< Initial duty cycle of this Pwm module instance         */

    uint16_t last_tick;     /**< Holds the last tick retrieved from timebase module     */
    uint16_t start_tick;    /**< Holds the start tick retrieved from timebase module    */
} soft_config[PWM_MAX_SOFT_INSTANCES] = {0};


static pwm_error_t pwm_init_single_soft(const uint8_t index);
static pwm_error_t pwm_init_single_hard(const uint8_t index);

pwm_error_t pwm_init(void)
{
    pwm_error_t err = PWM_ERR_OK;
    for (uint8_t i = 0 ; i < PWM_MAX_HARD_INSTANCES ; i++)
    {
        err |= pwm_init_single_hard(i);
    }

    for (uint8_t i = 0 ; i < PWM_MAX_SOFT_INSTANCES ; i++)
    {
        err |= pwm_init_single_soft(i);
    }

    if (err != PWM_ERR_OK)
    {
        err = PWM_ERR_CONFIG;
    }

    return err;
}

static pwm_error_t pwm_init_single_hard(const uint8_t index)
{
    pwm_error_t ret = PWM_ERR_OK;
    pwm_hard_static_config_t * config = &pwm_hard_config[index];
    timer_error_t timerr = TIMER_ERROR_OK;

    // Configuration varies accross the several timer drivers
    switch(config->targeted_timer)
    {
        case TIMEBASE_TIMER_8_BIT:
        {
            timer_8_bit_config_t config;
            //config.force_compare.force_comp_match_a
            //timerr = timer_8_bit_stop(config->timer_index);
        }
            break;

        case TIMEBASE_TIMER_8_BIT_ASYNC:
            timerr = timer_8_bit_async_stop(config->timer_index);
            break;

        case TIMEBASE_TIMER_16_BIT:
            timerr = timer_16_bit_stop(config->timer_index);
            break;

        default:
            ret = PWM_ERR_CONFIG;
            break;
    }

    if (TIMER_ERROR_OK != timerr)
    {
        ret = PWM_ERR_CONFIG;
    }

    return ret;
}

static pwm_error_t pwm_init_single_soft(const uint8_t index)
{
    pwm_error_t ret = PWM_ERR_OK;
    io_write(pwm_soft_config[index].io_index, IO_STATE_LOW);
    return ret;
}

pwm_error_t pwm_set_frequency(const pwm_type_t type, const uint8_t index, const uint32_t * frequency, const uint32_t * cpu_freq)
{
    pwm_error_t ret = PWM_ERR_OK;
    if (PWM_TYPE_HARDWARE == type)
    {
        timebase_config_t config;
        timebase_error_t timerr = TIMEBASE_ERROR_OK;
        uint16_t prescaler_val = 0;
        uint16_t ocr_value = 0;
        uint16_t accumulator = 0;

        config.cpu_freq = *cpu_freq;
        config.custom_target_freq = *frequency;
        config.timescale = TIMEBASE_TIMESCALE_CUSTOM;
        timerr = timebase_compute_timer_parameters(&config, &prescaler_val, &ocr_value, &accumulator);
        switch(pwm_hard_config[index].targeted_timer)
        {
            case TIMEBASE_TIMER_8_BIT:
                //timerr = timer_8_bit_
                break;

            case TIMEBASE_TIMER_8_BIT_ASYNC:
                timerr = timer_8_bit_async_stop(pwm_hard_config[index].timer_index);
                break;

            case TIMEBASE_TIMER_16_BIT:
                timerr = timer_16_bit_stop(pwm_hard_config[index].timer_index);
                break;

            default:
                ret = PWM_ERR_CONFIG;
                break;

        }
    }
    else
    {
        soft_config[index].frequency = *frequency;
    }

    return ret;
}
