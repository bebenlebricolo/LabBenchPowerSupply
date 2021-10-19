#include <stdlib.h>
#include <string.h>
#include <avr/interrupt.h>

#include "i2c.h"
#include "timebase.h"
#include "HD44780_lcd.h"
#include "timer_8_bit_async.h"

#include "driver_setup.h"
#include "module_setup.h"


static void error_handler(void);
static void bootup_sequence(void);
static void print_data(void);

ISR(TIMER2_COMPA_vect)
{
    timebase_interrupt_callback(0U);
}

int main(void)
{
    bootup_sequence();

    i2c_error_t i2c_err = I2C_ERROR_OK;

    while(true)
    {
        i2c_err = i2c_process(0U);
        (void) i2c_err;
        print_data();
    }

    return 0;
}

/* ########################################################################################
   ########################## Static functions definitions ################################
   ######################################################################################## */

void error_handler(void)
{
    while(1)
    {
    }
}

static void bootup_sequence(void)
{

    driver_setup_error_t driver_init_error = DRIVER_SETUP_ERROR_OK;
    module_setup_error_t module_init_error = MODULE_SETUP_ERROR_OK;

    /* Set up 8 bit timer 2 as 8 bit FAST PWM generator */
    driver_init_error = driver_init_timer_2();
    if (DRIVER_SETUP_ERROR_OK != driver_init_error)
    {
        error_handler();
    }

    module_init_error = module_init_timebase();
    if (MODULE_SETUP_ERROR_OK != module_init_error)
    {
        error_handler();
    }

    driver_init_error = driver_init_i2c();
    if (DRIVER_SETUP_ERROR_OK != driver_init_error)
    {
        error_handler();
    }

    driver_init_error = driver_init_lcd();
    if (DRIVER_SETUP_ERROR_OK != driver_init_error)
    {
        error_handler();
    }

    /* Start both timers */
    timer_error_t timer_error = timer_8_bit_async_start(0);
    if (TIMER_ERROR_OK != timer_error)
    {
        error_handler();
    }
}

static void print_data(void)
{
    static char msg1[30] = "Hello World!";
    static char msg2[30] = "i = ";
    static uint8_t state_count = 0;
    static uint8_t iterations = 0;
    static bool move_cursor = false;

    static char iteration_string[5] = "";

    hd44780_lcd_state_t state = hd44780_lcd_get_state();
    hd44780_lcd_error_t err = HD44780_LCD_ERROR_OK;

    if (HD44780_LCD_STATE_READY != state)
    {
        err = hd44780_lcd_process();
    }
    else
    {
        switch(state_count)
        {
            case 0 :
                err = hd44780_lcd_set_display_on_off(true);
                break;
            case 1 :
                err = hd44780_lcd_move_cursor_to_coord(0,0);
                break;
            case 2 :
                err = hd44780_lcd_print(strnlen(msg1, 30U), msg1);
                break;
            case 3 :
                err = hd44780_lcd_move_cursor_to_coord(1,0);
                break;
            case 4 :
                err = hd44780_lcd_print(strnlen(msg2, 30U), msg2);
                break;
            case 5 :
                err = hd44780_lcd_set_blinking_cursor(false);
                break;
            case 6:
                err = hd44780_lcd_set_cursor_visible(false);
                break;
            case 7 :
                if (move_cursor)
                {
                    err = hd44780_lcd_move_cursor_to_coord(1, 4U);
                    move_cursor = false;
                }
                else
                {
                    itoa(iterations, iteration_string, 10U);
                    int len = strnlen(iteration_string, 5U);
                    memset(&iteration_string[len], ' ', (5U - len));
                    err = hd44780_lcd_print(3U, iteration_string);
                    iterations++;
                    move_cursor = true;
                }
                break;

            default:
                break;
        }

        if (state_count < 7U)
        {
            state_count++;
        }
    }
    (void) err;
}
