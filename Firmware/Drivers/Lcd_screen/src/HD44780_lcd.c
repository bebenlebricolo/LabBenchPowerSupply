#include <stddef.h>
#include <string.h>

#include "HD44780_lcd.h"
#include "HD44780_lcd_private.h"
#include "timebase.h"
#include "i2c.h"

/* ##################################################################################################
   #################################### PCF8574 I/O expander ########################################
   ################################################################################################## */

#define PCF8574_I2C_ADDRESS_BASE    (0x20)
#define PCF8574_I2C_ADDRESS_PINMASK (0x07)
#define PCF8574_I2C_ADDRESS_DEFAULT (0x27)

// PCF8574 port mapping :
// | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
// | D7 | D6 | D5 | D4 | BL | E  | RW | RS |

// Bitmasking
#define PCF8574_REGISTER_SELECT_MSK (0x01)
#define PCF8574_READ_WRITE_MSK      (0x02)
#define PCF8574_PULSE_START_MSK     (0x04)
#define PCF8574_BACKLIGHT_MSK       (0x08)
#define PCF8574_LCD_DATA_LINES_MSK  (0xF0)

// Bitmapping :
#define PCF8574_REGISTER_SELECT_BIT (0U)
#define PCF8574_READ_WRITE_BIT      (1U)
#define PCF8574_PULSE_START_BIT     (2U)
#define PCF8574_BACKLIGHT_BIT       (3U)
// 4 data bits are layed out, controlling pins D4 to D7 of HD44780 controller
#define PCF8574_D4_BIT              (4U)    // D4
#define PCF8574_D5_BIT              (5U)    // D5
#define PCF8574_D6_BIT              (6U)    // D6
#define PCF8574_D7_BIT              (7U)    // D7

/* ##################################################################################################
   ################################### HD44780 commands payload #####################################
   ################################################################################################## */

#define HD44780_LCD_DEFAULT_I2C_RETRIES_COUNT (3U)

#ifdef HD44780_LCD_USE_MICROSECONDS_TIMER
    #define HD44780_LCD_ENABLE_PULSE_DURATION_WAIT (37U)    /**< 37 microseconds, absolute minimum to wait for an instruction to complete */
#else
    #define HD44780_LCD_ENABLE_PULSE_DURATION_WAIT (1U)     /**< 1 millisecond wait, minimum resolution                                   */
#endif
#define HD44780_LCD_BOOTUP_TIME_MS              (40U)   /**< We have to wait more than 40 ms in the case (worst case) where LCD screen is powered with 2.7 Volts            */
#define HD44780_LCD_FUNCTION_SET_FIRST_WAIT_MS  (5U)    /**< Should be more than 4.1ms, 5ms is fine                                                                         */
#define HD44780_LCD_FUNCTION_SET_SECOND_WAIT_MS (1U)    /**< Normally, 100 µs are sufficient, but this is only for initialisation so 1 ms resolution will do it just fine   */

/* Function set command payload mapping */
#define HD44780_LCD_FUNTION_SET_FONT_BIT        (2)
#define HD44780_LCD_FUNTION_SET_LINES_NUMB_BIT  (3)
#define HD44780_LCD_FUNTION_SET_DATA_LEN_BIT    (4)

#define HD44780_LCD_FUNTION_SET_FONT_MSK        (0x04)
#define HD44780_LCD_FUNTION_SET_LINES_NUMB_MSK  (0x08)
#define HD44780_LCD_FUNTION_SET_DATA_LEN_MSK    (0x10)

/* Display control command payload mapping */
#define HD44780_LCD_DISPLAY_CTRL_BLINKING_BIT   (0)
#define HD44780_LCD_DISPLAY_CTRL_CURSOR_BIT     (1)
#define HD44780_LCD_DISPLAY_CTRL_DISPLAY_BIT    (2)

#define HD44780_LCD_DISPLAY_CTRL_BLINKING_MSK   (0x01)
#define HD44780_LCD_DISPLAY_CTRL_CURSOR_MSK     (0x02)
#define HD44780_LCD_DISPLAY_CTRL_DISPLAY_MSK    (0x04)

#define HD44780_LCD_CURSOR_OR_SHIFT_CURSOR_ONLY (0x00)
#define HD44780_LCD_CURSOR_OR_SHIFT_SHIFT_ONLY  (0x08)
#define HD44780_LCD_CURSOR_OR_SHIFT_LEFT        (0x00)
#define HD44780_LCD_CURSOR_OR_SHIFT_RIGHT       (0x04)

/* Set CGRAM Address command payload mapping */
#define HD44780_LCD_CGRAM_ADDRESS_START_BIT     (0)
#define HD44780_LCD_CGRAM_ADDRESS_MSK           (0x3F)

/* Set DDRAM Address command payload mapping */
#define HD44780_LCD_DDRAM_ADDRESS_START_BIT     (0)
#define HD44780_LCD_DDRAM_ADDRESS_MSK           (0x7F)

#define HD44780_LCD_DDRAM_START_ADDRESS         (0x00)
#define HD44780_LCD_MAX_CHARACTERS              (80U)

/**
 * @brief Describes the HD44780 valid instruction set, to be used in write mode
*/
typedef enum
{
    HD44780_LCD_CMD_CLEAR_DISPLAY      = 0x01,  /**< Clears the display */
    HD44780_LCD_CMD_RETURN_HOME        = 0x02,  /**< Returns the cursor to the 1rst line, 1rst column position( display is shifted accordingly)                     */
    HD44780_LCD_CMD_ENTRY_MODE_SET     = 0x04,  /**< Controls how LCD screen behaves (cursor and display shift) when a new character is written into DDRAM/CGRAM    */
    HD44780_LCD_CMD_DISPLAY_CONTROL    = 0x08,  /**< Controls general settings about the display (general display, cursor, blink )                                  */
    HD44780_LCD_CMD_CURSOR_SHIFT       = 0x10,  /**< Controls the cursor position and display shift. Used to move cursor or dislay unitarily                        */
    HD44780_LCD_CMD_FUNCTION_SET       = 0x20,  /**< Allows to select the bus data length, line count and font used to display characters                           */
    HD44780_LCD_CMD_INIT_4BITS_MODE    = 0x30,  /**< Special command which handles initialisation by instruction sequence
                                                     (actually it is a function set command + Data length set to 4 bits)                                            */
    HD44780_LCD_CMD_SET_CG_RAM_ADDR    = 0x40,  /**< Sets the current character generation circuit address (from 0 to 63 )                                          */
    HD44780_LCD_CMD_SET_DD_RAM_ADDR    = 0x80,  /**< Sets the current address pointer of DDRAM (from 0 to 127)                                                      */
} hd44780_lcd_command_t;

/* Only there to prevent pointing to NULL memory within process_commands_sequencer */
void process_command_idling(void)
{
    return;
}

/* ##################################################################################################
   ################################### Internal data management #####################################
   ################################################################################################## */

// i2c buffer represents the after being mapped to PCF8574 pins
static uint8_t i2c_buffer = 0;

// Data byte represents the actual data we want to send to the LCD screen
static uint8_t data_byte = 0;

// Internal state machine persistent memory
static hd44780_lcd_state_t          internal_state = HD44780_LCD_STATE_NOT_INITIALISED;
static internal_configuration_t     internal_configuration = {0};
static hd44780_lcd_error_t last_error = HD44780_LCD_ERROR_UNKNOWN;
static process_commands_sequencer_t command_sequencer =
{
    .process_command = process_command_idling,
    .parameters = {0},
    .sequence =
    {
        .count = 0,
        .pulse_sent = false,
        .waiting = false,
        .first_pass = true,
        .lower_bits = false
    },
    .nested_sequence_mode = false
};



/* ##################################################################################################
   ################################### Static functions declaration #################################
   ################################################################################################## */
#ifndef UNIT_TESTING
static
#endif
void reset_command_sequencer(bool reset_all)
{
    command_sequencer.start_time = 0;

    if (reset_all || !command_sequencer.nested_sequence_mode)
    {
        memset(&command_sequencer.parameters, 0, sizeof(process_commands_parameters_t));
        command_sequencer.process_command = process_command_idling;
        command_sequencer.sequence.count = 0;
        command_sequencer.nested_sequence_mode = false;
    }

    command_sequencer.sequence.first_pass = true;
    command_sequencer.sequence.lower_bits = false;
    command_sequencer.sequence.pulse_sent = false;
    command_sequencer.sequence.waiting = false;
}

/* ##################################################################################################
   ###################################### API functions #############################################
   ################################################################################################## */

#ifdef UNIT_TESTING
void get_process_command_sequencer(process_commands_sequencer_t ** const p_command_sequencer)
{
    *p_command_sequencer = &command_sequencer;
}
#endif

#ifdef UNIT_TESTING
void get_internal_configuration(internal_configuration_t ** const p_internal_configuration)
{
    if (NULL != p_internal_configuration)
    {
        *p_internal_configuration = &internal_configuration;
    }
}
#endif

hd44780_lcd_error_t hd44780_lcd_get_default_config(hd44780_lcd_config_t * const config)
{
    if (NULL == config)
    {
        return HD44780_LCD_ERROR_NULL_POINTER;
    }

    config->display_controls.cursor_blinking = true;
    config->display_controls.cursor_visible = true;
    config->display_controls.display_enabled = true;
    config->display_controls.with_backlight = true;
    config->print_controls.entry_mode = HD44780_LCD_ENTRY_MODE_CURSOR_MOVE_RIGHT;
    config->print_controls.font = HD44780_LCD_FONT_5x8;
    config->print_controls.lines_mode = HD44780_LCD_LINES_2_LINES;
    config->i2c_address = PCF8574_I2C_ADDRESS_DEFAULT;
    config->indexes.i2c = 0;
    config->indexes.timebase = 0;

    return HD44780_LCD_ERROR_OK;
}

hd44780_lcd_state_t hd44780_lcd_get_state(void)
{
    return internal_state;
}

#ifdef UNIT_TESTING
uint8_t get_data_byte(void)
{
    return data_byte;
}

uint8_t get_i2c_buffer(void)
{
    return i2c_buffer;
}

void set_data_byte(const uint8_t value)
{
    data_byte = value;
}

void set_i2c_buffer(const uint8_t value)
{
    i2c_buffer = value;
}
#endif

hd44780_lcd_error_t hd44780_lcd_driver_reset(void)
{
    i2c_buffer = 0;
    data_byte = 0;
    last_error = HD44780_LCD_ERROR_OK;
    reset_command_sequencer(false);
    memset(&internal_configuration, 0, sizeof(internal_configuration_t));
    internal_state = HD44780_LCD_STATE_NOT_INITIALISED;
    return HD44780_LCD_ERROR_OK;
}


hd44780_lcd_error_t hd44780_lcd_get_last_error(void)
{
    return last_error;
}

hd44780_lcd_error_t hd44780_lcd_init(hd44780_lcd_config_t const * const config)
{
    if (NULL == config)
    {
        return HD44780_LCD_ERROR_NULL_POINTER;
    }

    // Prevents double initialisation, maybe this is not really useful here (...?)
    if(HD44780_LCD_STATE_NOT_INITIALISED != internal_state)
    {
        return HD44780_LCD_ERROR_DEVICE_WRONG_STATE;
    }

    /* Copy user configuration to internal representation */
    internal_configuration.i2c_address = config->i2c_address;
    internal_configuration.display.backlight = config->display_controls.with_backlight;
    internal_configuration.display.cursor_visible = config->display_controls.cursor_visible;
    internal_configuration.display.cursor_blinking = config->display_controls.cursor_blinking;
    internal_configuration.display.enabled = config->display_controls.display_enabled;
    internal_configuration.display.two_lines_mode = (config->print_controls.lines_mode == HD44780_LCD_LINES_2_LINES);
    internal_configuration.display.small_font = (config->print_controls.font == HD44780_LCD_FONT_5x8);
    internal_configuration.display.entry_mode = config->print_controls.entry_mode;
    internal_configuration.indexes.i2c = config->indexes.i2c;
    internal_configuration.indexes.timebase = config->indexes.timebase;

    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_INITIALISING;

    reset_command_sequencer(true);
    command_sequencer.process_command = internal_command_init;
    command_sequencer.nested_sequence_mode = true;
    command_sequencer.sequence.waiting = true;

    return HD44780_LCD_ERROR_OK;
}

hd44780_lcd_error_t hd44780_lcd_clear(void)
{
    hd44780_lcd_error_t err = is_ready_to_accept_instruction();
    if (HD44780_LCD_ERROR_OK != err)
    {
        return err;
    }

    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_PROCESSING;
    reset_command_sequencer(true);
    command_sequencer.process_command = internal_command_clear;

    return HD44780_LCD_ERROR_OK;
}

hd44780_lcd_error_t hd44780_lcd_home(void)
{
    hd44780_lcd_error_t err = is_ready_to_accept_instruction();
    if (HD44780_LCD_ERROR_OK != err)
    {
        return err;
    }

    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_PROCESSING;
    reset_command_sequencer(true);
    command_sequencer.process_command = internal_command_home;

    return HD44780_LCD_ERROR_OK;
}

/* ############################ Display controls related functions #######################################*/

hd44780_lcd_error_t hd44780_lcd_set_display_on_off(const bool enabled)
{
    hd44780_lcd_error_t err = is_ready_to_accept_instruction();
    if (HD44780_LCD_ERROR_OK != err)
    {
        return err;
    }

    internal_configuration.display.enabled = enabled;

    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_PROCESSING;
    reset_command_sequencer(false);
    command_sequencer.process_command = internal_command_handle_display_controls;

    return HD44780_LCD_ERROR_OK;
}

hd44780_lcd_error_t hd44780_lcd_set_cursor_visible(const bool visible)
{
    hd44780_lcd_error_t err = is_ready_to_accept_instruction();
    if (HD44780_LCD_ERROR_OK != err)
    {
        return err;
    }

    internal_configuration.display.cursor_visible = visible;

    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_PROCESSING;
    reset_command_sequencer(true);
    command_sequencer.process_command = internal_command_handle_display_controls;

    return HD44780_LCD_ERROR_OK;
}

hd44780_lcd_error_t hd44780_lcd_set_blinking_cursor(const bool blinking)
{
    hd44780_lcd_error_t err = is_ready_to_accept_instruction();
    if (HD44780_LCD_ERROR_OK != err)
    {
        return err;
    }

    internal_configuration.display.cursor_blinking = blinking;

    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_PROCESSING;
    reset_command_sequencer(true);
    command_sequencer.process_command = internal_command_handle_display_controls;

    return HD44780_LCD_ERROR_OK;
}

hd44780_lcd_error_t hd44780_lcd_confgure_display(hd44780_lcd_font_t p_font, hd44780_lcd_lines_mode_t p_line_mode)
{
    hd44780_lcd_error_t err = is_ready_to_accept_instruction();
    if (HD44780_LCD_ERROR_OK != err)
    {
        return err;
    }

    // This device does not accept 5x10 font and 2 lines mode at the same time
    if((HD44780_LCD_FONT_5x10 == p_font)
    && (HD44780_LCD_LINES_2_LINES == p_line_mode))
    {
        return HD44780_LCD_ERROR_UNSUPPORTED_VALUE;
    }

    internal_configuration.display.small_font = HD44780_LCD_FONT_5x8 == p_font;
    internal_configuration.display.two_lines_mode = HD44780_LCD_LINES_2_LINES == p_line_mode;

    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_PROCESSING;
    reset_command_sequencer(true);
    command_sequencer.process_command = internal_command_handle_function_set;

    return HD44780_LCD_ERROR_OK;
}



/* ############################ end of Display controls related functions #######################################*/


hd44780_lcd_error_t hd44780_lcd_set_backlight(const bool enabled)
{
    hd44780_lcd_error_t err = is_ready_to_accept_instruction();
    if (HD44780_LCD_ERROR_OK != err)
    {
        return err;
    }

    internal_configuration.display.backlight = enabled;

    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_PROCESSING;
    reset_command_sequencer(true);
    command_sequencer.process_command = internal_command_set_backlight;

    return HD44780_LCD_ERROR_OK;
}

hd44780_lcd_error_t hd44780_lcd_set_entry_mode(const hd44780_lcd_entry_mode_t entry_mode)
{
    hd44780_lcd_error_t err = is_ready_to_accept_instruction();
    if (HD44780_LCD_ERROR_OK != err)
    {
        return err;
    }

    internal_configuration.display.entry_mode = entry_mode;

    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_PROCESSING;
    reset_command_sequencer(true);
    command_sequencer.process_command = internal_command_set_entry_mode;

    return HD44780_LCD_ERROR_OK;
}

hd44780_lcd_error_t hd44780_lcd_move_cursor_to_coord(const uint8_t line, const uint8_t column)
{
    hd44780_lcd_error_t err = is_ready_to_accept_instruction();
    if (HD44780_LCD_ERROR_OK != err)
    {
        return err;
    }

    // Reject wrong parameters before trying to send the command
    if (internal_configuration.display.two_lines_mode)
    {
        if ((line >= 2U)
        || (column >= (HD44780_LCD_MAX_CHARACTERS / 2U)))
        {
            return HD44780_LCD_ERROR_UNSUPPORTED_VALUE;
        }
    }
    else
    {
        if((line != 0)
        || (column >= HD44780_LCD_MAX_CHARACTERS))
        {
            return HD44780_LCD_ERROR_UNSUPPORTED_VALUE;
        }
    }

    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_PROCESSING;
    reset_command_sequencer(true);

    command_sequencer.parameters.cursor_position.line = line;
    command_sequencer.parameters.cursor_position.column = column;
    command_sequencer.process_command = internal_command_move_cursor_to_coord;

    return HD44780_LCD_ERROR_OK;
}

hd44780_lcd_error_t hd44780_lcd_move_relative(const hd44780_lcd_cursor_move_action_t move)
{
    hd44780_lcd_error_t err = is_ready_to_accept_instruction();
    if (HD44780_LCD_ERROR_OK != err)
    {
        return err;
    }

    command_sequencer.parameters.move = move;
    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_PROCESSING;
    reset_command_sequencer(true);
    command_sequencer.process_command = internal_command_move_cursor_to_coord;

    return HD44780_LCD_ERROR_OK;
}


hd44780_lcd_error_t hd44780_lcd_shift_display(const hd44780_lcd_display_shift_t shift)
{
    hd44780_lcd_error_t err = is_ready_to_accept_instruction();
    if (HD44780_LCD_ERROR_OK != err)
    {
        return err;
    }

    command_sequencer.parameters.shift = shift;
    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_PROCESSING;
    reset_command_sequencer(true);
    command_sequencer.process_command = internal_command_shift_display;

    return HD44780_LCD_ERROR_OK;
}

hd44780_lcd_error_t hd44780_lcd_print(const uint8_t length, char * const buffer)
{
    hd44780_lcd_error_t err = is_ready_to_accept_instruction();
    if (HD44780_LCD_ERROR_OK != err)
    {
        return err;
    }

    command_sequencer.parameters.message.length = length;
    command_sequencer.parameters.message.index = 0;
    command_sequencer.parameters.message.buffer = buffer;

    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_PROCESSING;
    reset_command_sequencer(true);
    command_sequencer.process_command = internal_command_print;

    return HD44780_LCD_ERROR_OK;
}


hd44780_lcd_error_t hd44780_lcd_process(void)
{
    // Shall be initialised before process is called
    if (HD44780_LCD_STATE_NOT_INITIALISED == internal_state)
    {
        return HD44780_LCD_ERROR_DEVICE_WRONG_STATE;
    }

    // Process stuff !
    command_sequencer.process_command();

    return HD44780_LCD_ERROR_OK;
}

/* ##################################################################################################
   ################################### Internal (private) functions definition ##################################
   ################################################################################################## */

void prepare_i2c_buffer(const transmission_mode_t mode)
{
    i2c_buffer &= 0x0F;
    set_backlight_flag_in_i2c_buffer();
    i2c_buffer &= ~(PCF8574_READ_WRITE_MSK);

    if (TRANSMISSION_MODE_INSTRUCTION == mode)
    {
        i2c_buffer &= ~(PCF8574_REGISTER_SELECT_MSK);
    }
    else
    {
        i2c_buffer |= PCF8574_REGISTER_SELECT_MSK;
    }
}


hd44780_lcd_error_t is_ready_to_accept_instruction(void)
{
    // Asserts the device is ready to accept instructions
    if( (HD44780_LCD_STATE_READY != internal_state)
    &&  (HD44780_LCD_STATE_PROCESSING != internal_state))
    {
        return HD44780_LCD_ERROR_DEVICE_NOT_INITIALISED;
    }

    // Checks if the device is already being serviced by another command
    if (HD44780_LCD_STATE_PROCESSING == internal_state)
    {
        return HD44780_LCD_ERROR_DEVICE_BUSY;
    }

    return HD44780_LCD_ERROR_OK;
}



void bootup_sequence_handler(uint8_t time_to_wait, bool end_with_wait)
{
    timebase_error_t tim_err = TIMEBASE_ERROR_OK;
    i2c_error_t i2c_err = I2C_ERROR_OK;
    i2c_state_t i2c_state = I2C_STATE_READY;

    if(true == command_sequencer.sequence.first_pass)
    {
        tim_err = timebase_get_tick(internal_configuration.indexes.timebase, &command_sequencer.start_time);
        if( TIMEBASE_ERROR_OK != tim_err)
        {
            // Error handling placeholder
        }
        prepare_i2c_buffer(TRANSMISSION_MODE_INSTRUCTION);
        data_byte = HD44780_LCD_CMD_INIT_4BITS_MODE;
        i2c_buffer |= (data_byte & 0xF0);
        command_sequencer.sequence.first_pass = false;
    }

    // Check if I2C transaction completed
    i2c_err = i2c_get_state(internal_configuration.indexes.i2c, &i2c_state);
    if (I2C_ERROR_OK != i2c_err)
    {
        // Error handling placeholder
    }


    // Check if we are waiting waiting for device to bootup ...
    if (true == command_sequencer.sequence.waiting)
    {
        uint16_t duration = 0;
        if (I2C_STATE_READY == i2c_state)
        {
            bool time_has_passed = false;
            tim_err = timebase_get_duration_now(internal_configuration.indexes.timebase,
                                                &command_sequencer.start_time,
                                                &duration);
            if(true == command_sequencer.sequence.pulse_sent)
            {
                if (duration >= HD44780_LCD_ENABLE_PULSE_DURATION_WAIT)
                {
                    time_has_passed = true;
                    i2c_buffer &= ~PCF8574_PULSE_START_MSK;
                    command_sequencer.sequence.waiting = false;
                    command_sequencer.sequence.pulse_sent = false;
                }
            }
            else
            {
                if (duration >= time_to_wait)
                {
                    time_has_passed = true;
                    i2c_buffer |= PCF8574_PULSE_START_MSK;
                    command_sequencer.sequence.waiting = false;
                    command_sequencer.sequence.pulse_sent = true;
                }
            }

            if(time_has_passed)
            {
                i2c_err = i2c_write(internal_configuration.indexes.i2c, internal_configuration.i2c_address, &i2c_buffer, 1U, 3U);
                if( I2C_ERROR_OK != i2c_err)
                {
                    // Error handling placeholder
                }
            }
        }
        else
        {
            // I2C device is not ready yet, nothing to do except from checking later
        }
    }
    // We are not waiting anymore
    else
    {
        if(true == command_sequencer.sequence.pulse_sent)
        {
            command_sequencer.sequence.waiting = true;
            // Will wait 1 ms at next call (or 37 microseconds if used timer is set to work on a microsecond basis)
        }
        else
        {
            // Transaction finished, we can make the transition to next sequence number !
            reset_command_sequencer(false);
            command_sequencer.sequence.count++;
            command_sequencer.sequence.waiting = end_with_wait;
        }
    }

}

void set_backlight_flag_in_i2c_buffer(void)
{
    i2c_buffer &= ~PCF8574_BACKLIGHT_MSK;
    i2c_buffer |= internal_configuration.display.backlight << PCF8574_BACKLIGHT_BIT;
}

bool write_buffer(void)
{
    bool write_completed = false;
    uint16_t duration = 0;
    i2c_state_t i2c_state = I2C_STATE_NOT_INITIALISED;
    i2c_error_t i2c_err = I2C_ERROR_OK;
    timebase_error_t tim_err = TIMEBASE_ERROR_OK;

    i2c_err = i2c_get_state(internal_configuration.indexes.i2c, &i2c_state);
    if (I2C_ERROR_OK != i2c_err)
    {
        // Error handling here
    }

    if (true == command_sequencer.sequence.waiting)
    {
        tim_err = timebase_get_duration_now(internal_configuration.indexes.timebase,
                                            &command_sequencer.start_time,
                                            &duration);
        if (TIMEBASE_ERROR_OK != tim_err)
        {
            // Error handling here
        }

        // Time to reset the "enable" pulse
        if (duration >= HD44780_LCD_ENABLE_PULSE_DURATION_WAIT)
        {
            i2c_buffer &= ~PCF8574_PULSE_START_MSK;
            i2c_err = i2c_write(internal_configuration.indexes.i2c, internal_configuration.i2c_address,&i2c_buffer, 1U, 3U);
            if (I2C_ERROR_OK != i2c_err)
            {
                // Error handling here
            }
            write_completed = true;
        }
        else
        {
            // Do nothing and return
        }
    }
    else
    {
        // Ready to accept new instruction
        if (I2C_STATE_READY == i2c_state)
        {
            if (true == command_sequencer.sequence.pulse_sent)
            {
                tim_err = timebase_get_tick(internal_configuration.indexes.timebase, &command_sequencer.start_time);
                if (TIMEBASE_ERROR_OK != tim_err)
                {
                    // Error handling here
                }
                command_sequencer.sequence.waiting = true;
            }
            else
            {
                // Raise "Enable" pin high first
                i2c_buffer |= PCF8574_PULSE_START_MSK;

                i2c_err = i2c_write(internal_configuration.indexes.i2c, internal_configuration.i2c_address,&i2c_buffer, 1U, 3U);
                if (I2C_ERROR_OK != i2c_err)
                {
                    // Error handling here
                }

                command_sequencer.sequence.pulse_sent = true;
                command_sequencer.sequence.waiting = false;
            }
        }
        else
        {
            //Do nothing until I2C device becomes available again
        }

    }
    return write_completed;
}

/* ##################################################################################################
   ######################## Data-related internal functions implementation ##########################
   ################################################################################################## */

void handle_function_set(void)
{
    data_byte = (uint8_t) HD44780_LCD_CMD_FUNCTION_SET;
    data_byte |= (true == internal_configuration.display.two_lines_mode) ? HD44780_LCD_LINES_2_LINES : HD44780_LCD_LINES_1_LINE;
    data_byte |= (true == internal_configuration.display.small_font) ? HD44780_LCD_FONT_5x8 : HD44780_LCD_FONT_5x10;
}

void handle_display_controls(void)
{
    data_byte = (uint8_t) HD44780_LCD_CMD_DISPLAY_CONTROL;
    data_byte |= internal_configuration.display.enabled ? HD44780_LCD_DISPLAY_CTRL_DISPLAY_MSK : 0x00 ;
    data_byte |= internal_configuration.display.cursor_visible ? HD44780_LCD_DISPLAY_CTRL_CURSOR_MSK : 0x00 ;
    data_byte |= internal_configuration.display.cursor_blinking ? HD44780_LCD_DISPLAY_CTRL_BLINKING_MSK : 0x00 ;
}

void handle_entry_mode(void)
{
    data_byte = (uint8_t) HD44780_LCD_CMD_ENTRY_MODE_SET;
    data_byte |= internal_configuration.display.entry_mode;
}

/* ##################################################################################################
   #################################### Internal (private) handlers ###########################################
   ################################################################################################## */

void init_4_bits_selection_handler(void)
{
    if (command_sequencer.sequence.first_pass)
    {
        i2c_buffer &= 0x0F;
        prepare_i2c_buffer(TRANSMISSION_MODE_INSTRUCTION);
        command_sequencer.sequence.first_pass = false;
        data_byte = HD44780_LCD_CMD_FUNCTION_SET;
        i2c_buffer |= (data_byte & 0xF0);
    }

    // Handle data write in 4 bits mode only (one exception where we do not need to send the full 8 bits)
    bool write_completed = write_buffer();
    if (write_completed)
    {
        reset_command_sequencer(false);
        command_sequencer.sequence.count++;
    }
}

bool handle_byte_sending(void)
{
    bool byte_sent = false;
    if( true == command_sequencer.sequence.lower_bits)
    {
        i2c_buffer = (i2c_buffer & 0x0F) | ((data_byte & 0x0F) << 4U);
    }
    else
    {
        i2c_buffer = (i2c_buffer & 0x0F) | (data_byte & 0xF0);
    }

    bool write_completed = write_buffer();
    if (write_completed)
    {
        if (true == command_sequencer.sequence.lower_bits)
        {
            command_sequencer.sequence.lower_bits = false;
            command_sequencer.sequence.first_pass = true;
            byte_sent = true;
        }
        else
        {
            // Higher bits will be sent at next call
            command_sequencer.sequence.lower_bits = true;
        }
        command_sequencer.sequence.pulse_sent = false;
        command_sequencer.sequence.waiting = false;
    }
    return byte_sent;
}

void internal_command_handle_function_set(void)
{
    // Set the right data into PCF8574 buffer
    if (command_sequencer.sequence.first_pass)
    {
        handle_function_set();
        prepare_i2c_buffer(TRANSMISSION_MODE_INSTRUCTION);
        command_sequencer.sequence.first_pass = false;
    }

    bool byte_sent = handle_byte_sending();

    // Did we send the full payload ?
    if (true == byte_sent)
    {
        if (command_sequencer.nested_sequence_mode == false)
        {
            internal_state = HD44780_LCD_STATE_READY;
        }
        else
        {
            command_sequencer.sequence.count++;
        }
        reset_command_sequencer(false);
    }

}

void internal_command_clear(void)
{
    // Set the right data into PCF8574 buffer
    if (command_sequencer.sequence.first_pass)
    {
        data_byte = HD44780_LCD_CMD_CLEAR_DISPLAY;
        prepare_i2c_buffer(TRANSMISSION_MODE_INSTRUCTION);
        command_sequencer.sequence.first_pass = false;
    }

    bool byte_sent = handle_byte_sending();

    // Did we send the full payload ?
    if (true == byte_sent)
    {
        if (command_sequencer.nested_sequence_mode == false)
        {
            internal_state = HD44780_LCD_STATE_READY;
        }
        else
        {
            command_sequencer.sequence.count++;
        }
        reset_command_sequencer(false);
    }
}

void internal_command_set_entry_mode(void)
{
    // Set the right data into PCF8574 buffer
    if (command_sequencer.sequence.first_pass)
    {
        handle_entry_mode();
        prepare_i2c_buffer(TRANSMISSION_MODE_INSTRUCTION);
        command_sequencer.sequence.first_pass = false;
    }

    bool byte_sent = handle_byte_sending();

    // Did we send the full payload ?
    if (true == byte_sent)
    {
        if (command_sequencer.nested_sequence_mode == false)
        {
            internal_state = HD44780_LCD_STATE_READY;
        }
        else
        {
            command_sequencer.sequence.count++;
        }
        reset_command_sequencer(false);
    }
}


void internal_command_init(void)
{
    switch(command_sequencer.sequence.count)
    {
        // First, wait for more than 40 ms to account for screen bootup time
        case 0 :
            bootup_sequence_handler(HD44780_LCD_BOOTUP_TIME_MS, true);
            break;

        // Second ping
        case 1 :
            bootup_sequence_handler(HD44780_LCD_FUNCTION_SET_FIRST_WAIT_MS, true);
            break;

        // Last ping to let the device wake up
        case 2:
            bootup_sequence_handler(HD44780_LCD_FUNCTION_SET_SECOND_WAIT_MS, false);
            break;

        // Set 4 bits mode interface
        case 3:
            init_4_bits_selection_handler();
            break;

        // Set print controls (data length, lines count, font i.e. "Function set" command of HD44780 LCD screen
        case 4:
            internal_command_handle_function_set();
            break;

        // Set display off
        case 5:
            internal_configuration.display.enabled = false;
            internal_command_handle_display_controls();
            break;

        // Clear display
        case 6:
            internal_command_clear();
            break;

        // Configure the entry mode
        case 7:
            internal_command_set_entry_mode();
            break;

        // Stop execution
        default:
            reset_command_sequencer(true);
            internal_state = HD44780_LCD_STATE_READY;
            break;
    }
}


void internal_command_home(void)
{
    // Set the right data into PCF8574 buffer
    if (command_sequencer.sequence.first_pass)
    {
        data_byte = HD44780_LCD_CMD_RETURN_HOME;
        prepare_i2c_buffer(TRANSMISSION_MODE_INSTRUCTION);
        command_sequencer.sequence.first_pass = false;
    }

    bool byte_sent = handle_byte_sending();

    // Did we send the full payload ?
    if (true == byte_sent)
    {
        if (command_sequencer.nested_sequence_mode == false)
        {
            internal_state = HD44780_LCD_STATE_READY;
        }
        else
        {
            command_sequencer.sequence.count++;
        }
        reset_command_sequencer(false);
    }
}

void internal_command_handle_display_controls(void)
{
    // Set the right data into PCF8574 buffer
    if (command_sequencer.sequence.first_pass)
    {
        handle_display_controls();
        prepare_i2c_buffer(TRANSMISSION_MODE_INSTRUCTION);
        command_sequencer.sequence.first_pass = false;
    }

    bool byte_sent = handle_byte_sending();

    // Did we send the full payload ?
    if (true == byte_sent)
    {
        if (command_sequencer.nested_sequence_mode == false)
        {
            internal_state = HD44780_LCD_STATE_READY;
        }
        else
        {
            command_sequencer.sequence.count++;
        }
        reset_command_sequencer(false);
    }
}

void internal_command_set_backlight(void)
{
    // This one is a little different because we do not need to send anything to
    // HD44780 LCD screen : backlight is directly handled by a pin of PCF8574 I/O expander

    // This is not a command for HD44780 screen, so put the enable pin to low
    i2c_buffer &= ~PCF8574_PULSE_START_MSK;

    set_backlight_flag_in_i2c_buffer();
    i2c_error_t i2c_err = I2C_ERROR_OK;
    i2c_state_t i2c_state = I2C_STATE_NOT_INITIALISED;

    i2c_err = i2c_get_state(internal_configuration.indexes.i2c, &i2c_state);
    if (I2C_ERROR_OK != i2c_err)
    {
        // Error handling here
        // Let the driver know something bad happen (usually, we are in a wrong state)
        return;
    }

    if (I2C_STATE_READY == i2c_state)
    {
        i2c_err = i2c_write(internal_configuration.indexes.i2c,
                            internal_configuration.i2c_address,
                            &i2c_buffer, 1U,
                            HD44780_LCD_DEFAULT_I2C_RETRIES_COUNT);
        if (I2C_ERROR_OK != i2c_err)
        {
            // Error handling here
            // Let the driver know something bad happened such as a not responding slave
        }

        // Our transaction is over !
        internal_state = HD44780_LCD_STATE_READY;
        reset_command_sequencer(false);
    }
}


void internal_command_move_cursor_to_coord(void)
{
    // We need to write the new DDRAM address to the internal address counter of
    // LCD screen in the aim to move the cursor position

    // Set the right data into PCF8574 buffer
    if (command_sequencer.sequence.first_pass)
    {
        data_byte = HD44780_LCD_CMD_SET_DD_RAM_ADDR;
        uint8_t ddram_value = 0;
        if (internal_configuration.display.two_lines_mode)
        {
            ddram_value = command_sequencer.parameters.cursor_position.line * (HD44780_LCD_MAX_CHARACTERS / 2);
        }
        ddram_value += command_sequencer.parameters.cursor_position.column;
        data_byte |= (data_byte & HD44780_LCD_DDRAM_ADDRESS_MSK) + ddram_value;

        prepare_i2c_buffer(TRANSMISSION_MODE_INSTRUCTION);
        command_sequencer.sequence.first_pass = false;
    }

    bool byte_sent = handle_byte_sending();

    // Did we send the full payload ?
    if (true == byte_sent)
    {
        if (command_sequencer.nested_sequence_mode == false)
        {
            internal_state = HD44780_LCD_STATE_READY;
        }
        else
        {
            command_sequencer.sequence.count++;
        }
        reset_command_sequencer(false);
    }
}

void internal_command_move_relative(void)
{
    if (command_sequencer.sequence.first_pass)
    {
        data_byte = HD44780_LCD_CMD_CURSOR_SHIFT;
        switch(command_sequencer.parameters.move)
        {
            case HD44780_LCD_CURSOR_MOVE_RIGHT:
                // Generic hardware's behavior when cursor reaches the ends of a line : cursor is 'teleported' to the other end and data is written over the old one
                // If we want to prevent the cursor to teleport (and prevent the cursor to go out of the screen), we will need to know exactly the current position
                // of the cursor in the aim to discard further entries if cursor is at the end of a line, or prevent cursor to go back if at the beginning of the line
                // Note : this will not be implemented in this driver, but this is the place to do it if you want!
                data_byte |= HD44780_LCD_CURSOR_OR_SHIFT_CURSOR_ONLY
                          |  HD44780_LCD_CURSOR_OR_SHIFT_RIGHT;
                break;

            case HD44780_LCD_CURSOR_MOVE_LEFT:
                data_byte |= HD44780_LCD_CURSOR_OR_SHIFT_CURSOR_ONLY
                          |  HD44780_LCD_CURSOR_OR_SHIFT_LEFT;
                break;

            case HD44780_LCD_CURSOR_MOVE_UP:
            case HD44780_LCD_CURSOR_MOVE_DOWN:
                // Not implemented right now :
                // Requires to know exactly the current position of the cursor
                // And to process the new position given the current screen configuration (one, two lines)
                // For instance : actual address is 0x13 (first line, column N°19). Move UP instruction :
                // New address should be (0x13 + MAX_CHARACTERS_PER_LINE(=40)) % MAX_CHARACTERS_TOTAL => new position = 0x3B
                // NOTE : for both a 2 lines display and 1 line display :
                //  1 line : cursor will remain at the same position : input can be discarded, state is set to "READY" and function returns
                //  2 lines : UP and DOWN exhibit the same behavior, providing the cursor does 'teleport' when reaching the boundaries of the screen.
                //  Otherwise, cursor shall be stuck to the screen boundaries
                internal_state = HD44780_LCD_STATE_READY;
                return;

            default:
                internal_state = HD44780_LCD_STATE_READY;
                return;
        }

        prepare_i2c_buffer(TRANSMISSION_MODE_INSTRUCTION);
        command_sequencer.sequence.first_pass = false;
    }

    bool byte_sent = handle_byte_sending();

    // Did we send the full payload ?
    if (true == byte_sent)
    {
        if (command_sequencer.nested_sequence_mode == false)
        {
            internal_state = HD44780_LCD_STATE_READY;
        }
        else
        {
            command_sequencer.sequence.count++;
        }
        reset_command_sequencer(false);
    }
}

void internal_command_shift_display(void)
{
    if (command_sequencer.sequence.first_pass)
    {
        data_byte = HD44780_LCD_CMD_CURSOR_SHIFT;
        switch(command_sequencer.parameters.shift)
        {
            case HD44780_LCD_DISPLAY_SHIFT_RIGHT:
                data_byte |= HD44780_LCD_CURSOR_OR_SHIFT_SHIFT_ONLY
                          |  HD44780_LCD_CURSOR_OR_SHIFT_RIGHT;
                break;

            case HD44780_LCD_DISPLAY_SHIFT_LEFT:
                data_byte |= HD44780_LCD_CURSOR_OR_SHIFT_CURSOR_ONLY
                          |  HD44780_LCD_CURSOR_OR_SHIFT_LEFT;
                break;

            default:
                internal_state = HD44780_LCD_STATE_READY;
                return;
        }

        prepare_i2c_buffer(TRANSMISSION_MODE_INSTRUCTION);
        command_sequencer.sequence.first_pass = false;
    }

    bool byte_sent = handle_byte_sending();

    // Did we send the full payload ?
    if (true == byte_sent)
    {
        if (command_sequencer.nested_sequence_mode == false)
        {
            internal_state = HD44780_LCD_STATE_READY;
        }
        else
        {
            command_sequencer.sequence.count++;
        }
        reset_command_sequencer(false);
    }
}


void internal_command_print(void)
{
    // If previous command was related to read/write into CGRAM or setting the CGRAM address,
    // We'll need to reset the DDRAM address first to switch the device in DDRAM mode for next data write
    // Note : above functionality is not implemented yet, assuming device is writing to DDRAM ...
    if (command_sequencer.sequence.first_pass)
    {
        data_byte = *((uint8_t *)(command_sequencer.parameters.message.buffer + command_sequencer.parameters.message.index));
        prepare_i2c_buffer(TRANSMISSION_MODE_DATA);
        bool byte_sent = handle_byte_sending();
        if (byte_sent)
        {
            command_sequencer.parameters.message.index++;
            if (command_sequencer.parameters.message.index >= command_sequencer.parameters.message.length)
            {
                internal_state = HD44780_LCD_STATE_READY;
            }
        }
    }
}
