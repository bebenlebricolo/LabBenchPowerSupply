#ifndef THERMISTOR_HEADER
#define THERMISTOR_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "config.h"

#ifndef THERMISTOR_CURVE_MAX_SAMPLES
#define THERMISTOR_CURVE_MAX_SAMPLES 16U
#endif

#ifndef THERMISTOR_MAX_THERMISTORS
#define THERMISTOR_MAX_THERMISTORS 1U
#endif

/**
 * @brief Thermistor sensor error types
*/
typedef enum
{
    THERMISTOR_ERR_OK,
    THERMISTOR_ERR_WRONG_CONFIG,
} thermistor_error_t;

/**
 * @brief thermistor intrinsic type (positive thermal coefficient, negative thermal coefficient, ...)
*/
typedef enum
{
    THERMISTOR_TYPE_NTC,    /**< Negative thermal coefficient thermistor kind */
    THERMISTOR_TYPE_PTC,    /**< Positive thermal coefficient thermistor kind */
} thermistor_type_t;

/**
 * @brief thermistor intrinsic type (positive thermal coefficient, negative thermal coefficient, ...)
*/
typedef enum
{
    THERMISTOR_CURVE_LINEAR,    /**< Linear curve                                                        */
    THERMISTOR_CURVE_DISCRETE,  /**< Discrete points (requires array of data for the modeled thermistor) */
} thermistor_curve_type_t;

/**
 * @brief represents a basic curve using samples
*/
typedef union
{
    struct
    {
        int16_t samples[THERMISTOR_CURVE_MAX_SAMPLES];  /**< Indexes represent the ratio, from 0 to 100% of samples count, each value stores the reference temperature */
        uint8_t count;                                  /**< Stores how many samples are used in this curve                                                            */
    } discrete;

    struct
    {
        int16_t coefficient;                            /**< Linear coefficient using first order approximation                                                         */
    } linear;
} thermistor_curve_t;

/**
 * @brief Describes a thermistor model using all calibration parameters and data coming from the thermistor.
 * We do not rely on
*/
typedef struct
{
    thermistor_type_t type;     /**< thermistor kind (NTC, PTC)                                                  */
    struct
    {
        uint16_t resistance;    /**< Calibration resistance for the modelised thermistance (in Ohms)             */
        int16_t temperature;    /**< Calibration temperature for the modelised thermistance (in Kelvin)          */
    } calibration;
    uint16_t bridge_res;        /**< Divider bridge resistance (in Ohms)                                         */
    uint16_t ref_voltage;       /**< Reference voltage being used by ADC on MCU analog input pin (in millivolts) */
    struct
    {
        thermistor_curve_type_t type;       /**< Thermistor type */
        thermistor_curve_t const * curve;   /**< Thermistor curve (curve is owned externally so it can be reused for several identical thermistors) */
    } curve;
} thermistor_model_t;

#ifdef __cplusplus
}
#endif

#endif /* THERMISTOR_HEADER */