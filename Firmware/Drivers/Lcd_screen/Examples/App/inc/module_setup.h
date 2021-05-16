#ifndef MODULE_SETUP_HEADER
#define MODULE_SETUP_HEADER

typedef enum
{
    MODULE_SETUP_ERROR_OK,
    MODULE_SETUP_ERROR_INIT_FAILED,
} module_setup_error_t;

module_setup_error_t module_init_timebase(void);

#endif /* MODULES_SETUP_HEADER */