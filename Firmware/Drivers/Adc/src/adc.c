#include "adc.h"
#include "adc_reg.h"
#include "adc_stack.h"

#include <string.h>
#include <avr/io.h>

/* 10 bits adc, full range */
#define ADC_MAX_VALUE       1024U
#define ADC_1V1_MILLIVOLT   1100U


/* Holds the current configuration of the ADC module */
static struct
{
    adc_config_hal_t base_config;
    bool is_initialised;
} internal_configuration = {.base_config = {0},
                            .is_initialised = false};

static volatile adc_stack_t registered_channels;

static inline uint16_t retrieve_result_from_registers(void);
static inline void isr_helper_extract_data_from_adc_regs(void);

static inline uint16_t retrieve_result_from_registers(void)
{
    uint8_t low = *internal_configuration.base_config.handle.readings.adclow_reg;
    uint8_t high = *internal_configuration.base_config.handle.readings.adchigh_reg;
    return (uint16_t)(low | (high << 8U) );
}

static inline peripheral_error_t set_mux_register(volatile adc_channel_pair_t * const pair)
{
    peripheral_error_t ret = PERIPHERAL_ERROR_OK;
    if (NULL == pair)
    {
        ret = PERIPHERAL_ERROR_NULL_POINTER;
    }
    else
    {
        *internal_configuration.base_config.handle.mux_reg =  (*internal_configuration.base_config.handle.mux_reg & ~MUX_MSK) | pair->channel;
    }
    return ret;
}

peripheral_error_t adc_config_hal_copy(adc_config_hal_t * dest, adc_config_hal_t * const src)
{
    peripheral_error_t ret = PERIPHERAL_ERROR_OK;
    if (NULL == dest || NULL == src)
    {
        ret = PERIPHERAL_ERROR_NULL_POINTER;
    }
    else
    {
        memcpy(dest,src,sizeof(adc_config_hal_t));
    }
    return ret;
}

peripheral_error_t adc_config_hal_reset(adc_config_hal_t * config)
{
    peripheral_error_t ret = PERIPHERAL_ERROR_OK;
    if (NULL == config)
    {
        ret = PERIPHERAL_ERROR_NULL_POINTER;
    }
    else
    {
        adc_config_hal_get_default(config);
        adc_handle_reset(&config->handle);
    }
    return ret;
}

peripheral_error_t adc_config_hal_get_default(adc_config_hal_t * config)
{
    peripheral_error_t ret = PERIPHERAL_ERROR_OK;
    if (NULL == config)
    {
        ret = PERIPHERAL_ERROR_NULL_POINTER;
    }
    else
    {
        config->supply_voltage_mv = 0;
        config->prescaler = ADC_PRESCALER_DEFAULT_2;
        config->ref = ADC_VOLTAGE_REF_INTERNAL_1V1;
        config->alignment = ADC_RIGT_ALIGNED_RESULT;
        config->trigger_sources = ADC_TRIGGER_FREE_RUNNING;
        config->running_mode = ADC_RUNNING_MODE_SINGLE_SHOT;
        config->using_interrupt = false;
        adc_handle_get_default(&config->handle);
    }
    return ret;
}


peripheral_error_t adc_handle_copy(adc_handle_t * const dest, const adc_handle_t * const src)
{
    peripheral_error_t ret = PERIPHERAL_ERROR_OK;
    if (NULL == dest || NULL == src)
    {
        ret = PERIPHERAL_ERROR_NULL_POINTER;
    }
    else
    {
        memcpy(dest,src,sizeof(adc_handle_t));
    }
    return ret;
}

peripheral_error_t adc_handle_reset(adc_handle_t * const handle)
{
    peripheral_error_t ret = PERIPHERAL_ERROR_OK;
    if (NULL == handle)
    {
        ret = PERIPHERAL_ERROR_NULL_POINTER;
    }
    else
    {
        handle->mux_reg = NULL;
        handle->adcsra_reg = NULL;
        handle->adcsrb_reg = NULL;
        handle->readings.adchigh_reg = NULL ;
        handle->readings.adclow_reg = NULL ;
    }
    return ret;
}

peripheral_error_t adc_handle_get_default(adc_handle_t * const handle)
{
    peripheral_error_t ret = PERIPHERAL_ERROR_OK;
    if (NULL == handle)
    {
        ret = PERIPHERAL_ERROR_NULL_POINTER;
    }
    else
    {
        handle->mux_reg = &ADMUX;
        handle->adcsra_reg = &ADCSRA;
        handle->adcsrb_reg = &ADCSRB;
        handle->readings.adchigh_reg = &ADCH ;
        handle->readings.adclow_reg = &ADCL ;
    }
    return ret;
}


peripheral_error_t adc_base_init(adc_config_hal_t * const config)
{
    peripheral_error_t ret = PERIPHERAL_ERROR_OK;
    if (NULL == config)
    {
        ret = PERIPHERAL_ERROR_NULL_POINTER;
    }
    else
    {
        adc_stack_reset(&registered_channels);
        /* First, copy configuration data to the internal cache */
        adc_config_hal_copy(&(internal_configuration.base_config), config);
        adc_handle_t * handle = &internal_configuration.base_config.handle;
        *handle->mux_reg = (*handle->mux_reg & ~REF_MSK) | (config->ref << REFS0);            /* set reference voltage */
        *handle->mux_reg = (*handle->mux_reg & ~ADLAR_MSK) | (config->alignment << ADLAR);    /* set result adjustment */
        *handle->adcsra_reg = (*handle->adcsra_reg & ~ADPS_MSK) | (config->prescaler);        /* set precaler */
        *handle->adcsrb_reg |= config->trigger_sources;
        if (internal_configuration.base_config.using_interrupt)
        {
            *handle->adcsra_reg |= (1 << ADIE);
        }
        if (ADC_RUNNING_MODE_SINGLE_SHOT == config->running_mode)
        {
            *handle->adcsra_reg &= ~(1 << ADATE);
        }
        else
        {
            *handle->adcsra_reg |= 1 << ADATE;
        }

        internal_configuration.is_initialised = true;
    }
    return ret;
}

void adc_base_deinit(void)
{
    internal_configuration.is_initialised = false;
    adc_stack_reset(&registered_channels);
    {
        adc_handle_t * handle = &internal_configuration.base_config.handle;
        if (handle->mux_reg != NULL)
        {
            /* reset configured channels */
            *(handle->mux_reg) = 0;
            *(handle->adcsra_reg) = 0;
            *(handle->adcsrb_reg) = 0;
            *(handle->readings.adchigh_reg) = 0;
            *(handle->readings.adclow_reg) = 0;
        }
    }
    adc_config_hal_reset(&internal_configuration.base_config);
}


static inline peripheral_state_t check_initialisation(void)
{
    return (internal_configuration.is_initialised) ? PERIPHERAL_STATE_READY : PERIPHERAL_STATE_NOT_INITIALISED;
}

peripheral_state_t adc_start(void)
{
    peripheral_state_t init_state = check_initialisation();
    if (PERIPHERAL_STATE_READY == init_state)
    {
        peripheral_reg_t * reg = internal_configuration.base_config.handle.adcsra_reg;
        /* Enable and start the ADC peripheral */
        *reg |= (1 << ADEN) | (1 << ADSC);
    }

    return init_state;
}

peripheral_state_t adc_stop(void)
{
    peripheral_state_t init_state = check_initialisation();
    if (PERIPHERAL_STATE_READY == init_state)
    {
        peripheral_reg_t * reg = internal_configuration.base_config.handle.adcsra_reg;
        /* Disable the ADC peripheral */
        *reg &= ~((1 << ADEN) | (1 << ADSC));
        /* Disable ADC interrupt mode */
        *reg &= ~(1 << ADIE);
    }

    return init_state;
}

peripheral_error_t adc_register_channel(const adc_mux_t channel)
{
    peripheral_error_t ret = PERIPHERAL_ERROR_OK;
    ret = adc_stack_register_channel(&registered_channels, channel);
    return ret;
}

peripheral_error_t adc_unregister_channel(const adc_mux_t channel)
{
    peripheral_error_t ret = PERIPHERAL_ERROR_OK;
    ret = adc_stack_unregister_channel(&registered_channels, channel);
    return ret;
}

peripheral_error_t adc_read_raw(const adc_mux_t channel, adc_result_t * const result)
{
    peripheral_error_t ret = PERIPHERAL_ERROR_OK;
    if (NULL == result)
    {
        ret = PERIPHERAL_ERROR_NULL_POINTER;
    }
    else
    {
        volatile adc_channel_pair_t * pair = NULL;
        adc_stack_error_t find_error = adc_stack_find_channel(&registered_channels, channel, &pair);
        if (ADC_STACK_ERROR_OK == find_error)
        {
            *result = pair->result;
        }
        else
        {
            ret = PERIPHERAL_ERROR_FAILED;
        }
    }
    return ret;
}

static inline bool conversion_is_finished(void)
{
    return ((*internal_configuration.base_config.handle.adcsra_reg) & 1 << ADSC) == 0;
}

static inline void isr_helper_extract_data_from_adc_regs(void)
{
    static volatile adc_channel_pair_t * pair = NULL;
    adc_stack_error_t stack_error = ADC_STACK_ERROR_OK;
    if (NULL == pair)
    {
        stack_error = adc_stack_get_current(&registered_channels, &pair);
    }
    if (ADC_STACK_ERROR_OK == stack_error && conversion_is_finished())
    {
        uint16_t result = retrieve_result_from_registers();
        pair->result = result;

        #ifdef UNIT_TESTING
            /* Reset interrupt flag manually */
            *internal_configuration.base_config.handle.adcsra_reg &= ~ADIF_MSK;
        #else
            /* Reset interrupt flag manually */
            //*internal_configuration.base_config.handle.adcsra_reg |= (1U << ADIF);
        #endif
        stack_error = adc_stack_get_next(&registered_channels, &pair);
        if (ADC_STACK_ERROR_OK == stack_error)
        {
            set_mux_register(pair);
        }
    }
}

peripheral_state_t adc_process(void)
{
    peripheral_state_t ret = check_initialisation();
    if (PERIPHERAL_STATE_READY == ret)
    {
       isr_helper_extract_data_from_adc_regs();
        /* Start next conversion */
        *internal_configuration.base_config.handle.adcsra_reg |= 1U << ADSC ;
    }
    return ret;
}

peripheral_error_t adc_read_millivolt(const adc_mux_t channel, adc_millivolts_t * const reading)
{
    peripheral_error_t ret = PERIPHERAL_ERROR_OK;
    if (NULL == reading )
    {
        ret = PERIPHERAL_ERROR_NULL_POINTER;
    }
    else
    {
        adc_result_t result = 0;
        ret = adc_read_raw(channel, &result);
        if (PERIPHERAL_ERROR_OK == ret)
        {
            switch (internal_configuration.base_config.ref)
            {
                case ADC_VOLTAGE_REF_INTERNAL_1V1:
                    *reading =  (uint16_t)(((uint32_t)result * (uint32_t)ADC_1V1_MILLIVOLT) / ADC_MAX_VALUE);
                    break;
                case ADC_VOLTAGE_REF_AREF_PIN:
                case ADC_VOLTAGE_REF_AVCC:
                    *reading = (uint16_t)(((uint32_t)result * (uint32_t)internal_configuration.base_config.supply_voltage_mv) / ADC_MAX_VALUE);
                    break;
                default:
                    *reading = 0;
                    ret = PERIPHERAL_ERROR_FAILED;
                    break;
            }
        }

    }

    return ret;
}


#ifdef UNIT_TESTING
void adc_isr_handler(void)
{
    if (internal_configuration.is_initialised)
    {
        /* if using interrupt AND interrupt enable bit is ON
         AND Start Conversion bit is SET
         AND Interrupt Flag is SET
         -> read values from ADC registers and store them in local buffer
         */
        if((internal_configuration.base_config.using_interrupt == true)
        && (0 != (*internal_configuration.base_config.handle.adcsra_reg & ADIE_MSK))
        && (0 == (*internal_configuration.base_config.handle.adcsra_reg & ADSC_MSK))
        && (0 != (*internal_configuration.base_config.handle.adcsra_reg & ADIF_MSK))
        )
        {
            isr_helper_extract_data_from_adc_regs();

            /* Start next conversion */
            (*internal_configuration.base_config.handle.adcsra_reg) |= 1U << ADSC ;
        }
    }
}
#else
void adc_isr_handler(void)
{
    isr_helper_extract_data_from_adc_regs();
    /* Start next conversion */
    *(internal_configuration.base_config.handle.adcsra_reg) |= (1 << ADSC) ;
}
#endif
