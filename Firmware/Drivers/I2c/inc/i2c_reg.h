#ifndef I2C_REG_HEADER
#define I2C_REG_HEADER

#include <stdint.h>

/* TWCR register bit mapping */
#define TWIE_BIT  0
#define TWEN_BIT  2
#define TWWC_BIT  3
#define TWSTO_BIT 4
#define TWSTA_BIT 5
#define TWEA_BIT  6
#define TWINT_BIT 7

/* TWCR bit masks */
#define TWIE_MSK  0x01
#define TWEN_MSK  0x04
#define TWWC_MSK  0x08
#define TWSTO_MSK 0x10
#define TWSTA_MSK 0x20
#define TWEA_MSK  0x40
#define TWINT_MSK 0x80

/* TWSR Status register bit mapping */
#define TWPS0_BIT 0
#define TWPS1_BIT 1
#define TWS3_BIT  3
#define TWS4_BIT  4
#define TWS5_BIT  5
#define TWS6_BIT  6
#define TWS7_BIT  7

/* TWSR Status register bit masks */
#define TWPS_MSK 0x03
#define TWS_MSK  0xF8

/* TWAR register bit mapping */
#define TWGCE_BIT   0
#define TWA1_BIT    1
#define TWA2_BIT    2
#define TWA3_BIT    3
#define TWA4_BIT    4
#define TWA5_BIT    5
#define TWA6_BIT    6
#define TWA7_BIT    7

/* TWAR register bit masks */
#define TWGCE_MSK   0x01
#define TWA_MSK     0xFE

/* TWAMR register bit mapping */
#define TWAMR0_BIT  1
#define TWAMR1_BIT  2
#define TWAMR2_BIT  3
#define TWAMR3_BIT  4
#define TWAMR5_BIT  5
#define TWAMR6_BIT  6
#define TWAMR7_BIT  7

/* TWAMR bit mask */
#define TWAMR_MSK 0xFE

typedef enum
{
    MAS_TX_START_TRANSMITTED        = 0x08,
    MAS_TX_REPEATED_START           = 0x10,
    MAS_TX_SLAVE_WRITE_ACK          = 0x18,
    MAS_TX_SLAVE_WRITE_NACK         = 0x20,
    MAS_TX_DATA_TRANSMITTED_ACK     = 0x28,
    MAS_TX_DATA_TRANSMITTED_NACK    = 0x30,
    MAS_TX_ARBITRATION_LOST         = 0x38
} i2c_master_transmitter_mode_status_codes_t;

typedef enum
{
    MAS_RX_START_TRANSMITTED      = 0x08,
    MAS_RX_REPEATED_START         = 0x10,
    MAS_RX_ARBITRATION_LOST_NACK  = 0x38,
    MAS_RX_SLAVE_READ_ACK         = 0x40,
    MAS_RX_SLAVE_READ_NACK        = 0x48,
    MAS_RX_DATA_RECEIVED_ACK      = 0x50,
    MAS_RX_DATA_RECEIVED_NACK     = 0x58
} i2c_master_receiver_mode_status_codes_t;

typedef enum
{
    SLA_RX_SLAVE_WRITE_ACK                              = 0x60,
    SLA_RX_ARBITRATION_LOST_OWN_ADDR_RECEIVED_ACK       = 0x68,
    SLA_RX_GENERAL_CALL_RECEIVED_ACK                    = 0x70,
    SLA_RX_ARBITRATION_LOST_GENERAL_CALL_RECEIVED_ACK   = 0x78,
    SLA_RX_PREV_ADDRESSED_DATA_RECEIVED_ACK             = 0x80,
    SLA_RX_PREV_ADDRESSED_DATA_LOST_NACK                = 0x88,
    SLA_RX_GENERAL_CALL_ADDRESSED_DATA_RECEIVED_ACK     = 0x90,
    SLA_RX_GENERAL_CALL_ADDRESSED_DATA_LOST_NACK        = 0x98,
    SLA_RX_START_STOP_COND_RECEIVED_WHILE_OPERATING     = 0xA0,
} i2c_slave_receiver_mode_status_codes_t;

typedef enum
{
    SLA_TX_OWN_ADDR_SLAVE_READ_ACK                      = 0xA8,
    SLA_TX_ARBITRATION_LOST_OWN_ADDR_RECEIVED_ACK       = 0xB0,
    SLA_TX_DATA_TRANSMITTED_ACK                         = 0xB8,
    SLA_TX_DATA_TRANSMITTED_NACK                        = 0xC0,
    SLA_TX_LAST_DATA_TRANSMITTED_ACK                    = 0xC8,
} i2c_slave_transmitter_mode_status_codes_t;

typedef enum
{
    I2C_MISC_NO_RELEVANT_STATE              = 0xF8,
    I2C_MISC_BUS_ERROR_ILLEGAL_START_STOP   = 0x00,
} i2c_miscellaneous_states_t;

/**
 * @brief packs all necesseray register pointers to interact with TWI peripheral
*/
typedef struct
{
    volatile uint8_t * _TWCR;  /**< Two Wire Interface Control register              */
    volatile uint8_t * _TWBR;  /**< Two Wire Interface Bit Rate register             */
    volatile uint8_t * _TWSR;  /**< Two Wire Interface Status register               */
    volatile uint8_t * _TWDR;  /**< Two Wire Interface Data (in/out) register        */
    volatile uint8_t * _TWAR;  /**< Two Wire Interface Slave Address register        */
    volatile uint8_t * _TWAMR; /**< Two Wire Interface Slave Address Mask register   */
} i2c_handle_t;


#endif /* I2C_REG_HEADER */