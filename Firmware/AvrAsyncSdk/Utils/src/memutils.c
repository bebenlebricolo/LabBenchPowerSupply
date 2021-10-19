#include "memutils.h"

void volatile_memset(volatile void * const ptr, const uint8_t value, const size_t size)
{
    volatile uint8_t * data = ptr;
    for (size_t i = 0 ; i < size ; i++)
    {
        *(data + i) = value;
    }
}

void volatile_memcpy(volatile void* const dest, volatile void* const src, const size_t size)
{
    volatile uint8_t * dest_data = dest;
    volatile uint8_t * src_data = src;
    for (size_t i = 0 ; i < size ; i++)
    {
        *(dest_data + i) = *(src_data + i);
    }
}
