#include "i2c.h"
#include "pin_mapping.h"
#include "i2c_slave_handler.h"
// #include "driver_setup.h"
// #include "module_setup.h"

#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#define I2C_DRIVER_NUMBER        (0U)
#define I2C_SLAVE_ADDRESS        (0x56)

/**
 * @brief Simple enum that wraps driver errors
*/
typedef enum
{
    DRIVER_SETUP_ERROR_OK,          /**< Driver returned ok             */
    DRIVER_SETUP_ERROR_INIT_FAILED, /**< Driver's initialisation failed */
} driver_setup_error_t;


static void error_handler(void);
static void bootup_sequence(void);
/**
 * @brief initialises I2C driver
*/
static driver_setup_error_t driver_init_i2c(void);
static void init_i2c_slave_application_interface(void);

int main(void)
{
    i2c_error_t i2c_err = I2C_ERROR_OK;
    bootup_sequence();

    while(true)
    {
        i2c_err = i2c_process(0U);
        (void) i2c_err;
    }

    return 0;
}

/* ########################################################################################
   ########################## Static functions definitions ################################
   ######################################################################################## */
void error_handler(void)
{
    while(1)
    {
    }
}

static void bootup_sequence(void)
{
    driver_setup_error_t driver_init_error = driver_init_i2c();
    if (DRIVER_SETUP_ERROR_OK != driver_init_error)
    {
        error_handler();
    }
    sei();
    init_i2c_slave_application_interface();
}

static driver_setup_error_t driver_init_i2c(void)
{
    i2c_config_t config = {0};
    i2c_error_t err = i2c_get_default_config(&config);
    if (I2C_ERROR_OK != err)
    {
        return DRIVER_SETUP_ERROR_INIT_FAILED;
    }

    // SCL frequency = F_CPU / (16 + 2 * bitrate * prescaler)
    // 2 * bitrate * prescaler = (F_CPU / SCL frequency) - 16
    // targeted SCL frequency = 100'000 Hz
    // -> bitrate * prescaler = 72
    // choose highest where 72 / prescaler is a pure integer number (prescaler = 4)
    // then bitrate = (72/4) = 18 - 1 (-1 to account for the 0 based register value) = 17
    config.baudrate = 17U;
    config.interrupt_enabled = false;
    config.prescaler = I2C_PRESCALER_4;
    config.slave_address = I2C_SLAVE_ADDRESS;
    config.handle._TWAMR = &TWAMR;
    config.handle._TWAR = &TWAR;
    config.handle._TWBR = &TWBR;
    config.handle._TWCR = &TWCR;
    config.handle._TWDR = &TWDR;
    config.handle._TWSR = &TWSR;

    err = i2c_init(0U, &config);

    // Set the handler for incoming data
    if (I2C_ERROR_OK != err)
    {
        return DRIVER_SETUP_ERROR_INIT_FAILED;
    }
    else
    {
        err = i2c_slave_set_data_handler(0U, i2c_slave_data_handler);
    }

    // Set the transmission over callback
    if (I2C_ERROR_OK != err)
    {
        return DRIVER_SETUP_ERROR_INIT_FAILED;
    }
    else
    {
        err = i2c_slave_set_transmission_over_callback(0U, i2c_slave_transmission_over_callback);
    }

    if (I2C_ERROR_OK != err)
    {
        return DRIVER_SETUP_ERROR_INIT_FAILED;
    }

    return DRIVER_SETUP_ERROR_OK;
}

void init_i2c_slave_application_interface(void)
{
    i2c_slave_exposed_data.enabled = true;
    i2c_slave_exposed_data.locked = false;
    i2c_slave_exposed_data.mode = I2C_SLAVE_OPERATING_MODE_50;
    i2c_slave_exposed_data.power = 1200U;
    i2c_slave_exposed_data.speed = 20U;
    i2c_slave_exposed_data.temperature = 25;
}
