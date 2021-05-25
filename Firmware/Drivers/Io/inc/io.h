#ifndef IOMANAGER_HEADER
#define IOMANAGER_HEADER

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef IO_MAX_PINS
#define IO_MAX_PINS (28U)
#pragma warning("IO_MAX_PINS macro was not defined in this compilation unit, IO_MAX_PINS will take its default size.")
#endif

/**
 * @brief describes a simplified interface for any
 * given pin to be configured (using enums instead of plain registers)
*/
typedef enum
{
    IO_PORT_A = 0U,  /**< PORT A of avr core */
    IO_PORT_B = 1U,  /**< PORT B of avr core */
    IO_PORT_C = 2U,  /**< PORT C of avr core */
    IO_PORT_D = 3U,   /**< PORT D of avr core */
    IO_PORT_COUNT = 4U
} io_port_t;

typedef enum
{
    IO_STATE_LOW = 0U,
    IO_STATE_HIGH = 1U
} io_state_t;

/**
 * @brief gives the direction and settings for any given pin
*/
typedef enum
{
    IO_IN_TRISTATE,     /**< Pin is configured as input, high impedance. This is the default pin state  */
    IO_IN_PULL_UP,      /**< Pin is configured as input, with internal pull up resistor                 */
    IO_OUT_PUSH_PULL    /**< Pin is configured as output, with internal pull down resistor               */
} io_direction_t;

/**
 * @brief Describes an io pin abstraction to be used when configuring a new pin
*/
typedef struct
{
    io_state_t      state;      /**< Default pin state, (state == 1 sets it high).          */
                                /**< Only used when direction is set to output pin          */
    uint8_t         pin;        /**< Pin number, ranging from 0 to 7                        */
    io_port_t       port;       /**< Pin port name. Could be PortA, PortB, PortC or PortD   */
    io_direction_t  direction;
} io_t;

/**
 * @brief This structure gives the mandatory configuration for any ports of AVR cores
*/
typedef struct
{
    volatile uint8_t * port_reg; /**< PORTx register (e.g. using avr/io.h -> PORTA macro)    */
    volatile uint8_t * pin_reg;  /**< PINx register (e.g. using avr/io.h -> PINA macro)      */
    volatile uint8_t * ddr_reg;  /**< DDRx register (e.g. using avr/io.h -> DDRA macro)      */
} io_port_config_t;

/**
 * @brief This structure needs to be implemented only once by application code in order to route
 * ports addresses to the right locations.
 * Under normal circumstances, any further reference to those addresses by io's will be optimized as
 * as direct access instead of pointer dereferencing.
*/
typedef struct
{
    volatile uint8_t *  mcucr_reg;  /**< MCUCR register address, used to enable the Pull Up bit (PUD)   */
    io_port_config_t    porta_cfg;  /**< Port A configuration                                           */
    io_port_config_t    portb_cfg;  /**< Port B configuration                                           */
    io_port_config_t    portc_cfg;  /**< Port C configuration                                           */
    io_port_config_t    portd_cfg;  /**< Port D configuration                                           */
} io_config_t;

/**
 * @brief the following extern symbol is a lookup table with pins configuration.
 * Its size is determined at compile time by the IO_MAX_PINS macro which should be defined in the
 * config.h file.
*/
extern io_t io_pins_lut[IO_MAX_PINS];

/**
 * @brief the following configuration has to be provided by
 * application code (usually in a config.c file) to be able to wire
 * ports addresses once and for all by the compiler.
 * Further calls to those addresses (e.g. while reading or writing to a pin)
 * will normally by optimized by the compiler as direct calls afterwards (no pointer dereferencing then)
*/
extern io_config_t io_config;

/**
 * @brief reads data on a given io, using its index as an input (referencing an
 * io from the lookup up configuration table)
 * @param[in] index : index of the pin in the lookup table
 * @return
 *      true    : pin is set (high logic level)
 *      false   : pin is not set (low logic level)
*/
io_state_t io_read(const uint8_t index);

/**
 * @brief writes data to a single pin, using its index (from the lookup table)
 * Note that this function will not check if the pin is set as an output,
 * in case where the pin is set as an input and that you write to it, you may change the pin configuration !
 * @param[in] index : index of the pin in the lookup table
 * @param[in] state : new state of the pin
*/
void io_write(const uint8_t index, const io_state_t state);

/**
 * @brief configures all registered pins and give them default state, if any
*/
void io_init(void);

#ifdef __cplusplus
}
#endif

#endif /* IOMANAGER_HEADER */