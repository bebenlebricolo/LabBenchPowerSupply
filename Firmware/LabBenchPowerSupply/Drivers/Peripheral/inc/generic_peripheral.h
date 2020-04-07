#ifndef GENERIC_PERIPHERAL_HEADER
#define GENERIC_PERIPHERAL_HEADER
#include <stdint.h>

typedef volatile uint16_t peripheral_addr_t;
typedef volatile uint8_t  peripheral_reg_t;
//typedef (volatile uint8_t *) PERIPHERAL_ADDR;

/**
 * @brief gives a restricted set of available errors
*/
typedef enum
{
    PERIPHERAL_ERROR_OK,
    PERIPHERAL_ERROR_FAILED,
    PERIPHERAL_ERROR_CONFIG,
    PERIPHERAL_ERROR_NULL_POINTER
} peripheral_error_t;

/**
 * @brief basic adc peripheral state
*/
typedef enum
{
    PERIPHERAL_STATE_READY,              /**< Adc is initialised and is ready                */
    PERIPHERAL_STATE_BUSY,               /**< A conversion is ongoing, peripheral is busy    */
    PERIPHERAL_STATE_NOT_INITIALISED,    /**< Driver is not initialised                      */
} peripheral_state_t;

/**
 * @brief packs a state alongside an error value.
 * When using functions, peripheral might return different values with different semantics
 * This structure can be useful to discriminate which kind of issue the program is facing
*/
typedef struct
{
    peripheral_error_t error;
    peripheral_state_t state;
} peripheral_returned_t;

#endif /* GENERIC_PERIPHERAL_HEADER */