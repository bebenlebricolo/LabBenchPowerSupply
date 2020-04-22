#include "timer_8_bit.h"
#include "timer_8_bit_async.h"
#include "timer_16_bit.h"

#include "driver_setup.h"
#include <avr/interrupt.h>

#define MAX_MUX 5
#define USE_TIMER_DRIVER 1

void error_handler(void)
{
    while(1)
    {
        asm("NOP");
    }
}

int main(void)
{
    driver_setup_error_t init_error = DRIVER_SETUP_ERROR_OK;

#ifdef USE_TIMER_DRIVER
    init_error = init_timer_0();
#else
    TCCR0A = (1 << WGM01) | (1 << WGM00) | (1 << COM0A0) | (1 << COM0B0) | (1 << COM0B1)  ;
    TCCR0B = (1 << CS01);
#endif

    if (DRIVER_SETUP_ERROR_OK != init_error)
    {
        error_handler();
    }

#ifdef USE_TIMER_DRIVER
    timer_error_t timer_error = timer_8_bit_start(0);
    if (TIMER_ERROR_OK != timer_error)
    {
        error_handler();
    }
	
	
#endif

    while(true)
    {
        static uint8_t idx = 0;

        idx++;
        idx %= MAX_MUX;
    }

    return 0;
}