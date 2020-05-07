#ifndef I2C_REGISTER_STUB_HEADER
#define I2C_REGISTER_STUB_HEADER

#include "config.h"
#include <stddef.h>
#include <stdint.h>

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