#ifndef TIMEBASE_HEADER
#define TIMEBASE_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/**
 * @brief Describes available error codes for this timebase module
*/
typedef enum
{
    TIMEBASE_ERROR_OK,                      /**< No particular error                                    */
    TIMEBASE_ERROR_NULL_POINTER,            /**< One or more parameters are not initialised properly    */
    TIMEBASE_ERROR_INVALID_INDEX,           /**< Index is not set correctly, probably out of bounds     */
    TIMEBASE_ERROR_TIMER_UNINITIALISED      /**< Underlying timer is not initialised                    */
} timebase_error_t;

/**
 * @brief Allows to select the underlying timer driver
*/
typedef enum
{
    TIMEBASE_TIMER_8_BIT,           /**< Uses a regular 8 bit timer                     */
    TIMEBASE_TIMER_16_BIT,          /**< Uses a regular 16 bit timer                    */
    TIMEBASE_TIMER_8_BIT_ASYNC,     /**< Uses an advanced / async capable 8 bit timer   */
} timebase_timer_t;

/**
 * @brief Selects the reference timebase
*/
typedef enum
{
    TIMEBASE_TIMESCALE_MICROSECONDS,    /**< Microsecond timescale                                                                  */
    TIMEBASE_TIMESCALE_MILLISECONDS,    /**< Millisecond timescale                                                                  */
    TIMEBASE_TIMESCALE_SECONDS,         /**< Second timescale                                                                       */
    TIMEBASE_TIMESCALE_CUSTOM,          /**< Custom timescale, allows to use a custom configuration to handle timebase generation   */
} timebase_timescale_t;

/**
 * @brief Initialisation structure
*/
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

/**
 * @brief Initialises the timebase module using an id and a configuration.
 * @param[in] id     :  index of timebase module to be initialised
 * @param[in] config :  configuration to be used to initialise the targeted timebase module
 * @return
 *          TIMEBASE_ERROR_OK               :   operation succeeded
 *          TIMEBASE_ERROR_NULL_POINTER     :   given parameter is uninitialised
 *          TIMEBASE_ERROR_INVALID_INDEX    :   given module id is out of bounds
*/
timebase_error_t timebase_init(const uint8_t id, timebase_config_t const * const config);

/**
 * @brief Deinitialises targeted timebase module
 * @param[in] id    :   targeted timebase module index
 * @return
 *          TIMEBASE_ERROR_OK               :   operation succeeded
 *          TIMEBASE_ERROR_INVALID_INDEX    :   given module id is out of bounds
*/
timebase_error_t timebase_deinit(const uint8_t id);

/**
 * @brief Reads the current tick from underlying timer/accumulator
 * @param[in]   id      : index of targeted timebase module
 * @param[out]  tick    : output tick read from underlying timer/accumulator
 * @return
 *          TIMEBASE_ERROR_OK               :   operation succeeded
 *          TIMEBASE_ERROR_NULL_POINTER     :   given parameter is uninitialised
 *          TIMEBASE_ERROR_INVALID_INDEX    :   given module id is out of bounds
*/
timebase_error_t timebase_get_tick(const uint8_t id, uint16_t * const tick);

/**
 * @brief Computes a duration using two reference ticks
 * @param[in]  reference : input reference tick (used as a 'start' time)
 * @param[in]  new_tick  : tick to be compared against the reference
 * @param[out] duration  : calculated duration
 * @return
 *          TIMEBASE_ERROR_OK               :   operation succeeded
 *          TIMEBASE_ERROR_NULL_POINTER     :   given parameter is uninitialised
*/
timebase_error_t timebase_get_duration(uint16_t const * const reference, uint16_t const * const new_tick, uint16_t * const duration);

/**
 * @brief Computes the duration between a reference tick and now (fetched when this function is called)
 * @param[in]  id          : index of targeted timebase module
 * @param[in]  reference   : input reference tick (used as a 'start' time)
 * @param[out] duration    : calculated duration
 * @return
 *          TIMEBASE_ERROR_OK               :   operation succeeded
 *          TIMEBASE_ERROR_NULL_POINTER     :   given parameter is uninitialised
 *          TIMEBASE_ERROR_INVALID_INDEX    :   given module id is out of bounds
*/
timebase_error_t timebase_get_duration_now(const uint8_t id, uint16_t const * const reference, uint16_t * const duration);



#ifdef __cplusplus
}
#endif

#endif /* TIMEBASE_HEADER */