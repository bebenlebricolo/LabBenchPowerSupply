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

#include "timer_8_bit_async_registers_stub.h"
#include <string.h>

timer_8_bit_async_registers_stub_t timer_8_bit_async_registers_stub = {0};

void timer_8_bit_async_registers_stub_erase(void)
{
    memset(&timer_8_bit_async_registers_stub, 0, sizeof(timer_8_bit_async_registers_stub_t));
}

void timer_8_bit_async_registers_stub_init_handle(timer_8_bit_async_handle_t * handle)
{
    if (NULL != handle)
    {
        handle->OCRA = &timer_8_bit_async_registers_stub.OCRA;
        handle->OCRB = &timer_8_bit_async_registers_stub.OCRB;
        handle->TCCRA = &timer_8_bit_async_registers_stub.TCCRA;
        handle->TCCRB = &timer_8_bit_async_registers_stub.TCCRB;
        handle->TCNT = &timer_8_bit_async_registers_stub.TCNT;
        handle->TIMSK = &timer_8_bit_async_registers_stub.TIMSK;
        handle->TIFR = &timer_8_bit_async_registers_stub.TIFR;
        handle->ASSR_REG = &timer_8_bit_async_registers_stub.ASSR_REG;
    }
}


