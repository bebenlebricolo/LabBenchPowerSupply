#include <stddef.h>
#include <string.h>

#include "HD44780_lcd.h"
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

/* Set CGRAM Address command payload mapping */
#define HD44780_LCD_CGRAM_ADDRESS_START_BIT     (0)
#define HD44780_LCD_CGRAM_ADDRESS_MSK           (0x3F)

/* Set DDRAM Address command payload mapping */
#define HD44780_LCD_DDRAM_ADDRESS_START_BIT     (0)
#define HD44780_LCD_DDRAM_ADDRESS_MSK           (0x7F)

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
    HD44780_LCD_CMD_SET_CG_RAM_ADDR    = 0x40,  /**< Sets the current character generation circuit address (from 0 to 63 )                                          */
    HD44780_LCD_CMD_SET_DD_RAM_ADDR    = 0x80,  /**< Sets the current address pointer of DDRAM (from 0 to 127)                                                      */
} hd44780_lcd_command_t;

/**
 * @brief Packs configuration data internally, all data is squeezed so that the overall size of this structure
 * Shall be only 2 bytes (instead of the 5 bytes of the normal configuration structure used in the API , at the cost of explicity)
*/
typedef struct
{
    uint8_t i2c_address;    /**< I/O expander address       */

    struct {
        uint8_t timebase : 4;    /**< Used timebase module id    */
        uint8_t i2c : 4;         /**< I2C module index           */
    } indexes;

    // Bitfield storing persistent data about display state
    // All fields might be encoded within one CPU word (8 bits)
    struct
    {
        bool backlight : 1;                         /**< true : backlight on,       false : backlight off                                */
        bool enabled : 1;                           /**< true : display on,         false : display off                                  */
        bool cursor_visible : 1;                    /**< true : cursor visible,     false : cursor not displayed                         */
        bool cursor_blinking : 1;                   /**< true : blinking cursor,    false : persistent cursor                            */
        bool two_lines_mode : 1;                    /**< true : two lines mode,     false : one line mode (5x8 and 5x10 fonts supported) */
        bool small_font : 1;                        /**< true : 5x8 font used,      false : 5x10 dots font, single line only             */
        hd44780_lcd_entry_mode_t entry_mode : 2;    /**< Selects the kind of entry mode which is requested by the user upon typing                          */
    } display;
} internal_configuration_t;

/* ##################################################################################################
   ################################### Command sequencer description ################################
   ################################################################################################## */

/**
 * @brief Encodes available commands (shall respect the exposed API)
*/
typedef enum
{
    /* General purpose commands */
    INTERNAL_CMD_IDLE = 0,              /**< Idling command, nothing to do                      */
    INTERNAL_CMD_INIT,                  /**< Initialisation command                             */
    INTERNAL_CMD_DEINIT,                /**< Deinitialisation command                           */
    INTERNAL_CMD_CLEAR,                 /**< Clear display command                              */

    /* Unitary setters  */
    INTERNAL_CMD_SET_ENTRY_MODE,        /**< Set entry mode command                             */
    INTERNAL_CMD_SET_DISPLAY_ON_OFF,    /**< Set display on or off command                      */
    INTERNAL_CMD_SET_CURSOR_VISIBLE,    /**< Set cursor visibility command                      */
    INTERNAL_CMD_SET_BLINKING_CURSOR,   /**< Set blinking cursor command                        */
    INTERNAL_CMD_SET_BACKLIGHT,         /**< Set backlight command                              */

    /* Move commands */
    INTERNAL_CMD_HOME,                  /**< Go home command                                    */
    INTERNAL_CMD_MOVE_CURSOR_ABS,       /**< Move cursor to coordinates command                 */
    INTERNAL_CMD_MOVE_CURSOR_REL,       /**< Move cursor relatively command                     */
    INTERNAL_CMD_SHIFT_DISPLAY,         /**< Shift display command                              */

    /* Text related commands */
    INTERNAL_CMD_PRINT,                 /**< Print text command                                 */
} process_commands_t;

/**
 * @brief Packs all available parameters for API functions, in an union type to squeeze as much space as possible
*/
typedef union
{
    /* Single byte-wide data */
    bool enabled;                           /**< Generic boolean which depicts whether a functionality is enabled (or ON) or disabled (or OFF)  */
    hd44780_lcd_entry_mode_t entry_mode;    /**< Gives the entry mode                                                                           */
    hd44780_lcd_cursor_move_action_t move;  /**< Gives the cursor requested movement                                                            */
    hd44780_lcd_display_shift_t shift;      /**< Gives the display shifting direction                                                           */

    /* Single byte-wide structure */
    struct
    {
        uint8_t line : 2;                   /**< Gives the line number of the cursor position (from 0 to 3, could adapt this for 20x04 displays)*/
        uint8_t column : 6;                 /**< Gives the column number of the cursor position (from 0 to 63)                                  */
    } cursor_position;

    /* 3 bytes-wide structure */
    struct
    {
        uint8_t index;                      /**< Sets the index of current message character                                                    */
        uint8_t length;                     /**< Sets the overall length of the message to be printed                                           */
        char* buffer;
    } message;
} process_commands_parameters_t;

/**
 * @brief A command handler which is used to keep track of the current command state and where it should go at next process() call
*/
typedef struct
{
    void (*process_command)(void);              /**< Pointer to the private function to be called                                               */
    process_commands_parameters_t parameters;   /**< Stores all necessary parameters to perform requested commands                              */
    uint16_t start_time;
    uint16_t duration;
    struct
    {
        uint8_t count : 5;                      /**< Gives the current sequence number to allow each command to know where it should resume     */
        bool pulse_sent : 1;                    /**< Persistent flag which tells if a pulse (HD44780 'E' pin was sent or not                    */
        bool waiting : 1;                       /**< States whether the command sequence is waiting for LCD to settle or not                    */
        bool reentering : 1;                    /**< Tells if current sequence has already been entered once and is being reentered             */
    } sequence;
} process_commands_sequencer_t;


/* Only there to prevent pointing to NULL memory within process_commands_sequencer */
static inline void process_command_idling(void)
{
    return;
}

static void process_commands_sequencer_reset(process_commands_sequencer_t * sequencer)
{
    memset(sequencer, 0, sizeof(process_commands_sequencer_t));
    sequencer->process_command = process_command_idling;
}


/* ##################################################################################################
   ################################### Internal data management #####################################
   ################################################################################################## */

// Internal state machine persistent memory
static hd44780_lcd_state_t          internal_state = HD44780_LCD_STATE_NOT_INITIALISED;
static internal_configuration_t     internal_configuration = {0};
static process_commands_sequencer_t command_sequencer =
{
    .process_command = process_command_idling,
    .parameters = {0},
    .sequence =
    {
        .count = 0,
        .pulse_sent = false,
        .waiting = true,
        .reentering = false
    }
};
static uint8_t i2c_buffer = 0;

/* ##################################################################################################
   ################################### Static functions declaration #################################
   ################################################################################################## */

static void internal_command_init(void);
static void internal_command_deinit(void) {}

static void internal_command_clear(void) {}
static void internal_command_home(void) {}

static void internal_command_set_display_on_off(void) {}
static void internal_command_set_cursor_visible(void) {}
static void internal_command_set_blinking_cursor(void) {}
static void internal_command_set_backlight(void) {}
static void internal_command_set_entry_mode(void) {}

static void internal_command_move_cursor_to_coord(void) {}
static void internal_command_move_relative(void) {}
static void internal_command_shift_display(void) {}

static void internal_command_print(void) {}


/* ##################################################################################################
   ###################################### API functions #############################################
   ################################################################################################## */

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
    command_sequencer.process_command = internal_command_init;
    command_sequencer.sequence.count = 0;
    command_sequencer.sequence.pulse_sent = false;

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
   ################################### Static functions definition ##################################
   ################################################################################################## */

static void bootup_sequence_handler(uint8_t time_to_wait, bool end_with_wait)
{
    timebase_error_t tim_err = TIMEBASE_ERROR_OK;
    i2c_error_t i2c_err = I2C_ERROR_OK;
    i2c_state_t i2c_state = I2C_STATE_READY;

    if(false == command_sequencer.sequence.reentering)
    {
        tim_err = timebase_get_tick(internal_configuration.indexes.timebase, &command_sequencer.start_time);
        if( TIMEBASE_ERROR_OK != tim_err)
        {
            // Error handling placeholder
        }
        command_sequencer.sequence.reentering = true;
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
        if (I2C_STATE_READY == i2c_state)
        {
            bool time_has_passed = false;
            tim_err = timebase_get_duration_now(internal_configuration.indexes.timebase,
                                                command_sequencer.start_time,
                                                &command_sequencer.duration);
            if(true == command_sequencer.sequence.pulse_sent)
            {
                if(command_sequencer.duration >= HD44780_LCD_ENABLE_PULSE_DURATION_WAIT)
                {
                    time_has_passed = true;
                    i2c_buffer &= ~PCF8574_PULSE_START_MSK;
                    command_sequencer.sequence.waiting = false;
                    command_sequencer.sequence.pulse_sent = false;
                }
            }
            else
            {
                if(command_sequencer.duration >= time_to_wait)
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
            command_sequencer.sequence.count++;
            command_sequencer.sequence.reentering = false;
            command_sequencer.sequence.pulse_sent = false;
            command_sequencer.sequence.waiting = end_with_wait;
        }
    }

}

static inline void set_backlight_flag_in_i2c_buffer(void)
{
    i2c_buffer &= ~PCF8574_BACKLIGHT_MSK;
    i2c_buffer |= internal_configuration.display.backlight << PCF8574_BACKLIGHT_BIT;
}

static void handle_bootup_4_bits_mode(void)
{
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
                                            &command_sequencer.duration);
        if (TIMEBASE_ERROR_OK != tim_err)
        {
            // Error handling here
        }

        // Time to reset the "enable" pulse
        if (command_sequencer.duration >= HD44780_LCD_ENABLE_PULSE_DURATION_WAIT)
        {
            i2c_buffer &= ~PCF8574_PULSE_START_MSK;
            i2c_err = i2c_write(internal_configuration.indexes.i2c, internal_configuration.i2c_address,&i2c_buffer, 1U, 3U);
            if (I2C_ERROR_OK != i2c_err)
            {
                // Error handling here
            }
            command_sequencer.sequence.count++;
            command_sequencer.sequence.pulse_sent = false;
            command_sequencer.sequence.reentering = false;
            command_sequencer.sequence.waiting = false;
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
                // Clear data from buffer
                i2c_buffer &= 0x0F;
                i2c_buffer |= ( 1 << PCF8574_D5_BIT);   // Code for 4 bits instruction
                i2c_buffer |= PCF8574_PULSE_START_MSK;
                i2c_buffer &= ~ (PCF8574_READ_WRITE_MSK | PCF8574_REGISTER_SELECT_MSK);
                set_backlight_flag_in_i2c_buffer();

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

}

static void internal_command_init(void)
{
    switch(command_sequencer.sequence.count)
    {
        // First, wait for more than 40 ms to account for screen bootup time
        case 0 :
            if (false == command_sequencer.sequence.reentering)
            {
                i2c_buffer = (HD44780_LCD_CMD_FUNCTION_SET | HD44780_LCD_DATA_LENGTH_8_BITS);
                }
            set_backlight_flag_in_i2c_buffer();
            bootup_sequence_handler(HD44780_LCD_BOOTUP_TIME_MS, true);
            break;

        case 1 :
            bootup_sequence_handler(HD44780_LCD_FUNCTION_SET_FIRST_WAIT_MS, true);
            break;

        case 2:
            bootup_sequence_handler(HD44780_LCD_FUNCTION_SET_SECOND_WAIT_MS, false);
            break;

        case 3:
            handle_bootup_4_bits_mode();
            break;

        case 4:
            //configure_font_and_lines_count();
            break;


        default:
            break;


    }
}
