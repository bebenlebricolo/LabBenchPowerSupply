#ifndef TIMER_16_BIT_STUB_HEADER
#define TIMER_16_BIT_STUB_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include "timer_16_bit.h"

void timer_16_bit_stub_set_next_parameters(const timer_16_bit_prescaler_selection_t prescaler, const uint16_t ocra, uint32_t * const accumulator);

#ifdef __cplusplus
}
#endif


#endif /* TIMER_16_BIT_STUB_HEADER */