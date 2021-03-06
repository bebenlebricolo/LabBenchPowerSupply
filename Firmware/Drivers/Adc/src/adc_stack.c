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

#include <stddef.h>
#include <stdbool.h>
#include "adc_stack.h"

adc_stack_error_t adc_stack_reset(volatile adc_stack_t * const stack)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_ERROR_NULL_POINTER;
    }
    else
    {
        stack->count = 0;
        stack->index = 0;
        for (uint8_t i = 0 ; i < ADC_MUX_COUNT ; i++)
        {
            /* resets targeted pair to defaults */
            adc_channel_pair_reset(&stack->channels_pair[i]);
        }
    }

    return ret;
}

adc_stack_error_t adc_channel_pair_copy(volatile adc_channel_pair_t * dest, volatile adc_channel_pair_t * const src)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == dest || NULL == src)
    {
        ret = ADC_STACK_ERROR_NULL_POINTER;
    }
    else
    {
        dest->channel = src->channel;
        dest->result = src->result;
    }
    return ret;
}

adc_stack_error_t adc_channel_pair_reset(volatile adc_channel_pair_t * const pair)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == pair)
    {
        ret = ADC_STACK_ERROR_NULL_POINTER;
    }
    else
    {
        pair->channel = ADC_MUX_GND;
        pair->result = 0;
    }
    return ret;
}


adc_stack_error_t adc_stack_register_channel(volatile adc_stack_t * const stack, volatile const adc_mux_t mux)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_ERROR_NULL_POINTER;
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

adc_stack_error_t adc_stack_unregister_channel(volatile adc_stack_t * const stack, volatile const adc_mux_t mux)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_ERROR_NULL_POINTER;
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

            /* resets targeted pair to defaults */
            adc_channel_pair_reset(&stack->channels_pair[stack->count]);
            if (stack->index == index && stack->count != 0)
            {
                stack->index = (stack->count + stack->index - 1) % stack->count;
            }

        }
    }
    return ret;
}

adc_stack_error_t adc_stack_find_channel(volatile adc_stack_t * const stack, volatile const adc_mux_t channel, volatile adc_channel_pair_t ** pair)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_ERROR_NULL_POINTER;
    }
    else
    {
        /* Check if we can remove an element */
        if (0 == stack->count)
        {
            ret = ADC_STACK_ERROR_EMPTY;
        }
    }

    /* Get element address */
    if (ADC_STACK_ERROR_OK == ret)
    {
        bool found_item = false;
        for (uint8_t i = 0 ; i < stack->count ; i++ )
        {
            if (stack->channels_pair[i].channel == channel)
            {
                // Found first matching channel
                *pair = &stack->channels_pair[i];
                found_item = true;
                break;
            }
        }
        if (!found_item)
        {
            *pair = NULL;
            ret = ADC_STACK_ERROR_ELEMENT_NOT_FOUND;
        }
    }
    return ret;
}


adc_stack_error_t adc_stack_get_next(volatile adc_stack_t * const stack, volatile adc_channel_pair_t ** pair)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_ERROR_NULL_POINTER;
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
        *pair = &(stack->channels_pair[stack->index]);
    }

    return ret;
}

adc_stack_error_t adc_stack_get_current(volatile adc_stack_t * const stack, volatile adc_channel_pair_t ** pair)
{
    adc_stack_error_t ret = ADC_STACK_ERROR_OK;
    if (NULL == stack)
    {
        ret = ADC_STACK_ERROR_NULL_POINTER;
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
        *pair = &(stack->channels_pair[stack->index]);
    }

    return ret;
}

