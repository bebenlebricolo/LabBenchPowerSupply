#ifndef MEMUTILS_HEADER
#define MEMUTILS_HEADER

#include <stdint.h>
#include <stddef.h>


/**
 * @brief provides a volatile-ready memset kind-of interface
 * Sets all blocks of targeted memory to the given value, when memory is marked as volatile
 * @param[in/out]   ptr     :   memory block to be modified (volatile)
 * @param[in]       value   :   sets
 * @param[in]       size    :   sizeof the block in bytes
*/
void volatile_memset(volatile void * const ptr, const uint8_t value, const size_t size);

void volatile_memcpy(volatile void* const dest, volatile void* const src, const size_t size);

#endif /* MEMUTILS_HEADER */