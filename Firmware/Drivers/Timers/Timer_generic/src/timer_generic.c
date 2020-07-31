#include "timer_generic.h"

void timer_generic_compute_parameters(timer_generic_parameters_t * const parameters)
{
    const uint32_t freq_ratio = parameters->input.cpu_frequency / parameters->input.target_frequency;
    const uint16_t limit_value = (parameters->input.resolution == TIMER_GENERIC_RESOLUTION_8_BIT) ? (TIMER_GENERIC_8_BIT_LIMIT_VALUE - 1) : (TIMER_GENERIC_16_BIT_LIMIT_VALUE - 1);

    // It is possible that this operation produces aliasing because we do not check if
    // the remainder of this division is exactly 0 (no remainder, clean euclidean division)
    const uint32_t min_prescaler = freq_ratio / (uint32_t) limit_value;

    parameters->output.prescaler = 1U;
    uint16_t target_prescaler = 1U;

    for (uint8_t i = 0 ; i < parameters->input.prescaler_lookup_array.size ; i++)
    {
        parameters->output.prescaler = parameters->input.prescaler_lookup_array.array[i].value;
        target_prescaler = parameters->input.prescaler_lookup_array.array[i].value;
        if (parameters->input.prescaler_lookup_array.array[i].value >= min_prescaler)
        {
            break;
        }
    }

    uint32_t computed_ocra = 0;
    parameters->output.accumulator = 0;
    if (0 != target_prescaler)
    {
        // It is possible that this operation produces aliasing because we do not check if
        // the remainder of this division is exactly 0 (no remainder, clean euclidean division)
        computed_ocra = freq_ratio / (uint32_t) target_prescaler;
    }

    // Happens when timescale is really large compared to CPU frequency
    // We have to create an accumulator which will act as a second-stage prescaler
    if (computed_ocra >= limit_value)
    {
        // Select a remainder arbitrarily high to start the algorithm
        uint16_t min_remainder = 50U;
        parameters->output.prescaler = target_prescaler;

        // linear search, starting from the end of the resolution range
        // This will select the greatest value of OCRA while trying to minize the remainder.
        // Note that 1 is the only candidate for which the remainder will always be 0.
        // If this takes too long / too much computing power, fallback using 1 and use the accumulator value to
        // account for the remaining values.
        for (uint16_t i = limit_value ; i >= 1 ; i--)
        {
            uint16_t remainder = (computed_ocra % i);
            if ((0 != remainder) && (remainder < min_remainder))
            {
                min_remainder = remainder;
            }

            if (0 == remainder)
            {
                parameters->output.accumulator = (computed_ocra / i) - 1;
                computed_ocra = i;
                break;
            }
        }

        // If no suitable number was found, fallback on 1 and set the accumulator to computed_ocra old value.
        if (0 == parameters->output.accumulator)
        {
            parameters->output.accumulator = computed_ocra - 1;
            computed_ocra = 1U;
        }
    }

    if (computed_ocra != 0)
    {
        parameters->output.ocra = (computed_ocra - 1U);
    }
    else
    {
        //TODO : We might be a bit off. In this case, normally we would have to raise the prescaler one step further and recompute ocra value.
        parameters->output.ocra = computed_ocra;
    }
}
