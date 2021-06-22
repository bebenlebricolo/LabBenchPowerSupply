#ifndef THERMISTOR_PRIVATE_HEADER
#define THERMISTOR_PRIVATE_HEADER


#ifdef __cplusplus
extern "C"
{
#endif

#include "thermistor.h"

/**
 * @brief this enum is used internally to correctly express the resistance ratio number
 * without loosing too much of the precious digits, but fitting within a uint8_t enveloppe.
 * Handling this kind of things adds a bit more code to the program (bigger flash footprint) but allows
 * more fine-grain computation over a wide range of ratios number, ensuring ratios stay within the 0-255 range.
*/
typedef enum
{
    RATIO_COEF_1 = 1U,      /**< Ratio is expressed without scaling (1:1) scale                         */
    RATIO_COEF_10 = 10U,    /**< Ratio is expressed as 10 times the actual effective ratio (10:1) scale */
    RATIO_COEF_100 = 100U,  /**< Ratio is expressed as 100 times the actual ratio (100:1) scale         */
} ratio_coefficients_t;

/**
 * @brief implements a dichotomic search (binary search) to help finding the right
 * data interval. Data is rejected whenever given ratio is out of bounds, and in case of exact match,
 * both lower_bound and upper_bound points to the same data pair.
 * @param[in] curve         : input curve used to find the including interval
 * @param[in] ratio         : thermistor resistance ratio (nominal resistance over calibration resistance)
 * @param[out] lower_bound  : lower bound of the found interval
 * @param[out] upper_bound  : upper bound of the found interval
 * @return true or false. false means input ratio is out of bounds ; this algorithm does not extrapolate.
*/
bool find_segment(thermistor_curve_discrete_t const * const curve,
                  const uint8_t ratio,
                  const ratio_coefficients_t coef,
                  thermistor_curve_discrete_pair_t const ** lower_bound,
                  thermistor_curve_discrete_pair_t const ** upper_bound);

/**
 * @brief computes a temperature using sensor index and an appropriate sensor reading
 * @param[in] index     : sensor index
 * @param[in] reading   : analogic value read from the sensor's adc pin
 * @return processed temperature value.
 *  INT8_MIN is returned in case of error (@see limits.h)
*/
int8_t compute_temperature(const uint8_t index, adc_millivolts_t * reading);


/**
 * @brief calculates thermistor ratio (Rth/R0) where :
 *  - Rth : current resistance of thermistance
 *  - R0  : calibration resistance of thermistance (usually 25°C / 298.15K)
 * And returns a ratio_coefficient_t enum value to indicate the scale used by the ratio number to accurately represent it later.
 *
 * For instance, ratio could be encoded with :
 *   | scale | ratio values |
 *   | 1:1   |    >= 10     |
 *   | 10:1  |    [1,10]    |
 *   | 100:1 |    [0,1]     |
 *
 * @param[in]
*/
ratio_coefficients_t compute_ratio(thermistor_model_t const * const model, const adc_millivolts_t * reading, uint8_t * const ratio);


#ifdef __cplusplus
}
#endif

#endif /* THERMISTOR_PRIVATE_HEADER */