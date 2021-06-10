#include <avr/io.h>

#include "io.h"
#include "config.h"
#include "pinmapping.h"

static stubbed_registers_t stubbed_registers = {0};

stubbed_registers_t* get_stubbed_registers(void )
{
    return &stubbed_registers;
}

io_t io_pins_lut[IO_MAX_PINS] =
{
    // Right fan pin configuration
    [PIN_FAN_RIGHT] = {
        .pin = PIN_FAN_RIGHT_PIN_NUMBER,
        .direction = IO_OUT_PUSH_PULL,
        .port = IO_PORT_B,
        .state = IO_STATE_LOW
    },

    // Left fan pin configuration
    [PIN_FAN_LEFT] = {
        .pin = PIN_FAN_LEFT_PIN_NUMBER,
        .direction = IO_OUT_PUSH_PULL,
        .port = IO_PORT_B,
        .state = IO_STATE_HIGH
    },

    // Power switch pin configuration
    [PIN_POWER_SWITCH] = {
        .pin = PIN_POWER_SWITCH_PIN_NUMBER,
        .direction = IO_IN_TRISTATE,
        .port = IO_PORT_C
    },

    // High side mosfet pin configuration
    [PIN_MOSFET_HIGH_SIDE] = {
        .pin = PIN_MOSFET_HIGH_SIDE_PIN_NUMBER,
        .direction = IO_OUT_PUSH_PULL,
        .port = IO_PORT_D,
        .state = IO_STATE_LOW
    },

    // Low side mosfet pin configuration
    [PIN_MOSFET_LOW_SIDE] = {
        .pin = PIN_MOSFET_LOW_SIDE_PIN_NUMBER,
        .direction = IO_OUT_PUSH_PULL,
        .port = IO_PORT_D,
        .state = IO_STATE_HIGH
    }
};


io_config_t io_config =
{
    .mcucr_reg = &stubbed_registers.st_mcucr,
    .porta_cfg =
    {
        .port_reg = &stubbed_registers.st_porta.st_port_reg,
        .pin_reg = &stubbed_registers.st_porta.st_pin_reg,
        .ddr_reg = &stubbed_registers.st_porta.st_ddr_reg
    },
    .portb_cfg =
    {
        .port_reg = &stubbed_registers.st_portb.st_port_reg,
        .pin_reg = &stubbed_registers.st_portb.st_pin_reg,
        .ddr_reg = &stubbed_registers.st_portb.st_ddr_reg
    },
    .portc_cfg =
    {
        .port_reg = &stubbed_registers.st_portc.st_port_reg,
        .pin_reg = &stubbed_registers.st_portc.st_pin_reg,
        .ddr_reg = &stubbed_registers.st_portc.st_ddr_reg
    },
    .portd_cfg =
    {
        .port_reg = &stubbed_registers.st_portd.st_port_reg,
        .pin_reg = &stubbed_registers.st_portd.st_pin_reg,
        .ddr_reg = &stubbed_registers.st_portd.st_ddr_reg
    },
};
