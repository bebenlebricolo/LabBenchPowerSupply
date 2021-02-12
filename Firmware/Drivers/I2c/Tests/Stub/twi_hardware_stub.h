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

#ifndef TWI_HARDWARE_STUB_HEADER
#define TWI_HARDWARE_STUB_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "i2c_device_interface.h"

void twi_hardware_stub_process(const uint8_t id);

void twi_hardware_stub_clear(void);

void twi_hardware_stub_get_interface(const uint8_t bus_id, i2c_device_interface_t ** const p_interface);

bool twi_hardware_stub_is_busy(const uint8_t id);

#ifdef __cplusplus
}
#endif


#endif /* TWI_HARDWARE_STUB_HEADER */