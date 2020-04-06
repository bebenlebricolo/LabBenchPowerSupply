#include <stddef.h>
#include "adc_stack.h"

adc_stack_error_t adc_stack_clear(adc_stack_t * const stack)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_ERROR_WRONG_POINTER;
    }
    else
    {
        stack->count = 0;
        stack->index = 0;
        for (uint8_t i = 0 ; i < ADC_MUX_COUNT ; i++)
        {
            stack->channels_pair[i].channel = ADC_MUX_GND;
            stack->channels_pair[i].locked  = false;
            stack->channels_pair[i].result  = 0;
        }
    }
    
    return ret;
}

adc_stack_error_t adc_channel_pair_copy(adc_channel_pair_t * dest, adc_channel_pair_t * const src)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == dest || NULL == src)
    {
        ret = ADC_STACK_ERROR_WRONG_POINTER;
    }
    else
    {
        dest->channel = src->channel;
        dest->locked = src->locked;
        dest->result = src->result;
    }
    return ret;
}

adc_stack_error_t adc_stack_register_channel(adc_stack_t * const stack, const adc_mux_t mux)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_ERROR_WRONG_POINTER;
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

adc_stack_error_t adc_stack_unregister_channel(adc_stack_t * const stack, const adc_mux_t mux)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_ERROR_WRONG_POINTER;
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
        uint8_t index = ADC_MUX_COUNT;
        for (uint8_t i = 0 ; i < stack->count ; i++ )
        {
            if (stack->channels_pair[i].channel == mux)
            {
                // Found first matching channel
                index = i;
                break;
            }
        }

        /* If we haven't found any match */
        if (ADC_MUX_COUNT == index)
        {
            ret = ADC_STACK_ERROR_ELEMENT_NOT_FOUND;
        }
        else 
        {
            for (uint8_t i = index ; i < stack->count - 1 ; i++)
            {
                adc_channel_pair_copy(&stack->channels_pair[i], &stack->channels_pair[i+1]);
            }
            stack->count--;
            stack->channels_pair[stack->count].channel = ADC_MUX_GND;
            stack->channels_pair[stack->count].locked = false;
            stack->channels_pair[stack->count].result = 0;
            if (stack->index == index && stack->count != 0)
            {
                stack->index = (stack->count + stack->index - 1) % stack->count;
            }

        }
    }
    return ret;
}

adc_stack_error_t adc_stack_get_next(adc_stack_t * const stack, adc_channel_pair_t * pair)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_ERROR_WRONG_POINTER;
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
