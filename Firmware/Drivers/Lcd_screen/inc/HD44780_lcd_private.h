/*

------------------
@<FreeMyCode>
FreeMyCode version : 1.0 RC alpha
    Author : bebenlebricolo
    License : 
        name : GPLv3
        url : https://www.gnu.org/licenses/quick-guide-gplv3.html
    Date : 12/02/2021
    Project : LabBenchPowerSupply
    Description : The Lab Bench Power Supply provides a simple design based around an Arduino Nano board to convert AC main voltage into
 smaller ones, ranging from 0V to 16V, with voltage and current regulations
<FreeMyCode>@
------------------

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef HD44780_LCD_PRIVATE_HEADER
#define HD44780_LCD_PRIVATE_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include "i2c.h"
#include "HD44780_lcd.h"

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

#define HD44780_LCD_2_LINES_MODE_START_ADDRESS  (0x40)

/* ##################################################################################################
   ################################### Internal types description ###################################
   ################################################################################################## */

/**
 * @brief used to select data or instruction transmission mode
*/
typedef enum
{
    TRANSMISSION_MODE_INSTRUCTION = 0,
    TRANSMISSION_MODE_DATA = 1U
} transmission_mode_t;

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
        const char * buffer;
    } message;
} process_commands_parameters_t;

/**
 * @brief A command handler which is used to keep track of the current command state and where it should go at next process() call
*/
typedef struct
{
    void (*process_command)(void);              /**< Pointer to the private function to be called                                               */
    process_commands_parameters_t parameters;   /**< Stores all necessary parameters to perform requested commands                              */
    uint16_t start_time;                        /**< Used to record starting time of a wait operation, for instance                             */

    struct
    {
        uint8_t count : 4;                      /**< Gives the current sequence number to allow each command to know where it should resume     */
        bool pulse_sent : 1;                    /**< Persistent flag which tells if a pulse (HD44780 'E' pin was sent or not                    */
        bool waiting : 1;                       /**< States whether the command sequence is waiting for LCD to settle or not                    */
        bool first_pass : 1;                    /**< Tells if current sequence has already been entered once and is being reentered             */
        bool lower_bits : 1;                    /**< When sending a byte of information, selects which 4 bits to send from 8 bits data          */
    } sequence;

    bool nested_sequence_mode : 1;              /**< Tells whether a command is nested within a high-level sequence or not (such as in initialisation sequence for instance) */
} process_commands_sequencer_t;


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


/* ##################################################################################################
   ################################### Command sequencer description ################################
   ################################################################################################## */

/**
 * @brief internal handler which is used while initialising the device by instruction
 * It will set the interface mode to 4 bits
*/
void init_4_bits_selection_handler(void);

/**
 * @brief internal handler which aims to initialize hd44780 lcd screen device by instruction
 * Initialisation sequence was borrowed from Hitachi HD44780 LCD screen datasheet
*/
void internal_command_init(void);

/**
 * @brief Will reset device back to its original state (clears the screen, sets backlight off, etc.)
*/
void internal_command_deinit(void);

/**
 * @brief Internal handler which clears the LCD screen
*/
void internal_command_clear(void);

/**
 * @brief Sets the cursor to its original position
*/
void internal_command_home(void);

/**
 * @brief Handles display controls such as display enabled/disabled, cursor visibility and cursor blink
 */
void internal_command_handle_display_controls(void);

/**
 * @brief Handles displaying mode such as single line / 2 lines mode and font selection
*/
void internal_command_handle_function_set(void);

/**
 * @brief Handles LCD screen backlight using the PCF8574 GPIO directly
*/
void internal_command_set_backlight(void);

/**
 * @brief Handles how character are input in LCD screen and how display reacts to it (cursor moves to right, left, display shifts right, left)
*/
void internal_command_set_entry_mode(void);

/**
 * @brief Moves cursor to an absolute position on the screen
*/
void internal_command_move_cursor_to_coord(void);

/**
 * @brief moves the cursor relatively to its current position (right or left, up and down are not implemented yet)
*/
void internal_command_move_relative(void);

/**
 * @brief shifts the entire display right or left
*/
void internal_command_shift_display(void);

/**
 * @brief Handles character printing on device
*/
void internal_command_print(void);

/**
 * @brief Prepares and initialises internal buffers and sequencer before being able to send data
*/
void prepare_i2c_buffer(const transmission_mode_t mode);

/**
 * @brief Internal manipulator used to set the backlight flag within internal I2C buffer
*/
void set_backlight_flag_in_i2c_buffer(void);

/**
 * @brief Tells whether the device is ready to accept instructions or not
*/
hd44780_lcd_error_t is_ready_to_accept_instruction(void);

/* Data handlers */
void handle_function_set(void);
void handle_display_controls(void);
void handle_entry_mode(void);
bool handle_byte_sending(void);
void handle_end_of_internal_command(bool byte_sent);
hd44780_lcd_error_t convert_i2c_write_error(const i2c_error_t error);


void bootup_sequence_handler(uint8_t time_to_wait, bool end_with_wait);
bool write_buffer(void);
void process_command_idling(void);

#ifdef UNIT_TESTING
    void get_process_command_sequencer(process_commands_sequencer_t ** const p_command_sequencer);
    uint8_t get_i2c_buffer(void);
    uint8_t get_data_byte(void);
    void set_data_byte(const uint8_t value);
    void set_i2c_buffer(const uint8_t value);
    void get_internal_configuration(internal_configuration_t ** const p_internal_configuration);
    void reset_command_sequencer(bool reset_all);
#endif

#ifdef __cplusplus
}
#endif

#endif /* HD44780_LCD_HEADER */