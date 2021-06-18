#ifndef THERMISTOR_PRIVATE_HEADER
#define THERMISTOR_PRIVATE_HEADER


#ifdef __cplusplus
extern "C"
{
#endif

#include "thermistor.h"

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
                  thermistor_curve_discrete_pair_t const ** lower_bound,
                  thermistor_curve_discrete_pair_t const ** upper_bound);


#ifdef __cplusplus
}
#endif

#endif /* THERMISTOR_PRIVATE_HEADER */