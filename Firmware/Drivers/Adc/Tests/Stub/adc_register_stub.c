#include "adc_register_stub.h"
#include <memory.h>

adc_register_stub_t adc_register_stub = {0};

void adc_register_stub_erase(adc_register_stub_t * stub)
{
    memset(stub, 0, sizeof(adc_register_stub_t));
}

void adc_register_stub_init_adc_handle(adc_handle_t * handle, adc_register_stub_t * const stub)
{
    handle->adcsra_reg = &stub->adcsra_reg;
    handle->adcsrb_reg = &stub->adcsrb_reg;
    handle->mux_reg = &stub->mux_reg;
    handle->readings.adchigh_reg = &stub->readings.adchigh_reg;
    handle->readings.adclow_reg = &stub->readings.adclow_reg;
}
