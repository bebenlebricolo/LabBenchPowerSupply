#ifndef PIN_MAPPING_HEADER
#define PIN_MAPPING_HEADER

#include <avr/io.h>

/* Timer 0 output compare pins */
#define OC0A_PIN_NUMBER PORTD6
#define OC0A_PORT_REG   PORTD
#define OC0A_DDR_REG    DDRD
#define OCOA_PIN_REG    PIND

#define OC0B_PIN_NUMBER PORTD5
#define OC0B_PORT_REG   PORTD
#define OC0B_DDR_REG    DDRD
#define OCOB_PIN_REG    PIND

/* Timer 1 output compare pins */
#define OC1A_PIN_NUMBER PORTB1
#define OC1A_PORT_REG   PORTB
#define OC1A_DDR_REG    DDRB
#define OC1A_PIN_REG    PINB

#define OC1B_PIN_NUMBER PORTB2
#define OC1B_PORT_REG   PORTB
#define OC1B_DDR_REG    DDRB
#define OC1B_PIN_REG    PINB

/* Timer 2 output compare pins */
#define OC2A_PIN_NUMBER PORTB3
#define OC2A_PORT_REG   PORTB
#define OC2A_DDR_REG    DDRB
#define OC2A_PIN_REG    PINB

#define OC2B_PIN_NUMBER PORTD3
#define OC2B_PORT_REG   PORTD
#define OC2B_DDR_REG    DDRD
#define OC2B_PIN_REG    PIND

// User interface buttons
// Sequencing step button us wirted to Arduino Nano's pin D1 (which is PORTD1 of Atmega328P)
#define I2C_RUN_STEP_BUTTON_PIN_NUMBER  PORTD2
#define I2C_RUN_STEP_BUTTON_PORT_REG    PORTD
#define I2C_RUN_STEP_BUTTON_DDR_REG     DDRD
#define I2C_RUN_STEP_BUTTON_PIN_REG     PIND

// Reset button network is wired to Arduino Nano's Pin D3 (which is PORTD3 of Atmega328P)
#define I2C_SEQ_RESET_BUTTON_PIN_NUMBER  PORTD3
#define I2C_SEQ_RESET_BUTTON_PORT_REG    PORTD
#define I2C_SEQ_RESET_BUTTON_DDR_REG     DDRD
#define I2C_SEQ_RESET_BUTTON_PIN_REG     PIND



#endif /* PIN_MAPPING_HEADER */