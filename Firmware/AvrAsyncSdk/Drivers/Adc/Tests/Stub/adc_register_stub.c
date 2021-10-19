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

#include "adc_register_stub.h"
#include <memory.h>

adc_register_stub_t adc_register_stub = {0};

void adc_register_stub_erase(adc_register_stub_t * stub)
{
    memset(stub, 0, sizeof(adc_register_stub_t));
}

void adc_register_stub_init_adc_handle(adc_handle_t * handle, adc_register_stub_t * const stub)
{
    handle->adcsra_reg = &stub->adcsra_reg;
    handle->adcsrb_reg = &stub->adcsrb_reg;
    handle->mux_reg = &stub->mux_reg;
    handle->readings.adchigh_reg = &stub->readings.adchigh_reg;
    handle->readings.adclow_reg = &stub->readings.adclow_reg;
}
