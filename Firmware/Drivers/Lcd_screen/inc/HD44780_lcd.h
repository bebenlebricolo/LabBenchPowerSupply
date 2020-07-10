#ifndef HD44780_LCD_HEADER
#define HD44780_LCD_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>

/* Based on Hitachi HD44780 LCD display device with the PCF8574 I/O port expander connected to the I2C bus*/
/* reference for PCF8574 expander : http://www.ti.com/lit/ds/symlink/pcf8574.pdf?ts=1591255583129*/

/*
    Check HD44780 datasheet : https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
*/

/* ##################################################################################################
   #################################### Basic exposed types #########################################
   ################################################################################################## */

/**
 * @brief Global handling error codes
*/
typedef enum
{
    /* Generic interface errors */
    HD44780_LCD_ERROR_OK,                /**< Everything went fine                                               */
    HD44780_LCD_ERROR_NULL_POINTER,      /**< A Null pointer was given (unitialised memory)                      */
    HD44780_LCD_ERROR_SIZE_ERROR,        /**< Message length exceeds controller's capacity, or message size is 0 */
    HD44780_LCD_ERROR_UNSUPPORTED_VALUE, /**< A general error telling a given enumerate value is not supported   */

    /* I2C related errors */
    HD44780_LCD_ERROR_INVALID_ADDRESS,         /**< Unsupported I2C address (exceeds 127)                              */
    HD44780_LCD_ERROR_DEVICE_NOT_LISTENING,    /**< I/O expander did not respond to its address                        */

    /* Sequence-related errors (wrong states) */
    HD44780_LCD_ERROR_DEVICE_BUSY,             /**< Device is already processing and will not accept new instructions  */
    HD44780_LCD_ERROR_DEVICE_NOT_INITIALISED,  /**< Device is not initialised and cannot receive instructions          */
    HD44780_LCD_ERROR_DEVICE_WRONG_STATE,      /**< Current device state does not allow the requested operation        */

    HD44780_LCD_ERROR_UNKNOWN,           /**< Failure for an unsupported reason                                  */
} hd44780_lcd_error_t;

/**
 * @brief Describes entry mode set instruction set
*/
typedef enum
{
    // Cursor move direction
    HD44780_LCD_ENTRY_MODE_CURSOR_MOVE_LEFT     = 0x00, /**< Moves cursor to left when new character is written                     */
    HD44780_LCD_ENTRY_MODE_CURSOR_MOVE_RIGHT    = 0x02, /**< Moves cursor to right when new character is written                    */

    // Display shift direction
    HD44780_LCD_ENTRY_MODE_DISPLAY_SHIFT_RIGHT  = 0x01, /**< Moves the entire display to the right when a new character is written
                                                             Cursor will visually remain at the same place while the text shifts    */
    HD44780_LCD_ENTRY_MODE_DISPLAY_SHIFT_LEFT   = 0x03, /**< Moves the entire display to the left when a new character is written
                                                             Cursor will visually remain at the same place while the text shifts    */
} hd44780_lcd_entry_mode_t;


/**
 * @brief Encodes Font style
*/
typedef enum
{
    HD44780_LCD_FONT_5x8  = 0x00,
    HD44780_LCD_FONT_5x10 = 0x04,
} hd44780_lcd_font_t;

/**
 * @brief Encodes displayed lines count
*/
typedef enum
{
    HD44780_LCD_LINES_1_LINE    = 0x00,
    HD44780_LCD_LINES_2_LINES   = 0x08,
} hd44780_lcd_lines_mode_t;

/**
 * @brief Encodes data length used to communicate with HD44780
*/
typedef enum
{
    HD44780_LCD_DATA_LENGTH_4_BITS = 0x00,
    HD44780_LCD_DATA_LENGTH_8_BITS = 0x10,
} hd44780_lcd_data_length_t;

/**
 * @brief depicts available cursor movements
*/
typedef enum
{
    HD44780_LCD_CURSOR_MOVE_RIGHT,  /**< Cursor moves right relatively to its current location  */
    HD44780_LCD_CURSOR_MOVE_LEFT,   /**< Cursor moves left relatively to its current location   */
    HD44780_LCD_CURSOR_MOVE_UP,     /**< Cursor moves up relatively to its current location     */
    HD44780_LCD_CURSOR_MOVE_DOWN,   /**< Cursor moves down relatively to its current location   */
} hd44780_lcd_cursor_move_action_t;

/**
 * @brief depicts available screen shifting movements
*/
typedef enum
{
    HD44780_LCD_DISPLAY_SHIFT_RIGHT,    /**< Display is moved to right (1 character wide movement) */
    HD44780_LCD_DISPLAY_SHIFT_LEFT,     /**< Display is moved to left (1 character wide movement) */
} hd44780_lcd_display_shift_t;

/* ##################################################################################################
   #################################### Configuration types #########################################
   ################################################################################################## */

/**
 * @brief basic configuration used at initialisation time to configure the LCD screen
*/
typedef struct
{
    uint8_t i2c_address;                            /**< Gives the I2C address of I/O expander PCF8574 (defaults to 0x27 if pins are not pulled to ground)  */

    /* Handles dependencies indexes */
    struct {
        uint8_t timebase : 4;                       /**< Selects which timebase will be used to operate with proper timing                                  */
        uint8_t i2c : 4;                            /**< Gives the I2C instance index                                                                       */
    } indexes;

    /* Handles lcd printing parameters */
    struct {
        hd44780_lcd_entry_mode_t entry_mode;    /**< Selects the kind of entry mode which is requested by the user upon typing                          */
        hd44780_lcd_lines_mode_t lines_mode;    /**< Selects the number of lines to be used                                                             */
        hd44780_lcd_font_t font;                /**< Selects the adequate font style between 5x8 dots and 5x10 dots                                     */
    } print_controls;

    /* Handles display's specific flags */
    struct {
        bool display_enabled : 1;                   /**< Tells whether the display is enabled or not (if disabled, data can still be written into controller's
                                                        memory but nothing is displayed until it is enabled back)                                           */
        bool with_backlight  : 1;                   /**< Selects whether the display uses a backlight (integrated LED controlled by I/O expander directly   */
        bool cursor_visible  : 1;                   /**< Tells whether the cursor is enabled or not (i.e. visible on screen or not)                         */
        bool cursor_blinking : 1;                   /**< Selects if the cursor is blinking or not                                                           */
    } display_controls;
} hd44780_lcd_config_t;

/**
 * @brief Describes internal states of this driver, which is key to use asynchronously
*/
typedef enum
{
    HD44780_LCD_STATE_NOT_INITIALISED,  /**< Device is not yet initialised                                                                                   */
    HD44780_LCD_STATE_INITIALISING,     /**< Device is currently performing initialisation steps                                                             */
    HD44780_LCD_STATE_DEINITIALISING,   /**< Device is being reverted to the NOT INITIALISED state (reset to default, blank device state)                    */
    HD44780_LCD_STATE_READY,            /**< Device is ready to accept instructions                                                                          */
    HD44780_LCD_STATE_PROCESSING,       /**< Device is currently processing instructions and could not accept new ones until it goes back to the READY state */
} hd44780_lcd_state_t;



/* ##################################################################################################
   ########################################## General API ###########################################
   ################################################################################################## */

/**
 * @brief Resets the driver to its original state
 * @return HD44780_LCD_ERROR_OK
*/
hd44780_lcd_error_t hd44780_lcd_driver_reset(void);

/**
 * @brief returns the last encountered error
*/
hd44780_lcd_error_t hd44780_lcd_get_last_error(void);

/**
 * @brief Initialises LCD screen using the given configuration data
 * @param[in] config    :   device configuration (readonly)
 * @return hd44780_lcd_error_t
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_ERROR_NULL_POINTER      :   Given pointer is uninitialised
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
*/
hd44780_lcd_error_t hd44780_lcd_init(hd44780_lcd_config_t const * const config);

/**
 * @brief Gives a default configuration to start with
 * @param[in/out] config    :   device configuration
 * @return hd44780_lcd_error_t
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_ERROR_NULL_POINTER      :   Given pointer is uninitialised
*/
hd44780_lcd_error_t hd44780_lcd_get_default_config(hd44780_lcd_config_t * const config);

/**
 * @brief Deinitialises device and reverts it to its default state (display off, backlight off, Old data still in RAM)
 * @note this function may only be called when device has already been initialised (otherwise, communication will essentially fail)
 * @return hd44780_lcd_error_t
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_DEVICE_WRONG_STATE      :   Actual state does not support this operation
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
*/
hd44780_lcd_error_t hd44780_lcd_deinit(void);

/**
 * @brief Main function of this API which needs to be called as often as possible in the aim to keep the clock ticking!
 * @note this function may only be called when device has already been initialised (otherwise, communication will essentially fail)
 * @return hd44780_lcd_error_t
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_DEVICE_WRONG_STATE      :   Actual state does not support this operation
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
*/
hd44780_lcd_error_t hd44780_lcd_process(void);

/**
 * @brief Gets the current state of internal finite state machine
 * @return hd44780_lcd_state_t : current state
*/
hd44780_lcd_state_t hd44780_lcd_get_state(void);



/* ##################################################################################################
   #################################### Single manipulators #########################################
   ################################################################################################## */

/**
 * @brief clears the while screen (DDRAM is filled with whitespaces)
 * @return
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
 *      HD44780_LCD_DEVICE_NOT_INITIALISED  :   Device is not initialised yet, perform an initialisation cycle before using this function
*/
hd44780_lcd_error_t hd44780_lcd_clear(void);

/**
 * @brief Sets the cursor to "Home", which is line 0, column 0
 * @return
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
 *      HD44780_LCD_DEVICE_NOT_INITIALISED  :   Device is not initialised yet, perform an initialisation cycle before using this function
*/
hd44780_lcd_error_t hd44780_lcd_home(void);

/**
 * @brief Sets the current state of the display (switch it on/off)
 * @param[in] enabled : selects whether the display is on or off
 * @return
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
 *      HD44780_LCD_DEVICE_NOT_INITIALISED  :   Device is not initialised yet, perform an initialisation cycle before using this function
*/
hd44780_lcd_error_t hd44780_lcd_set_display_on_off(const bool enabled);

/**
 * @brief Sets the visibility of the cursor.
 * @param[in] visible : selects whether the cursor is visible or not
 * @return
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
 *      HD44780_LCD_DEVICE_NOT_INITIALISED  :   Device is not initialised yet, perform an initialisation cycle before using this function
*/
hd44780_lcd_error_t hd44780_lcd_set_cursor_visible(const bool visible);

/**
 * @brief Sets the blinking function of the cursor on and off
 * @param[in] blinking : true will set the cursor blinking, false switches off this functionality
 * @return
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
 *      HD44780_LCD_DEVICE_NOT_INITIALISED  :   Device is not initialised yet, perform an initialisation cycle before using this function
*/
hd44780_lcd_error_t hd44780_lcd_set_blinking_cursor(const bool blinking);

/**
 * @brief Enables or disables the LCD backlight LED
 * @param[in] enabled : Sets the backlight state
 * @return
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
 *      HD44780_LCD_DEVICE_NOT_INITIALISED  :   Device is not initialised yet, perform an initialisation cycle before using this function
*/
hd44780_lcd_error_t hd44780_lcd_set_backlight(const bool enabled);

/**
 * @brief Configures the entry mode of the LCD.
 * @details This drives how the LCD controller moves the display or cursor when a new character is entered.
 *          For instance, if entry mode is set to HD44780_LCD_ENTRY_MODE_CURSOR_MOVE_LEFT, cursor will go
 *          backward when a new character is typed-in, which means we can write right to left.
 *
 * @param[in] entry_mode : Selected entry mode
 * @return
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_ERROR_UNSUPPORTED_VALUE :   Input parameter does not resolve do any supported value
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
 *      HD44780_LCD_DEVICE_NOT_INITIALISED  :   Device is not initialised yet, perform an initialisation cycle before using this function
*/
hd44780_lcd_error_t hd44780_lcd_set_entry_mode(const hd44780_lcd_entry_mode_t entry_mode);

/**
 * @brief Configures the display modes of LCD screen
 * @details This functions allows to configure the number of lines and the font kind the user
 *          wants to use with this device
 *
 * @param[in] p_font :  selected font : 5x8 (one line or 2 lines) or 5x10 (1 line only)
 * @return
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_ERROR_UNSUPPORTED_VALUE :   Input parameter does not resolve do any supported value
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
 *      HD44780_LCD_DEVICE_NOT_INITIALISED  :   Device is not initialised yet, perform an initialisation cycle before using this function
*/
hd44780_lcd_error_t hd44780_lcd_confgure_display(hd44780_lcd_font_t p_font, hd44780_lcd_lines_mode_t p_line_mode);

/**
 * @brief Moves the cursor to a given location (absolute, origin point located at screen top left corner (0,0))
 * @note Input line and column parameters are checked against maximum screen capacity.
 *       If values are set wrong, you'll get an HD44780_LCD_ERROR_UNSUPPORTED_VALUE error.
 * @param[in] line      :   Line number of cursor position (which stands for the 'Y' coordinate)
 * @param[in] column    :   Column number of cursor position (stands for the 'X' coordinate)
 * @return
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_ERROR_UNSUPPORTED_VALUE :   Input parameter does not resolve do any supported value
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
 *      HD44780_LCD_DEVICE_NOT_INITIALISED  :   Device is not initialised yet, perform an initialisation cycle before using this function
*/
hd44780_lcd_error_t hd44780_lcd_move_cursor_to_coord(const uint8_t line, const uint8_t column);

/**
 * @brief Moves the cursor relatively to its current location
 * @param[in] move : Selected cursor movement (1rst reads current location and process new location starting from there)
 * @return
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_ERROR_UNSUPPORTED_VALUE :   Input parameter does not resolve do any supported value
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
 *      HD44780_LCD_DEVICE_NOT_INITIALISED  :   Device is not initialised yet, perform an initialisation cycle before using this function
*/
hd44780_lcd_error_t hd44780_lcd_move_relative(const hd44780_lcd_cursor_move_action_t move);

/**
 * @brief Shifts the whole display left or right (1 character wide movement)
 * @param[in] shift : Selected display shift movement
 * @return
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_ERROR_UNSUPPORTED_VALUE :   Input parameter does not resolve do any supported value
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
 *      HD44780_LCD_DEVICE_NOT_INITIALISED  :   Device is not initialised yet, perform an initialisation cycle before using this function
*/
hd44780_lcd_error_t hd44780_lcd_shift_display(const hd44780_lcd_display_shift_t shift);

/**
 * @brief Inputs all message characters into LCD Screen internal RAM
 * @param[in] length    :   message length
 * @param[in] buffer    :   message buffer
 * @note length parameter is checked against the maximum available length of LCD screen. If length is bigger, you'll receive HD44780_LCD_ERROR_SIZE_ERROR error
 * @return
 *      HD44780_LCD_ERROR_OK                :   Operation succeeded
 *      HD44780_LCD_ERROR_SIZE_ERROR        :   Input message length is too big compared to slave's internal data buffer
 *      HD44780_LCD_DEVICE_NOT_LISTENING    :   Could not get any response from slave (I2C NACK everywhere)
 *      HD44780_LCD_DEVICE_BUSY             :   Device is already processing instructions
 *      HD44780_LCD_DEVICE_NOT_INITIALISED  :   Device is not initialised yet, perform an initialisation cycle before using this function
*/
hd44780_lcd_error_t hd44780_lcd_print(const uint8_t length, char const * const buffer);



#ifdef __cplusplus
}
#endif

#endif /* HD44780_LCD_HEADER */