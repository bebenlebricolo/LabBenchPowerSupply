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
    HD44780_LCD_ERROR_OK,               /**< Everything went fine                                               */
    HD44780_LCD_ERROR_NULL_POINTER,     /**< A Null pointer was given (unitialised memory)                      */
    HD44780_LCD_ERROR_SIZE_ERROR,       /**< Message length exceeds controller's capacity, or message size is 0 */
    HD44780_LCD_INVALID_ADDRESS,        /**< Unsupported I2C address (exceeds 127)                              */
    HD44780_LCD_DEVICE_NOT_LISTENING,   /**< I/O expander did not respond to its address                        */
    HD44780_LCD_ERROR_UNKNOWN,          /**< Failure for an unsupported reason                                  */
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

typedef enum
{
    HD44780_LCD_MOVE_RIGHT,
    HD44780_LCD_MOVE_LEFT
} hd44780_lcd_move_action_t;
/* ##################################################################################################
   #################################### Configuration types #########################################
   ################################################################################################## */

/**
 * @brief basic configuration used at initialisation time to configure the LCD screen
*/
typedef struct
{
    uint8_t i2c_address;                    /**< Gives the I2C address of I/O expander PCF8574 (defaults to 0x27 if pins are not pulled to ground)  */
    hd44780_lcd_entry_mode_t entry_mode;    /**< Selects the kind of entry mode which is requested by the user upon typing                          */
    hd44780_lcd_lines_mode_t lines_mode;    /**< Selects the number of lines to be used                                                             */
    hd44780_lcd_font_t font;                /**< Selects the adequate font style between 5x8 dots and 5x10 dots                                     */
    
    struct                                  /**< Handles display's specific flags                                                                   */
    {
        bool display_enabled : 1;           /**< Tells whether the display is enabled or not (if disabled, data can still be written into controller's
                                                memory but nothing is displayed until it is enabled back)                                           */
        bool with_backlight  : 1;           /**< Selects whether the display uses a backlight (integrated LED controlled by I/O expander directly   */
        bool cursor_visible  : 1;           /**< Tells whether the cursor is enabled or not (i.e. visible on screen or not)                         */
        bool cursor_blinking : 1;           /**< Selects if the cursor is blinking or not                                                           */
    } display_controls;
} hd44780_lcd_config_t;

/* ##################################################################################################
   ########################################## General API ###########################################
   ################################################################################################## */



hd44780_lcd_error_t hd44780_lcd_clear(void);
hd44780_lcd_error_t hd44780_lcd_home(void);

hd44780_lcd_error_t hd44780_lcd_set_display_on_off(bool enabled);
hd44780_lcd_error_t hd44780_lcd_set_cursor_visible(bool visible);
hd44780_lcd_error_t hd44780_lcd_set_blinking_cursor(bool blinking);
hd44780_lcd_error_t hd44780_lcd_set_backlight(bool enabled);

hd44780_lcd_error_t hd44780_lcd_set_entry_mode(hd44780_entry_mode_t entry_mode);
hd44780_lcd_error_t hd44780_lcd_move_cursor_to_coord(uint8_t line, uint8_t column);
hd44780_lcd_error_t hd44780_lcd_move_cursor_near(hd44780_lcd_move_action_t move);
hd44780_lcd_error_t hd44780_lcd_shift_display(hd44780_lcd_move_action_t move);
hd44780_lcd_error_t hd44780_lcd_write(uint8_t length, char* buffer);




#ifdef __cplusplus
}
#endif

#endif /* HD44780_LCD_HEADER */