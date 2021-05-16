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

#include "adc.h"
#include "timer_8_bit.h"
#include "timer_8_bit_async.h"
#include "timer_16_bit.h"
#include "HD44780_lcd.h"
#include "timebase.h"
#include "i2c.h"

#include "driver_setup.h"
#include "module_setup.h"

#include <stdlib.h>
#include <string.h>

#include <avr/interrupt.h>

#define MAX_MUX 5

static adc_mux_t mux_table[MAX_MUX] =
{
    ADC_MUX_ADC0,
    ADC_MUX_ADC1,
    ADC_MUX_ADC2,
    ADC_MUX_ADC3,
    ADC_MUX_ADC4,
};

static void error_handler(void);
static void bootup_sequence(void);
static driver_setup_error_t adc_register_all_channels(void);
static void adc_read_values(void);
static void print_data(void);

ISR(ADC_vect)
{
    adc_isr_handler();
}

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
        adc_read_values();
        print_data();
    }

    return 0;
}

/* ########################################################################################
   ########################## Static functions definitions ################################
   ######################################################################################## */

void adc_read_values(void)
{
    static uint8_t idx = 0;
    adc_result_t results[5];
    adc_read_raw(mux_table[idx % MAX_MUX], &results[idx]);

    idx++;
    idx %= MAX_MUX;
}

void error_handler(void)
{
    while(1)
    {
    }
}

driver_setup_error_t adc_register_all_channels(void)
{
    for (uint8_t i = 0; i < MAX_MUX ; i++)
    {
        adc_error_t err = adc_register_channel(mux_table[i]);
        if (ADC_ERROR_OK != err)
        {
            return DRIVER_SETUP_ERROR_INIT_FAILED;
        }
    }
    return DRIVER_SETUP_ERROR_OK;
}

static void bootup_sequence(void)
{

    driver_setup_error_t driver_init_error = DRIVER_SETUP_ERROR_OK;
    module_setup_error_t module_init_error = MODULE_SETUP_ERROR_OK;

    /* Set up 8 bit timer 0 as 8 bit FAST PWM generator */
    driver_init_error = driver_init_timer_0();
    if (DRIVER_SETUP_ERROR_OK != driver_init_error)
    {
        error_handler();
    }

    /* Set up 16 bit timer 1 as 10 bit FAST PWM generator */
    driver_init_error = driver_init_timer_1();
    if (DRIVER_SETUP_ERROR_OK != driver_init_error)
    {
        error_handler();
    }

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

    driver_init_error = driver_init_adc();
    if (DRIVER_SETUP_ERROR_OK != driver_init_error)
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

    driver_init_error = adc_register_all_channels();
    if (DRIVER_SETUP_ERROR_OK != driver_init_error)
    {
        error_handler();
    }

    adc_start();
    sei();

    /* Start both timers */
    timer_error_t timer_error = timer_8_bit_start(0);
    if (TIMER_ERROR_OK != timer_error)
    {
        error_handler();
    }
    timer_error = timer_16_bit_start(0);
    if (TIMER_ERROR_OK != timer_error)
    {
        error_handler();
    }
    timer_error = timer_8_bit_async_start(0);
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
                    // Replace null-terminating characters in string
                    //for (uint8_t i = len ; i < 5U ; i++)
                    //{
                    //    if (iteration_string[i] == 0)
                    //    {
                    //        iteration_string[i] = ' ';
                    //    }
                    //}
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
