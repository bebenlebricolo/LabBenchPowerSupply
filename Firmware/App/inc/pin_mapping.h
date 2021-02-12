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

#ifndef PIN_MAPPING_HEADER
#define PIN_MAPPING_HEADER

#include <avr/io.h>

/* Timer 0 output compare pins */
#define OC0A_PIN_NUMBER PORTD6
#define OC0A_PORT_REG   PORTD
#define OC0A_DDR_REG    DDRD
#define OCOA_PIN_REG    PIND

#define OC0B_PIN_NUMBER PORTD5
#define OC0B_PORT_REG   PORTD
#define OC0B_DDR_REG    DDRD
#define OCOB_PIN_REG    PIND

/* Timer 1 output compare pins */
#define OC1A_PIN_NUMBER PORTB1
#define OC1A_PORT_REG   PORTB
#define OC1A_DDR_REG    DDRB
#define OC1A_PIN_REG    PINB

#define OC1B_PIN_NUMBER PORTB2
#define OC1B_PORT_REG   PORTB
#define OC1B_DDR_REG    DDRB
#define OC1B_PIN_REG    PINB

/* Timer 2 output compare pins */
#define OC2A_PIN_NUMBER PORTB3
#define OC2A_PORT_REG   PORTB
#define OC2A_DDR_REG    DDRB
#define OC2A_PIN_REG    PINB

#define OC2B_PIN_NUMBER PORTD3
#define OC2B_PORT_REG   PORTD
#define OC2B_DDR_REG    DDRD
#define OC2B_PIN_REG    PIND





#endif /* PIN_MAPPING_HEADER */