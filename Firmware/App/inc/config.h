#ifndef CONFIG_HEADER
#define CONFIG_HEADER

/* Defines how many instances of timer/counters will be created */
#define TIMER_8_BIT_COUNT       1
#define TIMER_8_BIT_ASYNC_COUNT 1
#define TIMER_16_BIT_COUNT      1

#define TIMEBASE_MAX_MODULES 3U
#define I2C_DEVICES_COUNT 1U

// Only implement master tx driver
//#define I2C_IMPLEM_MASTER_TX
//#define I2C_IMPLEM_FULL_DRIVER

#endif /* CONFIG_HEADER */