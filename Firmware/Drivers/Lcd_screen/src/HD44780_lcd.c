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
        uint8_t count : 4;                      /**< Gives the current sequence number to allow each command to know where it should resume     */
        bool pulse_sent : 1;                    /**< Persistent flag which tells if a pulse (HD44780 'E' pin was sent or not                    */
        bool waiting : 1;                       /**< States whether the command sequence is waiting for LCD to settle or not                    */
        bool first_pass : 1;                    /**< Tells if current sequence has already been entered once and is being reentered             */
        bool lower_bits : 1;                    /**< When sending a byte of information, selects which 4 bits to send from 8 bits data          */
    } sequence;
    bool nested_sequence_mode;                  /**< Tells whether a command is nested within a high-level sequence or not (such as in initialisation sequence for instance) */
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
        .waiting = false,
        .first_pass = true,
        .lower_bits = false
    }
};

static void reset_command_sequencer(void)
{
    command_sequencer.start_time = 0;
    command_sequencer.duration = 0;
    memset(&command_sequencer.parameters, 0, sizeof(process_commands_parameters_t));
    command_sequencer.process_command = process_command_idling;
    command_sequencer.sequence.count = 0;
    command_sequencer.sequence.first_pass = true;
    command_sequencer.sequence.lower_bits = false;
    command_sequencer.sequence.pulse_sent = false;
    command_sequencer.sequence.waiting = false;
    command_sequencer.nested_sequence_mode = false;
}

// i2c buffer represents the after being mapped to PCF8574 pins
static uint8_t i2c_buffer = 0;

// Data byte represents the actual data we want to send to the LCD screen
static uint8_t data_byte = 0;

/* ##################################################################################################
   ################################### Static functions declaration #################################
   ################################################################################################## */

static void internal_command_init(void);

static void init_bootup_ping_handler();
static void init_4_bits_selection_handler();

static void internal_command_deinit(void) {}

static void internal_command_clear(void);
static void internal_command_home(void);

static void internal_command_handle_display_controls(void);
static void internal_command_handle_function_set(void);
static void internal_command_set_backlight(void);
static void internal_command_set_entry_mode(void);

static void internal_command_move_cursor_to_coord(void);
static void internal_command_move_relative(void);
static void internal_command_shift_display(void);

static void internal_command_print(void) {}

static inline void initialise_buffer_and_sequencer(void);
static inline void set_backlight_flag_in_i2c_buffer(void);
static inline hd44780_lcd_error_t is_ready_to_accept_instruction(void);




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

    reset_command_sequencer();
    command_sequencer.process_command = internal_command_init;
    command_sequencer.nested_sequence_mode = true;

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
    reset_command_sequencer();
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
    reset_command_sequencer();
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
    reset_command_sequencer();
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
    reset_command_sequencer();
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
    reset_command_sequencer();
    command_sequencer.process_command = internal_command_handle_display_controls;

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
    reset_command_sequencer();
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
    reset_command_sequencer();
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

    command_sequencer.parameters.cursor_position.line = line;
    command_sequencer.parameters.cursor_position.column = column;

    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_PROCESSING;
    reset_command_sequencer();
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
    reset_command_sequencer();
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
    reset_command_sequencer();
    command_sequencer.process_command = internal_command_shift_display;

    return HD44780_LCD_ERROR_OK;
}

hd44780_lcd_error_t hd44780_lcd_print(const uint8_t length, char const * const buffer)
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
    reset_command_sequencer();
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
   ################################### Static functions definition ##################################
   ################################################################################################## */

static inline void initialise_buffer_and_sequencer(void)
{
    i2c_buffer &= 0x0F;
    set_backlight_flag_in_i2c_buffer();
    i2c_buffer &= ~ (PCF8574_READ_WRITE_MSK | PCF8574_REGISTER_SELECT_MSK);

    // Send higher bits first (D7 to D4)
    command_sequencer.sequence.lower_bits = false;
    command_sequencer.sequence.first_pass = false;
}


static inline hd44780_lcd_error_t is_ready_to_accept_instruction(void)
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



static void bootup_sequence_handler(uint8_t time_to_wait, bool end_with_wait)
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
        if (I2C_STATE_READY == i2c_state)
        {
            bool time_has_passed = false;
            tim_err = timebase_get_duration_now(internal_configuration.indexes.timebase,
                                                &command_sequencer.start_time,
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
            command_sequencer.sequence.first_pass = true;
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

static bool write_buffer(void)
{
    bool write_completed = false;
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

static inline void handle_function_set(void)
{
    data_byte = (uint8_t) HD44780_LCD_CMD_FUNCTION_SET;
    data_byte |= (true == internal_configuration.display.two_lines_mode) ? HD44780_LCD_LINES_2_LINES : HD44780_LCD_LINES_1_LINE;
    data_byte |= (true == internal_configuration.display.small_font) ? HD44780_LCD_FONT_5x8 : HD44780_LCD_FONT_5x10;
}

static inline void handle_display_controls(void)
{
    data_byte = (uint8_t) HD44780_LCD_CMD_DISPLAY_CONTROL;
    data_byte |= internal_configuration.display.enabled ? HD44780_LCD_DISPLAY_CTRL_DISPLAY_MSK : 0x00 ;
    data_byte |= internal_configuration.display.cursor_visible ? HD44780_LCD_DISPLAY_CTRL_CURSOR_MSK : 0x00 ;
    data_byte |= internal_configuration.display.cursor_blinking ? HD44780_LCD_DISPLAY_CTRL_BLINKING_MSK : 0x00 ;
}

static inline void handle_entry_mode(void)
{
    data_byte = (uint8_t) HD44780_LCD_CMD_ENTRY_MODE_SET;
    data_byte |= internal_configuration.display.entry_mode;
}

/* ##################################################################################################
   #################################### Internal handlers ###########################################
   ################################################################################################## */

static void init_4_bits_selection_handler(void)
{
    if (command_sequencer.sequence.first_pass)
    {
        i2c_buffer &= 0x0F;
        set_backlight_flag_in_i2c_buffer();
        i2c_buffer &= ~ (PCF8574_READ_WRITE_MSK | PCF8574_REGISTER_SELECT_MSK);
        i2c_buffer |= ( 1 << PCF8574_D5_BIT);   // Code for 4 bits instruction
        command_sequencer.sequence.first_pass = false;
    }

    // Handle data write in 4 bits mode only (one exception where we do not need to send the full 8 bits)
    bool write_completed = write_buffer();
    if (write_completed)
    {
        command_sequencer.sequence.count++;
        command_sequencer.sequence.pulse_sent = false;
        command_sequencer.sequence.first_pass = true;
        command_sequencer.sequence.waiting = false;
        command_sequencer.sequence.lower_bits = false;
    }
}

static inline void handle_byte_sending(void)
{
    if( true == command_sequencer.sequence.lower_bits)
    {
        i2c_buffer |= (data_byte & 0x0F) << 4U;
    }
    else
    {
        i2c_buffer |= (data_byte & 0xF0);
    }

    bool write_completed = write_buffer();
    if (write_completed)
    {
        if (true == command_sequencer.sequence.lower_bits)
        {
            command_sequencer.sequence.count++;
            command_sequencer.sequence.lower_bits = false;
            command_sequencer.sequence.first_pass = true;
        }
        else
        {
            // Higher bits will be sent at next call
            command_sequencer.sequence.lower_bits = true;
        }
        command_sequencer.sequence.pulse_sent = false;
        command_sequencer.sequence.waiting = false;
    }
}

static void internal_command_handle_function_set(void)
{
    // Set the right data into PCF8574 buffer
    if (command_sequencer.sequence.first_pass)
    {
        handle_function_set();
        initialise_buffer_and_sequencer();
    }

    handle_byte_sending();
}

static void internal_command_clear(void)
{
    // Set the right data into PCF8574 buffer
    if (command_sequencer.sequence.first_pass)
    {
        data_byte = HD44780_LCD_CMD_CLEAR_DISPLAY;
        initialise_buffer_and_sequencer();
    }

    handle_byte_sending();
}

static void internal_command_set_entry_mode(void)
{
    // Set the right data into PCF8574 buffer
    if (command_sequencer.sequence.first_pass)
    {
        handle_entry_mode();
        initialise_buffer_and_sequencer();
    }

    handle_byte_sending();
}


static void internal_command_init(void)
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
            reset_command_sequencer();
            internal_state = HD44780_LCD_STATE_READY;
            break;
    }
}


static void internal_command_home(void)
{
    // Set the right data into PCF8574 buffer
    if (command_sequencer.sequence.first_pass)
    {
        data_byte = HD44780_LCD_CMD_RETURN_HOME;
        initialise_buffer_and_sequencer();
    }

    handle_byte_sending();
}

static void internal_command_handle_display_controls(void)
{
    // Set the right data into PCF8574 buffer
    if (command_sequencer.sequence.first_pass)
    {
        handle_display_controls();
        initialise_buffer_and_sequencer();
    }

    handle_byte_sending();
}

static void internal_command_set_backlight(void)
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
    }
}


static void internal_command_move_cursor_to_coord(void)
{
    // We need to write the new DDRAM address to the internal address counter of
    // LCD screen in the aim to move the cursor position

    // Set the right data into PCF8574 buffer
    if (command_sequencer.sequence.first_pass)
    {
        data_byte = HD44780_LCD_CMD_SET_DD_RAM_ADDR;
        if (internal_configuration.display.two_lines_mode)
        {
            data_byte |= command_sequencer.parameters.cursor_position.line * (HD44780_LCD_MAX_CHARACTERS / 2);
        }
        data_byte |= (data_byte & HD44780_LCD_DDRAM_ADDRESS_MSK) + command_sequencer.parameters.cursor_position.column;

        initialise_buffer_and_sequencer();
    }

    handle_byte_sending();
}

static void internal_command_move_relative(void)
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

        initialise_buffer_and_sequencer();
    }

    handle_byte_sending();
}

static void internal_command_shift_display(void)
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

        initialise_buffer_and_sequencer();
    }

    handle_byte_sending();
}
