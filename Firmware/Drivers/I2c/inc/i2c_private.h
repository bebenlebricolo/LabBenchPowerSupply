#ifndef I2C_PRIVATE_HEADER
#define I2C_PRIVATE_HEADER

#include "i2c.h"

#ifdef UNIT_TESTING

#include "memutils.h"

/**
 * @brief clears the driver's internal memory and resets everything to 0 (memset used)
 * as depicted with the above conditionnal compilation #define, this is only needed while
 * performing unit testing.
*/
void i2c_driver_reset_memory(void);

/**
 * @brief retrieves internal buffer
 * @param   id : I2C driver's id
*/
uint8_t * i2c_get_internal_data_buffer(const uint8_t id);


/**
 * @brief this is used in a unit testing context and is meaningful when checking if
 * the command handler has effectively been registered correctly
*/
i2c_command_handler_t i2c_slave_get_command_handler(const uint8_t id);

/**
 * @brief this is only needed while performing unit tests, and allows
 * to manually set the internal state machine current state.
 * This is essentially meaningful when checking whether get/set api is accessing the
 * right data
*/
void i2c_set_state(const uint8_t id, const i2c_state_t state);

#endif

#endif /* I2C_PRIVATE_HEADER */