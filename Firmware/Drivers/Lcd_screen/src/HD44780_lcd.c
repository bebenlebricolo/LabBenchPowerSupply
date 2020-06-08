#include "HD44780_lcd.h"

// PCF8574 port mapping :
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
