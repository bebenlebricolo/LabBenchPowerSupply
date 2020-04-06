#ifndef GENERIC_PERIPHERAL_HEADER
#define GENERIC_PERIPHERAL_HEADER
#include <stdint.h>

typedef volatile uint16_t peripheral_addr_t;
typedef volatile uint8_t  peripheral_reg_t;
//typedef (volatile uint8_t *) PERIPHERAL_ADDR;

typedef enum
{
    PERIPHERAL_READY = 0U,
    PERIPHERAL_ALREADY_INITIALISED ,
    PERIPHERAL_NOT_INITIALISED ,
    PERIPHERAL_WRONG_CONFIG
} peripheral_error_t;

#endif /* GENERIC_PERIPHERAL_HEADER */