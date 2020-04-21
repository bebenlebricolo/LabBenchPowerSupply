#ifndef DRIVER_SETUP_HEADER
#define DRIVER_SETUP_HEADER

typedef enum
{
    DRIVER_SETUP_ERROR_OK,
    DRIVER_SETUP_ERROR_INIT_FAILED,
} driver_setup_error_t;

driver_setup_error_t init_adc(void);
driver_setup_error_t init_timer_0(void);
driver_setup_error_t init_timer_1(void);
driver_setup_error_t init_timer_2(void);

#endif /* DRIVER_SETUP_HEADER */