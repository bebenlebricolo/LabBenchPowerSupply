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