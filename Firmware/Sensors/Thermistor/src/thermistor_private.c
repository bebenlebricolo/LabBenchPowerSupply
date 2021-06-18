#include "thermistor_private.h"

bool find_segment(thermistor_curve_discrete_t const * const curve,
                  const uint8_t ratio,
                  thermistor_curve_discrete_pair_t const ** lower_bound,
                  thermistor_curve_discrete_pair_t const ** upper_bound)
{
    uint8_t index = 0;
    uint8_t upper_idx = curve->count - 1;
    uint8_t lower_idx = 0;

    // Reject out of bounds values
    if ((ratio > curve->data[0].ratio)
     && (ratio < curve->data[curve->count - 1].ratio))
    {
        return false;
    }

    while((upper_idx - lower_idx) > 1)
    {
        index = ((upper_idx - lower_idx) / 2) + lower_idx;
        uint8_t current_ratio = curve->data[index].ratio;

        // Exact match, we need to return the two exact same data pair then
        if (ratio == current_ratio)
        {
            lower_idx = index;
            upper_idx = index;
            break;
        }

        // Curves are written in reverse order (maximum ratio first, cold temperatures first)
        if (ratio > current_ratio)
        {
            upper_idx = index;
        }
        else
        {
            lower_idx = index;
        }
    }

    *lower_bound = &curve->data[lower_idx];
    *upper_bound = &curve->data[upper_idx];

    return true;
}