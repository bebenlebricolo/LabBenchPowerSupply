#ifndef IO_CONFIG_HEADER
#define IO_CONFIG_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

// Tells to IO driver what is the maximum available pin count for this application
// This is usually provided by the application layer
#define IO_MAX_PINS (5U)

/**
 * @brief fake pin naming using indexes
 * This will be used in the config.c when pins will be addressed so that we can
 * reuse that scheme within application code as well
*/
typedef enum
{
    PIN_FAN_RIGHT        = 0,
    PIN_FAN_LEFT         = 1,
    PIN_POWER_SWITCH     = 2,
    PIN_MOSFET_HIGH_SIDE = 3,
    PIN_MOSFET_LOW_SIDE  = 4
} pins_t;

typedef struct
{
    volatile uint8_t st_port_reg;
    volatile uint8_t st_pin_reg;
    volatile uint8_t st_ddr_reg;
} stubbed_port_t;

typedef struct
{
    volatile uint8_t st_mcucr;
    stubbed_port_t st_porta;
    stubbed_port_t st_portb;
    stubbed_port_t st_portc;
    stubbed_port_t st_portd;
} stubbed_registers_t;

stubbed_registers_t* get_stubbed_registers(void);

#ifdef __cplusplus
}
#endif

#endif /* IO_CONFIG_HEADER */