#include "timer_8_bit_async_registers_stub.h"
#include <string.h>

timer_8_bit_async_registers_stub_t timer_8_bit_async_registers_stub = {0};

void timer_8_bit_async_registers_stub_erase(void)
{
    memset(&timer_8_bit_async_registers_stub, 0, sizeof(timer_8_bit_async_registers_stub_t));
}

void timer_8_bit_async_registers_stub_init_handle(timer_8_bit_async_handle_t * handle)
{
    if (NULL != handle)
    {
        handle->OCRA = &timer_8_bit_async_registers_stub.OCRA;
        handle->OCRB = &timer_8_bit_async_registers_stub.OCRB;
        handle->TCCRA = &timer_8_bit_async_registers_stub.TCCRA;
        handle->TCCRB = &timer_8_bit_async_registers_stub.TCCRB;
        handle->TCNT = &timer_8_bit_async_registers_stub.TCNT;
        handle->TIMSK = &timer_8_bit_async_registers_stub.TIMSK;
        handle->TIFR = &timer_8_bit_async_registers_stub.TIFR;
        handle->ASSR_REG = &timer_8_bit_async_registers_stub.ASSR_REG;
    }
}


