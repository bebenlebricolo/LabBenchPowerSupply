#ifndef TIMER_GENERIC_REG_HEADER
#define TIMER_GENERIC_REG_HEADER

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/* #########################################################################################
   ############################## Generic types for timers #################################
   ######################################################################################### */

/**
 * @brief generic structure which holds timer error types
*/
typedef enum
{
    TIMER_ERROR_OK,               /**< Everything went well so far                                  */
    TIMER_ERROR_CONFIG,           /**< Given configuration is not well-formed                       */
    TIMER_ERROR_NULL_POINTER,     /**< One or more parameters were set to NULL                      */
    TIMER_ERROR_NULL_HANDLE,      /**< Timer handle still points to NULL                            */
    TIMER_ERROR_UNKNOWN_TIMER,    /**< Given timer id exceeds the range of registered timers        */
    TIMER_ERROR_NOT_INITIALISED,  /**< Given configuration is not well-formed                       */
    TIMER_ERROR_REGISTER_IS_BUSY, /**< Selected register cannot be written/read : register is busy  */
} timer_error_t;

#define TIMER_GENERIC_8_BIT_LIMIT_VALUE (256U)
#define TIMER_GENERIC_16_BIT_LIMIT_VALUE (65536U)

typedef enum
{
    TIMER_GENERIC_RESOLUTION_8_BIT,
    TIMER_GENERIC_RESOLUTION_16_BIT,
} timer_generic_resolution_t;

typedef struct
{
    uint16_t value : 11;
    uint16_t type : 5;
} timer_generic_prescaler_pair_t;

typedef struct
{
    struct
    {
        uint32_t cpu_frequency;
        uint32_t target_frequency;
        timer_generic_resolution_t resolution;
        struct
        {
            timer_generic_prescaler_pair_t const * array;
            uint8_t size;
        } prescaler_lookup_array;
    } input;

    struct
    {
        uint16_t prescaler;
        uint16_t ocra;
        uint32_t accumulator;
    } output;
} timer_generic_parameters_t;

void timer_generic_compute_parameters(timer_generic_parameters_t * const parameters);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_GENERIC_REG_HEADER */