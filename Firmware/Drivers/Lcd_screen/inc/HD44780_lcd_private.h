#ifndef HD44780_LCD_PRIVATE_HEADER
#define HD44780_LCD_PRIVATE_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include "HD44780_lcd.h"

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
void initialise_buffer_and_sequencer(const transmission_mode_t mode);

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


void bootup_sequence_handler(uint8_t time_to_wait, bool end_with_wait);
bool write_buffer(void);


#ifdef UNIT_TESTING
uint8_t get_i2c_buffer(void);
uint8_t get_data_byte(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* HD44780_LCD_HEADER */