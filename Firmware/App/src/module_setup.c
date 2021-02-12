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

#include "module_setup.h"
#include "timebase.h"

module_setup_error_t module_init_timebase(void)
{
    timebase_config_t config = {0};
    config.cpu_freq = 16000000;
    config.timer.index = 0;
    config.timer.type = TIMEBASE_TIMER_8_BIT_ASYNC;
    config.timescale = TIMEBASE_TIMESCALE_MILLISECONDS;
    timebase_error_t err = timebase_init(0U, &config);
    if (TIMEBASE_ERROR_OK != err)
    {
        return MODULE_SETUP_ERROR_INIT_FAILED;
    }
    return MODULE_SETUP_ERROR_OK;
}


