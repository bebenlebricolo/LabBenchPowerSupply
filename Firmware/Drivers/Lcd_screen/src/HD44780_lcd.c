#include "HD44780_lcd.h"

/* ##################################################################################################
   #################################### PCF8574 I/O expander ########################################
   ################################################################################################## */

// PCF8574 port mapping :
// | 7  | 6  | 5  | 4  | 3  | 2  | 1  | 0  |
// | D3 | D2 | D1 | D0 | BL | E  | RW | RS |

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
#define PCF8574_D0_BIT              (4U)    // D4
#define PCF8574_D1_BIT              (5U)    // D5
#define PCF8574_D2_BIT              (6U)    // D6
#define PCF8574_D3_BIT              (7U)    // D7

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
    HD44780_LCD_CMD_SET_DD_RAM_ADDR    = 0x18,  /**< Sets the current address pointer of DDRAM (from 0 to 127)                                                          */
} hd44780_lcd_command_t;

/* ##################################################################################################
   ################################### Internal data management #####################################
   ################################################################################################## */

// Internal state machine persistent memory
static hd44780_lcd_state_t internal_state = HD44780_LCD_STATE_NOT_INITIALISED;
static struct
{
    uint8_t i2c_address;    /**< I/O expander address */

    // Bitfield storing persistent data about display state
    // All fields might be encoded within one CPU word (8 bits)
    struct
    {
        bool backlight : 1;         /**< true : backlight on,       false : backlight off                                */
        bool enabled : 1;           /**< true : display on,         false : display off                                  */
        bool cursor_visible : 1;    /**< true : cursor visible,     false : cursor not displayed                         */
        bool cursor_blinking : 1;   /**< true : blinking cursor,    false : persistent cursor                            */
        bool two_lines_mode : 1;    /**< true : two lines mode,     false : one line mode (5x8 and 5x10 fonts supported) */
        bool small_font : 1;        /**< true : 5x8 font used,      false : 5x10 dots font, single line only             */
    } display;
} internal_configuration = {0};
