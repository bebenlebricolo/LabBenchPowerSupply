#include "adc.h"
#include "timer_8_bit.h"
#include "timer_8_bit_async.h"
#include "timer_16_bit.h"
#include "HD44780_lcd.h"
#include "timebase.h"
#include "i2c.h"

#include "driver_setup.h"
#include "module_setup.h"


#include <avr/interrupt.h>

#ifdef USE_SIMAVR_VCD_TRACE
    #include <simavr/avr/avr_mcu_section.h>
    #include "pin_mapping.h"
    AVR_MCU(16000000, "atmega328p");
    AVR_MCU_VCD_FILE("my_trace_file.vcd", 1000);

    const struct avr_mmcu_vcd_trace_t _mytrace[] _MMCU_ =
    {
        { AVR_MCU_VCD_SYMBOL("OC0A"), .mask = (1 << OC0A_PIN_NUMBER), .what = (void*)&OC0A_PORT_REG },
        { AVR_MCU_VCD_SYMBOL("OC0B"), .mask = (1 << OC0B_PIN_NUMBER), .what = (void*)&OC0B_PORT_REG },
        { AVR_MCU_VCD_SYMBOL("OC1A"), .mask = (1 << OC1A_PIN_NUMBER), .what = (void*)&OC1A_PORT_REG },
        { AVR_MCU_VCD_SYMBOL("OC1A"), .mask = (1 << OC1B_PIN_NUMBER), .what = (void*)&OC1B_PORT_REG },
    };
#endif

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
    #ifdef DEBUG_WITH_SIMAVR
        // Should start with the INT flag set, TWI peripheral boots up in its 'awaiting request' mode
        TWCR |= TWINT_MSK;
    #endif
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
    char msg[30] = "Hello World!\n";
    hd44780_lcd_state_t state = hd44780_lcd_get_state();
    hd44780_lcd_error_t err = HD44780_LCD_ERROR_OK;
    if (HD44780_LCD_STATE_READY != state)
    {
        err = hd44780_lcd_process();
    }
    else
    {
        err = hd44780_lcd_print(30U, msg);
    }
    (void) err;
}
