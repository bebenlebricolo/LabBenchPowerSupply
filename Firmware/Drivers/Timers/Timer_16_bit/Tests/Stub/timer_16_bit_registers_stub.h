/*

------------------
@<FreeMyCode>
FreeMyCode version : 1.0 RC alpha
    Author : bebenlebricolo
    License : 
        name : GPLv3
        url : https://www.gnu.org/licenses/quick-guide-gplv3.html
    Date : 12/02/2021
    Project : LabBenchPowerSupply
    Description : The Lab Bench Power Supply provides a simple design based around an Arduino Nano board to convert AC main voltage into
 smaller ones, ranging from 0V to 16V, with voltage and current regulations
<FreeMyCode>@
------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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