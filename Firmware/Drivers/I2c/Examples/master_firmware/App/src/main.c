#include "i2c.h"
#include "pin_mapping.h"
#include <stdbool.h>
// #include "driver_setup.h"
// #include "module_setup.h"

#include <stdlib.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include "targeted_slave_i2c_interface.h"

#define I2C_DRIVER_NUMBER        (0U)
#define I2C_TARGET_SLAVE_ADDRESS (0x28)
#define I2C_MASTER_WRITE_RETRIES (2U)

/**
 * @brief Simple enum that wraps driver errors
*/
typedef enum
{
    DRIVER_SETUP_ERROR_OK,          /**< Driver returned ok             */
    DRIVER_SETUP_ERROR_INIT_FAILED, /**< Driver's initialisation failed */
} driver_setup_error_t;


static void error_handler(void);
static void bootup_sequence(bool force);
/**
 * @brief initialises I2C driver
*/
static driver_setup_error_t driver_init_i2c(void);
static void init_user_interface_button(void);

static void handle_master_slave_communication(void);
static bool check_sequence_next_button_clicked(void);
static bool check_sequence_reset_button_clicked(void);
static bool detect_sequence_next_button_rising_edge_event(bool clicked);

// Static buffer implementation used for I2C communication
static struct
{
    uint8_t data[I2C_MAX_BUFFER_SIZE];
    uint8_t size;
} i2c_buffer;

typedef i2c_error_t (*sequence_callback_t)(void);

static i2c_error_t i2c_slave_read_temps(void);
static i2c_error_t i2c_slave_read_speed(void);
static i2c_error_t i2c_slave_read_power(void);
static i2c_error_t i2c_slave_read_mode (void);
static i2c_error_t i2c_slave_set_on_off(void);
static i2c_error_t i2c_slave_set_mode(void);

#define MAX_I2C_CALLBACKS 9U
static struct
{
    uint8_t sequence_number;
    sequence_callback_t callbacks[MAX_I2C_CALLBACKS];
} i2c_sequence =
{
    .sequence_number = 0,
    .callbacks =
    {
        [0] = i2c_slave_read_temps,
        [1] = i2c_slave_read_speed,
        [2] = i2c_slave_read_power,
        [3] = i2c_slave_read_mode,
        [4] = i2c_slave_set_mode,
        [5] = i2c_slave_read_mode,
        [6] = i2c_slave_set_mode,
        [7] = i2c_slave_set_on_off,
        [8] = i2c_slave_read_power
    }
};




int main(void)
{
    static bool pending_rising_edge_event = false;
    i2c_error_t i2c_err = I2C_ERROR_OK;
    bootup_sequence(false);

    while(true)
    {
        i2c_err = i2c_process(0U);
        (void) i2c_err;
        bool button_clicked = check_sequence_next_button_clicked();
        if (check_sequence_reset_button_clicked() == true)
        {
            i2c_sequence.sequence_number = 0;
        }
        // Only use I2C device if a transition is detected (rising edge detected on the button clicked event)
        pending_rising_edge_event |= detect_sequence_next_button_rising_edge_event(button_clicked);

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
    // Reinit things ?
    bootup_sequence(true);
}

static void bootup_sequence(bool force)
{
    init_user_interface_button();
    driver_setup_error_t driver_init_error = driver_init_i2c();
    if (!force && DRIVER_SETUP_ERROR_OK != driver_init_error)
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
    config.slave.address = (0x32);
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

static i2c_error_t i2c_slave_read_temps()
{
    i2c_error_t ret = I2C_ERROR_OK;
    i2c_buffer.data[0] = I2C_SLAVE_COMMAND_MEAS_TEMP;
    // opcode + read (1 byte)
    i2c_buffer.size = 2U;
    ret = i2c_read(I2C_DRIVER_NUMBER, I2C_TARGET_SLAVE_ADDRESS, i2c_buffer.data, i2c_buffer.size, true, I2C_MASTER_WRITE_RETRIES);
    return ret;
}

static i2c_error_t i2c_slave_read_speed(void)
{
    i2c_error_t ret = I2C_ERROR_OK;
    i2c_buffer.data[0] = I2C_SLAVE_COMMAND_MEAS_SPEED;
    i2c_buffer.size = 2U;
    ret = i2c_read(I2C_DRIVER_NUMBER, I2C_TARGET_SLAVE_ADDRESS, i2c_buffer.data, i2c_buffer.size, true, I2C_MASTER_WRITE_RETRIES);
    return ret;
}

static i2c_error_t i2c_slave_read_power(void)
{
    i2c_error_t ret = I2C_ERROR_OK;
    i2c_buffer.data[0] = I2C_SLAVE_COMMAND_MEAS_POWER;
    i2c_buffer.size = 3U;
    ret = i2c_read(I2C_DRIVER_NUMBER, I2C_TARGET_SLAVE_ADDRESS, i2c_buffer.data, i2c_buffer.size, true, I2C_MASTER_WRITE_RETRIES);
    return ret;
}

static i2c_error_t i2c_slave_read_mode(void)
{
    i2c_error_t ret = I2C_ERROR_OK;
    i2c_buffer.data[0] = I2C_SLAVE_COMMAND_MODE;
    i2c_buffer.size = 2U;
    ret = i2c_read(I2C_DRIVER_NUMBER, I2C_TARGET_SLAVE_ADDRESS, i2c_buffer.data, i2c_buffer.size, true, I2C_MASTER_WRITE_RETRIES);
    return ret;
}

static i2c_error_t i2c_slave_set_on_off(void)
{
    static bool previous_state_on = true;
    i2c_error_t ret = I2C_ERROR_OK;

    i2c_buffer.data[0] = I2C_SLAVE_COMMAND_SWITCH_ON_OFF;
    if (previous_state_on)
    {
        i2c_buffer.data[1] = (uint8_t) false;
        previous_state_on = false;
    }
    else
    {
        i2c_buffer.data[1] = (uint8_t) true;
        previous_state_on = true;
    }
    i2c_buffer.size = 2U;
    ret = i2c_write(I2C_DRIVER_NUMBER, I2C_TARGET_SLAVE_ADDRESS, i2c_buffer.data, i2c_buffer.size, I2C_MASTER_WRITE_RETRIES);
    return ret;
}

static i2c_error_t i2c_slave_set_mode(void)
{
    static uint8_t mode_index = 0;
    static i2c_slave_operating_modes_t modes_array[4U] =
    {
        I2C_SLAVE_OPERATING_MODE_0,
        I2C_SLAVE_OPERATING_MODE_25,
        I2C_SLAVE_OPERATING_MODE_50,
        I2C_SLAVE_OPERATING_MODE_100
    };

    i2c_error_t ret = I2C_ERROR_OK;
    i2c_buffer.data[0] = I2C_SLAVE_COMMAND_MODE;
    i2c_buffer.data[1] = modes_array[mode_index];
    mode_index = (mode_index + 1) % 4U;
    i2c_buffer.size = 2U;

    ret = i2c_write(I2C_DRIVER_NUMBER, I2C_TARGET_SLAVE_ADDRESS, i2c_buffer.data, i2c_buffer.size, I2C_MASTER_WRITE_RETRIES);
    return ret;
}

static void handle_master_slave_communication(void)
{
    i2c_error_t i2c_err = I2C_ERROR_OK;
    i2c_state_t state = I2C_STATE_READY;

    i2c_err = i2c_get_state(I2C_DRIVER_NUMBER, &state);
    if (I2C_ERROR_OK != i2c_err)
    {
        error_handler();
    }

    if (I2C_STATE_READY == state)
    {
        i2c_err = i2c_sequence.callbacks[i2c_sequence.sequence_number]();
        if (I2C_ERROR_OK == i2c_err)
        {
            i2c_sequence.sequence_number = (i2c_sequence.sequence_number + 1) % MAX_I2C_CALLBACKS;
        }
    }

    if (I2C_ERROR_OK != i2c_err)
    {
        error_handler();
    }
}

static bool check_sequence_next_button_clicked(void)
{
    bool button_clicked = false;
    uint8_t port_state = (I2C_RUN_STEP_BUTTON_PIN_REG & (1 << I2C_RUN_STEP_BUTTON_PIN_NUMBER)) >> I2C_RUN_STEP_BUTTON_PIN_NUMBER;

    // Invert port_state value : when port state is equal to 0, then it means the switch is triggered (pulls pin to ground)
    button_clicked = (bool) !port_state;
    return button_clicked;
}


static bool check_sequence_reset_button_clicked(void)
{
    bool button_clicked = false;
    uint8_t port_state = (I2C_SEQ_RESET_BUTTON_PIN_REG & (1 << I2C_SEQ_RESET_BUTTON_PIN_NUMBER)) >> I2C_SEQ_RESET_BUTTON_PIN_NUMBER;

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

    // Sets the port down before changing data direction register
    I2C_SEQ_RESET_BUTTON_PORT_REG &= ~(1 << I2C_SEQ_RESET_BUTTON_PIN_NUMBER);
    // Sets button pin as input (unset bit at button's port)
    I2C_SEQ_RESET_BUTTON_DDR_REG &= ~(1 << I2C_SEQ_RESET_BUTTON_PIN_NUMBER);

    // Now PORTx for this button equals 0
    // Now DDRx for this button pin equals 0
    // So we are in the tristate, high impedance input pin mode
}

static bool detect_sequence_next_button_rising_edge_event(bool button_clicked)
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
