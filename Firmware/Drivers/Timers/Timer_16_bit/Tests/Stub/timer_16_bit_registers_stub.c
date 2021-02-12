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

#include "timer_16_bit_registers_stub.h"
#include <string.h>

timer_16_bit_registers_stub_t timer_16_bit_registers_stub = {0};

void timer_16_bit_registers_stub_erase(void)
{
    memset(&timer_16_bit_registers_stub, 0, sizeof(timer_16_bit_registers_stub_t));
}

void timer_16_bit_registers_stub_init_handle(timer_16_bit_handle_t * handle)
{
    if (NULL != handle)
    {
        handle->TCCRA = &timer_16_bit_registers_stub.TCCRA;
        handle->TCCRB = &timer_16_bit_registers_stub.TCCRB;
        handle->TCCRC = &timer_16_bit_registers_stub.TCCRC;
        handle->OCRA_H = &timer_16_bit_registers_stub.OCRA_H;
        handle->OCRA_L = &timer_16_bit_registers_stub.OCRA_L;
        handle->OCRB_H = &timer_16_bit_registers_stub.OCRB_H;
        handle->OCRB_L = &timer_16_bit_registers_stub.OCRB_L;
        handle->TCNT_H = &timer_16_bit_registers_stub.TCNT_H;
        handle->TCNT_L = &timer_16_bit_registers_stub.TCNT_L;
        handle->ICR_H = &timer_16_bit_registers_stub.ICR_H;
        handle->ICR_L = &timer_16_bit_registers_stub.ICR_L;
        handle->TIMSK = &timer_16_bit_registers_stub.TIMSK;
        handle->TIFR = &timer_16_bit_registers_stub.TIFR;
    }
}


