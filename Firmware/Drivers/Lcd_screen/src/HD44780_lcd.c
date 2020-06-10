#include <stddef.h>

#include "HD44780_lcd.h"
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
    HD44780_LCD_CMD_FUNCTION_SET       = 0x12,  /**< Allows to select the bus data length, line count and font used to display characters                           */
    HD44780_LCD_CMD_SET_CG_RAM_ADDR    = 0x14,  /**< Sets the current character generation circuit address (from 0 to 63 )                                          */
    HD44780_LCD_CMD_SET_DD_RAM_ADDR    = 0x18,  /**< Sets the current address pointer of DDRAM (from 0 to 127)                                                      */
} hd44780_lcd_command_t;

/**
 * @brief Packs configuration data internally, all data is squeezed so that the overall size of this structure
 * Shall be only 2 bytes (instead of the 5 bytes of the normal configuration structure used in the API , at the cost of explicity)
*/
typedef struct
{
    uint8_t i2c_address;    /**< I/O expander address */

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
    process_commands_t command;                 /**< Command enum which encodes what command is currently being processed                       */
    process_commands_parameters_t parameters;   /**< Stores all necessary parameters to perform requested commands                              */
    uint8_t sequence_number;                    /**< Gives the current sequence number to allow each command to know where it should resume     */
} process_commands_sequencer_t;

/* ##################################################################################################
   ################################### Internal data management #####################################
   ################################################################################################## */

// Internal state machine persistent memory
static hd44780_lcd_state_t          internal_state = HD44780_LCD_STATE_NOT_INITIALISED;
static internal_configuration_t     internal_configuration = {0};
static process_commands_sequencer_t commands_sequencer = {0};

/* ##################################################################################################
   ################################### Static functions declaration #################################
   ################################################################################################## */

static void internal_command_init(void);
static void internal_command_deinit(void);

static void internal_command_clear(void);
static void internal_command_home(void);

static void internal_command_set_display_on_off(void);
static void internal_command_set_cursor_visible(void);
static void internal_command_set_blinking_cursor(void);
static void internal_command_set_backlight(void);
static void internal_command_set_entry_mode(void);

static void internal_command_move_cursor_to_coord(void);
static void internal_command_move_relative(void);
static void internal_command_shift_display(void);

static void internal_command_print(void);


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
    config->entry_mode = HD44780_LCD_ENTRY_MODE_CURSOR_MOVE_RIGHT;
    config->font = HD44780_LCD_FONT_5x8;
    config->lines_mode = HD44780_LCD_LINES_2_LINES;
    config->i2c_address = PCF8574_I2C_ADDRESS_DEFAULT;

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
    internal_configuration.display.two_lines_mode = (config->lines_mode == HD44780_LCD_LINES_2_LINES);
    internal_configuration.display.small_font = (config->font == HD44780_LCD_FONT_5x8);
    internal_configuration.display.entry_mode = config->entry_mode;

    // Update commands sequencer to handle the initialisation command at next process() call
    internal_state = HD44780_LCD_STATE_INITIALISING;
    commands_sequencer.command = INTERNAL_CMD_INIT;
    commands_sequencer.sequence_number = 0;

    return HD44780_LCD_ERROR_OK;
}

hd44780_lcd_error_t hd44780_lcd_process(void)
{
    // Shall be initialised before process is called
    if (HD44780_LCD_STATE_NOT_INITIALISED == internal_state)
    {
        return HD44780_LCD_ERROR_DEVICE_WRONG_STATE;
    }

    switch(commands_sequencer.command)
    {
        case INTERNAL_CMD_INIT :
            internal_command_init();
            break;

        case INTERNAL_CMD_DEINIT :
            internal_command_deinit();
            break;

        case INTERNAL_CMD_HOME :
            internal_command_home();
            break;

        case INTERNAL_CMD_CLEAR :
            internal_command_clear();
            break;

        case INTERNAL_CMD_SET_BACKLIGHT :
            internal_command_set_backlight();
            break;

        case INTERNAL_CMD_SET_BLINKING_CURSOR :
            internal_command_set_blinking_cursor();
            break;

        case INTERNAL_CMD_SET_CURSOR_VISIBLE :
            internal_command_set_cursor_visible();
            break;

        case INTERNAL_CMD_SET_DISPLAY_ON_OFF :
            internal_command_set_display_on_off();
            break;

        case INTERNAL_CMD_SET_ENTRY_MODE :
            internal_command_set_entry_mode();
            break;

        case INTERNAL_CMD_MOVE_CURSOR_ABS :
            internal_command_move_cursor_to_coord();
            break;

        case INTERNAL_CMD_MOVE_CURSOR_REL :
            internal_command_move_relative();
            break;

        case INTERNAL_CMD_SHIFT_DISPLAY :
            internal_command_shift_display();
            break;

        case INTERNAL_CMD_PRINT :
            internal_command_print();
            break;

        case INTERNAL_CMD_IDLE :
        default:
            process_command_idling();
            break;
    }

    return HD44780_LCD_ERROR_OK;
}

/* ##################################################################################################
   ################################### Static functions definition ##################################
   ################################################################################################## */

static void internal_command_init(void)
{

}
