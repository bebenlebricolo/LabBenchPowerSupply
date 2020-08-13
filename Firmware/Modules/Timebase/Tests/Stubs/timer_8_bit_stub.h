#ifndef TIMER_8_BIT_STUB_HEADER
#define TIMER_8_BIT_STUB_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include "timer_8_bit.h"
#define TIMER_8_BIT_STUB_MAX_INSTANCES (1U)

void timer_8_bit_stub_set_next_parameters(const timer_8_bit_prescaler_selection_t prescaler, const uint8_t ocra, const uint32_t accumulator);
void timer_8_bit_stub_set_initialised(const bool initialised);
void timer_8_bit_stub_reset(void);
void timer_8_bit_stub_get_driver_configuration(timer_8_bit_config_t * const config);

#ifdef __cplusplus
}
#endif


#endif /* TIMER_8_BIT_STUB_HEADER */