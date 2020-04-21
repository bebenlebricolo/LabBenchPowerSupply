#ifndef TIMER_16_BIT_REGISTERS_STUB
#define TIMER_16_BIT_REGISTERS_STUB

#include "timer_16_bit.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief provides a stub interface representing 8 bit timers registers
*/
typedef struct
{
    volatile uint8_t TCCRA;  /**< Timer Counter Control register A               */
    volatile uint8_t TCCRB;  /**< Timer Counter Control register B               */
    volatile uint8_t TCCRC;  /**< Timer Counter Control register C               */
    volatile uint8_t OCRA_H; /**< Output Compare value register A high half      */
    volatile uint8_t OCRA_L; /**< Output Compare value register A low half       */
    volatile uint8_t OCRB_H; /**< Output Compare value register B high half      */
    volatile uint8_t OCRB_L; /**< Output Compare value register B low half       */
    volatile uint8_t TCNT_H; /**< Timer Counter main counting register high half */
    volatile uint8_t TCNT_L; /**< Timer Counter main counting register low half  */
    volatile uint8_t ICR_H;  /**< Timer Counter main counting register high half */
    volatile uint8_t ICR_L;  /**< Timer Counter main counting register low half  */
    volatile uint8_t TIMSK;  /**< Timer Interrupt Mask register                  */
    volatile uint8_t TIFR;   /**< Timer Interrupt Flags register                 */
} timer_16_bit_registers_stub_t;

extern timer_16_bit_registers_stub_t timer_16_bit_registers_stub;

void timer_16_bit_registers_stub_erase(void);
void timer_16_bit_registers_stub_init_handle(timer_16_bit_handle_t * handle);

#ifdef __cplusplus
}
#endif

#endif /* TIMER_16_BIT_REGISTERS_STUB */