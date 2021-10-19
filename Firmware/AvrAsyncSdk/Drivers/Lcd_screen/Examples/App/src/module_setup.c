#include "module_setup.h"
#include "timebase.h"

module_setup_error_t module_init_timebase(void)
{
    timebase_config_t config = {0};
    config.cpu_freq = 16000000;
    config.timer.index = 0;
    config.timer.type = TIMEBASE_TIMER_8_BIT_ASYNC;
    config.timescale = TIMEBASE_TIMESCALE_MILLISECONDS;
    timebase_error_t err = timebase_init(0U, &config);
    if (TIMEBASE_ERROR_OK != err)
    {
        return MODULE_SETUP_ERROR_INIT_FAILED;
    }
    return MODULE_SETUP_ERROR_OK;
}


