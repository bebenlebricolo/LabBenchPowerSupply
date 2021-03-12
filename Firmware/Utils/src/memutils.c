#include "memutils.h"

void volatile_memset(volatile void * const ptr, const uint8_t value, const size_t size)
{
    for (size_t i = 0 ; i < size ; i++)
    {
        *(volatile uint8_t*)(ptr + i) = value;
    }
}

void volatile_memcpy(volatile void* const dest, volatile void* const src, const size_t size)
{
    for (size_t i = 0 ; i < size ; i++)
    {
        *(volatile uint8_t*)(dest + i) = *(volatile uint8_t*)(src + i);
    }
}
