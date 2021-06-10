#ifndef IO_STUB_PINMAPPING_HEADER
#define IO_STUB_PINMAPPING_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <avr/io.h>

// Fake fans
#define PIN_FAN_RIGHT_PORT_REG      PORTB
#define PIN_FAN_RIGHT_DDR_REG       DDRB
#define PIN_FAN_RIGHT_PIN_NUMBER    PINB0
#define PIN_FAN_RIGHT_PIN           PINB

#define PIN_FAN_LEFT_PORT_REG      PORTB
#define PIN_FAN_LEFT_DDR_REG       DDRB
#define PIN_FAN_LEFT_PIN_NUMBER    PINB1
#define PIN_FAN_LEFT_PIN           PINB

// Fake switch
#define PIN_POWER_SWITCH_PORT_REG      PORTC
#define PIN_POWER_SWITCH_DDR_REG       DDRC
#define PIN_POWER_SWITCH_PIN_NUMBER    PINC3
#define PIN_POWER_SWITCH_PIN           PINC

// Fake mosfets high and low sides
#define PIN_MOSFET_HIGH_SIDE_PORT_REG      PORTD
#define PIN_MOSFET_HIGH_SIDE_DDR_REG       DDRD
#define PIN_MOSFET_HIGH_SIDE_PIN_NUMBER    PIND1
#define PIN_MOSFET_HIGH_SIDE_PIN           PIND

#define PIN_MOSFET_LOW_SIDE_PORT_REG      PORTD
#define PIN_MOSFET_LOW_SIDE_DDR_REG       DDRD
#define PIN_MOSFET_LOW_SIDE_PIN_NUMBER    PIND5
#define PIN_MOSFET_LOW_SIDE_PIN           PIND

#ifdef __cplusplus
}
#endif

#endif /* IO_STUB_PINMAPPING_HEADER */