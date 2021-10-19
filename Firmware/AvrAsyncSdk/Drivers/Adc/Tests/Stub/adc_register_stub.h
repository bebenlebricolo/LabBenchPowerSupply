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

#ifndef ADC_REGISTER_STUB
#define ADC_REGISTER_STUB

#ifdef __cplusplus
extern "C" {
#endif

#include "adc.h"

/**
 * @brief mimics adc internal registers
 * Note: it is almost 100% the same structure as adc_handle_t, but contains actual values instead of
 * pointers
 * Note 2 : this structure does not represent by itself the actual behavior of ADC's registers, such as
 * read-only registers, reserved registers, reset register by writing a 1 to it, etc..
 * This might be handled by a dedicated module which continuously checks against a reference to detect changes
 * and react accordingly
*/
typedef struct
{
    volatile uint8_t mux_reg;
    volatile uint8_t adcsra_reg;
    volatile uint8_t  adcsrb_reg;
    struct {
        volatile uint8_t adclow_reg;
        volatile uint8_t adchigh_reg;
    } readings;
} adc_register_stub_t;

/* Implemented in source file */
extern adc_register_stub_t adc_register_stub;

/**
 * @brief resets the entire structure, just like a memset
 * @param[in]   stub : adc_register_stub object to be erased
*/
void adc_register_stub_erase(adc_register_stub_t * stub);

/**
 * @brief initialises adc_handle to point to actual registers which are stubbed ones
 * @param[in]   stub   :    input referenced stub
 * @param[out]  handle :    handle to be initialised
*/
void adc_register_stub_init_adc_handle(adc_handle_t * handle, adc_register_stub_t * const stub);

#ifdef __cplusplus
}
#endif

#endif /* ADC_REGISTER_STUB */