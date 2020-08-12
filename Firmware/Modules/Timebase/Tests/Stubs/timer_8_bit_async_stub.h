#ifndef TIMER_8_BIT_ASYNC_STUB_HEADER
#define TIMER_8_BIT_ASYNC_STUB_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include "timer_8_bit_async.h"

void timer_8_bit_async_stub_set_next_parameters(const timer_8_bit_async_prescaler_selection_t prescaler, const uint8_t ocra, uint32_t * const accumulator);

#ifdef __cplusplus
}
#endif


#endif /* TIMER_8_BIT_ASYNC_STUB_HEADER */