#include "io.h"

#define PUD_MSK (0x10)

static io_port_config_t * port_lut[IO_PORT_COUNT] =
{
    &io_config.porta_cfg,
    &io_config.portb_cfg,
    &io_config.portc_cfg,
    &io_config.portd_cfg
};

static inline void configure_single_pin(io_port_config_t * config, io_t * io)
{
    if(io->direction == IO_OUT_PUSH_PULL)
    {
        *config->ddr_reg |= (1 << io->pin);
        if (io->state == IO_STATE_LOW)
        {
            *config->port_reg &= ~(1 << io->pin);
        }
        else
        {
            *config->port_reg |= (1 << io->pin);
        }
    }
    else
    {
        *config->ddr_reg &= ~(1 << io->pin);
        if (io->direction == IO_IN_PULL_UP)
        {
            *config->port_reg |= (1 << io->pin);
        }
        else
        {
            *config->port_reg &= ~(1 << io->pin);
        }
    }
}

void io_init(void)
{
    bool needs_pull_up = false;
    for (uint8_t i = 0 ; i < IO_MAX_PINS ; i++)
    {
        io_t * io = &io_pins_lut[i];
        configure_single_pin(port_lut[io->port], io);
        needs_pull_up |= (io->direction == IO_IN_PULL_UP);
    }

    // Only enables pull ups if requested
    if (true == needs_pull_up)
    {
        *io_config.mcucr_reg |= PUD_MSK;
    }
}

io_state_t io_read(const uint8_t index)
{
    io_t * io = &io_pins_lut[index];
    io_state_t state = (io_state_t) ((*port_lut[io->port]->pin_reg & (1 << io->pin)) >> io->pin);
    return state;
}

void io_write(const uint8_t index, const io_state_t state)
{
    io_t * io = &io_pins_lut[index];
    if ( IO_STATE_HIGH == state )
    {
        *port_lut[io->port]->port_reg |= (1 << io->pin);
    }
    else
    {
        *port_lut[io->port]->port_reg &= ~(1 << io->pin);
    }
}

