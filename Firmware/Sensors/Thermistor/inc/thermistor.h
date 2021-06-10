#ifndef THERMISTOR_HEADER
#define THERMISTOR_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

#include "adc.h"
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
    THERMISTOR_ERR_INDEX_OUT_OF_BOUNDS,
} thermistor_error_t;


/**
 * @brief thermistor intrinsic type (positive thermal coefficient, negative thermal coefficient, ...)
*/
typedef enum
{
    THERMISTOR_CURVE_LINEAR,    /**< Linear curve                                                        */
    THERMISTOR_CURVE_DISCRETE,  /**< Discrete points (requires array of data for the modeled thermistor) */
} thermistor_curve_type_t;

typedef struct
{
    uint8_t ratio;                        /**< Rtherm/Rcalibration                                          */
    int8_t  temperature;                  /**< matching temperature in Celsius degrees (°C)                 */
} thermistor_curve_discrete_pair_t;

/**
 * @brief discrete curve implementation. Curve should be filled with consecutive data points with no doubles
 * List of points should be ordered as well.
*/
typedef struct
{
    thermistor_curve_discrete_pair_t hot_half[THERMISTOR_CURVE_MAX_SAMPLES / 2U + 1];   /**< Hot half curve (ratio <= 1)    */
    thermistor_curve_discrete_pair_t cold_half[THERMISTOR_CURVE_MAX_SAMPLES / 2U + 1];  /**< Cold half curve (ratio >= 1)   */
    uint8_t count;                            /**< Stores how many samples are used in this curve               */
} thermistor_curve_discrete_t;

/**
 * @brief linear curve using classic affine equation of type T = a.X + b
 * where :
 *  - T : temperature (Kelvin)
 *  - a : temperature coefficient
 *  - b : offset
*/
typedef struct
{
    int8_t coefficient;                       /**< Linear coefficient using first order approximation      */
    int8_t offset;                            /**< Offset for linear equation                              */
} thermistor_curve_linear_t;

/**
 * @brief represents a basic curve using samples
*/
typedef struct
{
    thermistor_curve_type_t type;
    union
    {
        thermistor_curve_discrete_t * discrete;
        thermistor_curve_linear_t * linear;
    } data;
} thermistor_curve_t;

/**
 * @brief Describes a thermistor model using all calibration parameters and data coming from the thermistor.
 * We do not rely on
*/
typedef struct
{
    struct
    {
        uint8_t resistance;         /**< Calibration resistance for the modelised thermistance (in kOhms)            */
        int8_t temperature;         /**< Calibration temperature for the modelised thermistance (in Kelvin)          */
    } calibration;

    uint8_t bridge_res;             /**< Divider bridge resistance (in kOhms)                                        */
    adc_millivolts_t ref_voltage;   /**< Reference voltage being used by ADC on MCU analog input pin (in millivolts) */
    thermistor_curve_t curve;       /**< Thermistor modeled curve (either linear equation or discrete curve)         */
} thermistor_model_t;

/**
 * @brief configuration structure for thermistor driver
*/
typedef struct
{
    adc_mux_t adc_index;          /**< Index of adc input to be requested to adc driver   */
    thermistor_model_t model;   /**< Thermistor model used                              */
} thermistor_config_t;

/**
 * @brief configures thermistor driver using static configuration (@see thermistor_driver_config symbol)
 * @return thermistor driver errors
*/
thermistor_error_t thermistor_init(void);

/**
 * @brief reads temperature for a given sensor (fetched data from internal data bank, thermistor_process function should be called to
 * update stored data)
 * @see thermistor_process
 * @param[in]   index   : index of targeted thermistor driver
 * @param[out]  temp    : computed temperature
*/
thermistor_error_t thermistor_read(const uint8_t index, int8_t * temp);

/**
 * @brief scans all registered thermistors and compute temperatures
*/
thermistor_error_t thermistor_process(void);

/**
 * @brief this extern symbol has to be implemented by application code. Usually, this is done in a config.c file
 * whose symbols are linked when building the whole application
*/
extern thermistor_config_t thermistor_driver_config[THERMISTOR_MAX_THERMISTORS];

#ifdef __cplusplus
}
#endif

#endif /* THERMISTOR_HEADER */