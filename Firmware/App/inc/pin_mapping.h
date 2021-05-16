#ifndef PIN_MAPPING_HEADER
#define PIN_MAPPING_HEADER

#include <avr/io.h>

// ####################################################################################
// ######################### Timers output compare pins  ##############################
// ####################################################################################

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

// ####################################################################################
// ######################### Buttons user interface ###################################
// ####################################################################################

/* Buttons interface mapping */
#define BUTTON_PLUS_PIN_NUMBER PORTD2
#define BUTTON_PLUS_PORT_REG   PORTD
#define BUTTON_PLUS_DDR_REG    DDRD
#define BUTTON_PLUS_PIN_REG    PIND

#define BUTTON_MINUS_PIN_NUMBER PORTD3
#define BUTTON_MINUS_PORT_REG   PORTD
#define BUTTON_MINUS_DDR_REG    DDRD
#define BUTTON_MINUS_PIN_REG    PIND

#define BUTTON_VARSELECT_PIN_NUMBER PORTD4
#define BUTTON_VARSELECT_PORT_REG   PORTD
#define BUTTON_VARSELECT_DDR_REG    DDRD
#define BUTTON_VARSELECT_PIN_REG    PIND

#define BUTTON_MODE_PIN_NUMBER PORTD5
#define BUTTON_MODE_PORT_REG   PORTD
#define BUTTON_MODE_DDR_REG    DDRD
#define BUTTON_MODE_PIN_REG    PIND

// ####################################################################################
// ######################### Power mosfet pins ########################################
// ####################################################################################

/* Mosfet controls */
#define MOSFET_HIGH_SIDE_PIN_NUMBER PORTB1   // Wired to OC1A output
#define MOSFET_HIGH_SIDE_PORT_REG   PORTB
#define MOSFET_HIGH_SIDE_DDR_REG    DDRB
#define MOSFET_HIGH_SIDE_PIN_REG    PINB

#define MOSFET_LOW_SIDE_PIN_NUMBER PORTB2   // Wired to OC1B output
#define MOSFET_LOW_SIDE_PORT_REG   PORTB
#define MOSFET_LOW_SIDE_DDR_REG    DDRB
#define MOSFET_LOW_SIDE_PIN_REG    PINB

// ####################################################################################
// ######################### Fan control network ######################################
// ####################################################################################

/* Fan control network */
#define FAN_CONTROL_PIN_NUMBER PORTD6   // which is wired to OC0A output
#define FAN_CONTROL_PORT_REG   PORTD
#define FAN_CONTROL_DDR_REG    DDRD
#define FAN_CONTROL_PIN_REG    PIND

// ####################################################################################
// ############################# Analog readings ######################################
// ####################################################################################

/* Temperature probe, analogic reading */
#define TEMP_PROBE_PIN_NUMBER PORTC3
#define TEMP_PROBE_PORT_REG   PORTC
#define TEMP_PROBE_DDR_REG    DDRC
#define TEMP_PROBE_PIN_REG    PINC
#define TEMP_PROBE_ADC        ADC3

/* Voltage analogic readings */
#define SECONDARY_SMOOTHED_PIN_NUMBER PORTC2
#define SECONDARY_SMOOTHED_PORT_REG   PORTC
#define SECONDARY_SMOOTHED_DDR_REG    DDRC
#define SECONDARY_SMOOTHED_PIN_REG    PINC
#define SECONDARY_SMOOTHED_ADC        ADC2

/* Voltage analogic readings */
#define OUTVOLTAGE_SENSING_PIN_NUMBER PORTC1
#define OUTVOLTAGE_SENSING_PORT_REG   PORTC
#define OUTVOLTAGE_SENSING_DDR_REG    DDRC
#define OUTVOLTAGE_SENSING_PIN_REG    PINC
#define OUTVOLTAGE_SENSING_ADC        ADC1

/* Voltage analogic readings */
#define CURRENT_SENSING_PIN_NUMBER PORTC0
#define CURRENT_SENSING_PORT_REG   PORTC
#define CURRENT_SENSING_DDR_REG    DDRC
#define CURRENT_SENSING_PIN_REG    PINC
#define CURRENT_SENSING_ADC        ADC0

#endif /* PIN_MAPPING_HEADER */