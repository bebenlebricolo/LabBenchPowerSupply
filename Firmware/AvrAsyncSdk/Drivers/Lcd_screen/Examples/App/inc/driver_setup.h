#ifndef DRIVER_SETUP_HEADER
#define DRIVER_SETUP_HEADER

typedef enum
{
    DRIVER_SETUP_ERROR_OK,
    DRIVER_SETUP_ERROR_INIT_FAILED,
} driver_setup_error_t;

driver_setup_error_t driver_init_timer_2(void);
driver_setup_error_t driver_init_i2c(void);
driver_setup_error_t driver_init_lcd(void);

#endif /* DRIVER_SETUP_HEADER */