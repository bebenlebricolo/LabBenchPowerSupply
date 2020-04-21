#include "timer_16_bit_registers_stub.h"
#include <string.h>

timer_16_bit_registers_stub_t timer_16_bit_registers_stub = {0};

void timer_16_bit_registers_stub_erase(void)
{
    memset(&timer_16_bit_registers_stub, 0, sizeof(timer_16_bit_registers_stub_t));
}

void timer_16_bit_registers_stub_init_handle(timer_16_bit_handle_t * handle)
{
    if (NULL != handle)
    {
        handle->TCCRA = &timer_16_bit_registers_stub.TCCRA;
        handle->TCCRB = &timer_16_bit_registers_stub.TCCRB;
        handle->TCCRC = &timer_16_bit_registers_stub.TCCRC;
        handle->OCRA_H = &timer_16_bit_registers_stub.OCRA_H;
        handle->OCRA_L = &timer_16_bit_registers_stub.OCRA_L;
        handle->OCRB_H = &timer_16_bit_registers_stub.OCRB_H;
        handle->OCRB_L = &timer_16_bit_registers_stub.OCRB_L;
        handle->TCNT_H = &timer_16_bit_registers_stub.TCNT_H;
        handle->TCNT_L = &timer_16_bit_registers_stub.TCNT_L;
        handle->ICR_H = &timer_16_bit_registers_stub.ICR_H;
        handle->ICR_L = &timer_16_bit_registers_stub.ICR_L;
        handle->TIMSK = &timer_16_bit_registers_stub.TIMSK;
        handle->TIFR = &timer_16_bit_registers_stub.TIFR;
    }
}


