#include "i2c.h"
#include "pin_mapping.h"
// #include "driver_setup.h"
// #include "module_setup.h"

#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#define I2C_DRIVER_NUMBER        (0U)
#define I2C_TARGET_SLAVE_ADDRESS (0x23)
#define I2C_MASTER_WRITE_RETRIES (3U)

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
static void init_user_interface_button(void);

static void handle_master_slave_communication(void);
static bool check_button_clicked(void);
static bool detect_rising_edge_event(bool button_clicked);

// Static buffer implementation used for I2C communication
static struct
{
    uint8_t data[I2C_MAX_BUFFER_SIZE];
    uint8_t size;
} i2c_buffer;

int main(void)
{
    static bool pending_rising_edge_event = false;
    i2c_error_t i2c_err = I2C_ERROR_OK;
    bootup_sequence();

    while(true)
    {
        i2c_err = i2c_process(0U);
        (void) i2c_err;
        bool button_clicked = check_button_clicked();

        // Only use I2C device if a transition is detected (rising edge detected on the button clicked event)
        pending_rising_edge_event |= detect_rising_edge_event(button_clicked);

        // Consume last rising edge event and run the master to slave communication
        if (pending_rising_edge_event)
        {
            handle_master_slave_communication();
            pending_rising_edge_event = false;
        }

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
    init_user_interface_button();
    driver_setup_error_t driver_init_error = driver_init_i2c();
    if (DRIVER_SETUP_ERROR_OK != driver_init_error)
    {
        error_handler();
    }
    sei();
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
    config.slave_address = (0x32);
    config.handle._TWAMR = &TWAMR;
    config.handle._TWAR = &TWAR;
    config.handle._TWBR = &TWBR;
    config.handle._TWCR = &TWCR;
    config.handle._TWDR = &TWDR;
    config.handle._TWSR = &TWSR;

    err = i2c_init(0U, &config);
    if (I2C_ERROR_OK != err)
    {
        return DRIVER_SETUP_ERROR_INIT_FAILED;
    }
    return DRIVER_SETUP_ERROR_OK;
}

static void handle_master_slave_communication(void)
{
    static uint8_t step_number = 0;
    static const char* write_msg = "Testing i2c communication!";

    i2c_error_t i2c_err = I2C_ERROR_OK;
    i2c_state_t state = I2C_STATE_READY;

    i2c_err = i2c_get_state(I2C_DRIVER_NUMBER, &state);
    if (I2C_ERROR_OK != i2c_err)
    {
        error_handler();
    }

    if (I2C_STATE_READY == state)
    {
        switch(step_number)
        {
            case 0:
                i2c_buffer.size = strnlen(write_msg, I2C_MAX_BUFFER_SIZE);
                memcpy(i2c_buffer.data, write_msg, i2c_buffer.size);
                i2c_err = i2c_write(I2C_DRIVER_NUMBER, I2C_TARGET_SLAVE_ADDRESS, i2c_buffer.data, i2c_buffer.size, I2C_MASTER_WRITE_RETRIES);
                (void) i2c_err;
                step_number++;
                break;

            case 1:
                break;

            case 2:
                break;

            case 3:
                break;

            default:
                step_number = 0;
                break;
        }

    }


}

static bool check_button_clicked(void)
{
    bool button_clicked = false;
    uint8_t port_state = (I2C_RUN_STEP_BUTTON_PORT_REG & (1 << I2C_RUN_STEP_BUTTON_PIN_NUMBER)) >> I2C_RUN_STEP_BUTTON_PIN_NUMBER;

    // Invert port_state value : when port state is equal to 0, then it means the switch is triggered (pulls pin to ground)
    button_clicked = (bool) !port_state;
    return button_clicked;
}

static void init_user_interface_button(void)
{
    // Sets the port down before changing data direction register
    I2C_RUN_STEP_BUTTON_PORT_REG &= ~(1 << I2C_RUN_STEP_BUTTON_PIN_NUMBER);
    // Sets button pin as input (unset bit at button's port)
    I2C_RUN_STEP_BUTTON_DDR_REG &= ~(1 << I2C_RUN_STEP_BUTTON_PIN_NUMBER);

    // Now PORTx for this button equals 0
    // Now DDRx for this button pin equals 0
    // So we are in the tristate, high impedance input pin mode
}

static bool detect_rising_edge_event(bool button_clicked)
{
    static bool previous_state = false;
    bool rising_edge_detected = false;

    // If both states are different and button clicked is true then we know our previous state was false
    // So we are detecting an edge rising event
    if ((previous_state != button_clicked)
    && (true == button_clicked ))
    {
        rising_edge_detected = true;
    }

    // Update last state with current state in prevision for next call
    previous_state = button_clicked;
    return rising_edge_detected;
}
