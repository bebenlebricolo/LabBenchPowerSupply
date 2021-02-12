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

#include "i2c_register_stub.h"
#include "i2c.h"
#include "string.h"

i2c_register_stub_t i2c_register_stub[I2C_DEVICES_COUNT] = {0};

void i2c_register_stub_erase(const uint8_t id)
{
    memset(&i2c_register_stub[id], 0, sizeof(i2c_register_stub_t));

    // TWINT is set to indicate peripheral is stopped and its registers can be manipulated
    i2c_register_stub[id].twcr_reg |= TWINT_MSK;
}

void i2c_register_stub_init_handle(const uint8_t id, i2c_handle_t * handle)
{
    i2c_register_stub_t * stub = &i2c_register_stub[id];
    handle->_TWCR = &stub->twcr_reg;
    handle->_TWBR = &stub->twbr_reg;
    handle->_TWDR = &stub->twdr_reg;
    handle->_TWSR = &stub->twsr_reg;
    handle->_TWAR = &stub->twar_reg;
    handle->_TWAMR = &stub->twamr_reg;
}
