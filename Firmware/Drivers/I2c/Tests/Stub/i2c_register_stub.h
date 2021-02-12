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

#ifndef I2C_REGISTER_STUB_HEADER
#define I2C_REGISTER_STUB_HEADER

#include "config.h"
#include <stddef.h>
#include <stdint.h>
#include "i2c_reg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief mimics twi internal registers
*/
typedef struct
{
    volatile uint8_t twcr_reg;  /**< control register               */
    volatile uint8_t twbr_reg;  /**< bitrate generator register     */
    volatile uint8_t twdr_reg;  /**< data register                  */
    volatile uint8_t twsr_reg;  /**< status register                */
    volatile uint8_t twar_reg;  /**< slave address register         */
    volatile uint8_t twamr_reg; /**< slave address mask register    */
} i2c_register_stub_t;

/* Implemented in source file */
extern i2c_register_stub_t i2c_register_stub[I2C_DEVICES_COUNT];

/**
 * @brief resets the entire structure, just like a memset
 * @param[in]   id : index of targeted element
*/
void i2c_register_stub_erase(const uint8_t id);

/**
 * @brief initialises given handle to point to actual registers which are stubbed ones
 * @param[in]   id     :    index of targeted element
 * @param[out]  handle :    handle to be initialised
*/
void i2c_register_stub_init_handle(const uint8_t id, i2c_handle_t * handle);


#ifdef __cplusplus
}
#endif

#endif /* I2C_REGISTER_STUB_HEADER */