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

#ifndef I2C_DEVICE_INTERFACE_HEADER
#define I2C_DEVICE_INTERFACE_HEADER

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*
typedef enum
{
    I2C_ACKNOWLEDGMENT_ACK,
    I2C_ACKNOWLEDGMENT_NACK,
    I2C_ACKNOWLEDGMENT_UNKNOWN
} i2c_acknowledgment_t;
*/

/* All devices registered on I2C bus simulator should implement this interface */
typedef struct
{
    uint8_t data;               /**< Points to the internal device's data register                                                  */
    uint8_t address;            /**< Gives the current I2C address of targeted device                                               */
    bool start_sent;            /**< Tells if the device implementing this interface tries to send a Start condition over I2C bus.  */
    bool stop_sent;             /**< Tells if the device implementing this interface tries to send a Stop condition over I2C bus.   */
    bool ack_sent;              /**< An Acknoledged bit was sent (or not) over the bus.                                             */
    bool lost_arbitration;      /**< I2C bus determines if this device has lost arbitration or not.                                 */
    bool general_call_enabled;  /**< General call flag, tells this device responds to a general call                                */
    bool bus_busy;              /**< Used to inform the device the I2C bus is busy, which might help the device know what to do     */
    bool available;             /**< Tells if this device is available on I2C bus (basically if it will respond on I2C bus or not)  */
} i2c_device_interface_t;

typedef void (*i2c_interface_getter_function)(const uint8_t bus_id, i2c_device_interface_t ** const interface);
typedef void (*i2c_process_function)(const uint8_t id);

/* Common I2C device interface used by I2C Bus simulator to handle its registered devices */
typedef struct
{
    i2c_process_function process;       /**< function to be called on the targeted device at each I2C bus simulator process() call */
    i2c_device_interface_t * interface; /**< targeted device i2c bus interface                                                     */
} i2c_device_stub_t;



#ifdef __cplusplus
}
#endif

#endif /* I2C_DEVICE_INTERFACE_HEADER */