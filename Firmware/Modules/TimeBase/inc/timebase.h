#ifndef TIMEBASE_HEADER
#define TIMEBASE_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef enum
{
    TIMEBASE_ERROR_OK
} timebase_error_t;

typedef enum
{
    TIMEBASE_TIMER_8_BIT,
    TIMEBASE_TIMER_16_BIT,
    TIMEBASE_TIMER_8_BIT_ASYNC,
} timebase_timer_t;

typedef enum
{
    TIMEBASE_TIMESCALE_MICROSECONDS,
    TIMEBASE_TIMESCALE_MILLISECONDS,
    TIMEBASE_TIMESCALE_SECONDS,
} timebase_timescale_t;

typedef struct
{
    struct
    {
        timebase_timer_t type : 2;  /**< Used to select a timer from its type                                       */
        uint8_t index : 6;          /**< Used to select a particular timer from the available ones                  */
    } timer;
    timebase_timescale_t timescale; /**< Selects what is the resolution of the timer                                */
    uint16_t cpu_freq_khz;          /**< Gives the CPU frequency to compute the right prescaler for the timebase    */
} timebase_config_t;

timebase_error_t timebase_init(uint8_t id, timebase_config_t const * const config);
timebase_error_t timebase_deinit(uint8_t id);

timebase_error_t timebase_get_tick(uint8_t id, uint16_t * tick);
timebase_error_t timebase_get_duration(uint16_t reference, uint16_t new_tick, uint16_t * duration);
timebase_error_t timebase_get_duration_now(uint8_t id, uint16_t reference, uint16_t * duration);



#ifdef __cplusplus
}
#endif

#endif /* TIMEBASE_HEADER */