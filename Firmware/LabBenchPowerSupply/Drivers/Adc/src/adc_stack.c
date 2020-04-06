#include "adc_stack.h"

adc_stack_error_t adc_registered_channels_clear(adc_registered_channels_t * const stack)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_WRONG_POINTER;
    }
    else
    {
        stack->count = 0;
        stack->index = 0;
        for (uint8_t i = 0 ; i < ADC_MUX_COUNT ; i++)
        {
            stack->channels_pair[i].channel = ADC_MUX_GDN;
            stack->channels_pair[i].locked  = false;
            stack->channels_pair[i].result  = 0;
        }
    }
    
    return ret;
}

adc_stack_error_t adc_registered_channels_add_channel(adc_registered_channels_t * const stack, const adc_mux_t mux)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_WRONG_POINTER;
    }
    else
    {
        /* Check if we can add one more element */
        if( ADC_MUX_COUNT < stack->count + 1)
        {
            ret = ADC_STACK_ERROR_FULL;
        }
    }

    /* Add new element in the stack */
    if (ADC_STACK_ERROR_OK == ret)
    {
        stack->count++;
        stack->channels_pair[stack->count - 1].channel = mux;
    }
    
    return ret;
}

adc_stack_error_t adc_registered_channels_remove_channel(adc_registered_channels_t * const stack, const adc_mux_t mux)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_WRONG_POINTER;
    }
    else
    {
        /* Check if we can remove an element */
        if (0 == stack->count)
        {
            ret = ADC_STACK_ERROR_EMPTY;
        }
    }

    /* Remove one element from the stack and clean previous entry */
    if (ADC_STACK_ERROR_OK == ret)
    {
        stack->count--;
        stack->channels_pair[stack->count].channel = ADC_MUX_GDN;
        stack->channels_pair[stack->count].locked = false;
        stack->channels_pair[stack->count].result = 0;
    }
    return ret;
}

adc_stack_error_t adc_registered_channels_get_next(adc_registered_channels_t * const stack, adc_channel_pair_t * const pair)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_WRONG_POINTER;
    }
    else
    {
        /* Check that we can target at least one element of the array */
        if (0 == stack->count)
        {
            ret = ADC_STACK_ERROR_EMPTY;
        }
    }  
    
    /* Move to next element and return its address */
    if (ADC_STACK_ERROR_OK == ret)
    {
        stack->index++;
        stack->index %= stack->count;
        pair = &(stack->channels_pair[stack->index]);
    }

    return ret;
}
