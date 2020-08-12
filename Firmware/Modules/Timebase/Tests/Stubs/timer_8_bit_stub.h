#ifndef TIMER_8_BIT_STUB_HEADER
#define TIMER_8_BIT_STUB_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include "timer_8_bit.h"

void timer_8_bit_stub_set_next_parameters(const timer_8_bit_prescaler_selection_t prescaler, const uint8_t ocra, uint32_t * const accumulator);

// void timer_8_bit_compute_matching_parameters(const uint32_t * const cpu_freq,
//                                              const uint32_t * const target_freq,
//                                              timer_8_bit_prescaler_selection_t * const prescaler,
//                                              uint8_t * const ocra,
//                                              uint32_t * const accumulator);
//
// uint16_t timer_8_bit_prescaler_to_value(const timer_8_bit_prescaler_selection_t prescaler);
// timer_8_bit_prescaler_selection_t timer_8_bit_prescaler_from_value(uint16_t const * const input_prescaler);
// timer_error_t timer_8_bit_stop(uint8_t id);
// timer_error_t timer_8_bit_get_handle(uint8_t id, timer_8_bit_handle_t * const handle);
// timer_error_t timer_8_bit_get_default_config(timer_8_bit_config_t * config);
// timer_error_t timer_8_bit_reconfigure(uint8_t id, timer_8_bit_config_t * const config);


#ifdef __cplusplus
}
#endif


#endif /* TIMER_8_BIT_STUB_HEADER */