#include "adc.h"
#include "adc_stack.h"
#include "assert.h"

/* 10 bits adc, full range */
#define ADC_MAX_VALUE       1024U
#define ADC_1V1_MILLIVOLT   1100U
#define ADC_VCC_MILLIVOLT   (VCC_SUPPLY_VOLTAGE * 1000U)

/* ADMUX register masks */
#define ADC_MUX_MSK             (0x0F)
#define ADC_REF_VOLTAGE_MSK     (1U << 6U)
#define ADC_RESULT_ADJUST_MSK   (1U << 5U)

/* ADCSRA register masks */
#define ADC_PRESCALER_MSK       (0b111)


static adc_channel_descriptor_t registered_channels[ADC_MUX_COUNT];

static inline void set_mux(adc_mux_t mux);
static inline bool is_adc_enabled(void);
static inline bool adc_conversion_has_ended(void);




static bool adc_is_initialised = false;

peripheral_error_t adc_init(adc_config_hal_t * const config, adc_handle_t const * const handle);
{
    static_assert(handle != NULL, L"Handle cannot be NULL!");
	/* Leave PORTC pins 0 1 2 as tristate pins (no IO functions) */
	IN_ADC0_DDREG &= ~(1<<IN_ADC0_PIN);
	IN_ADC0_PORT &= ~(1<<IN_ADC0_PIN);
	IN_ADC1_DDREG &= ~(1<<IN_ADC1_PIN);
	IN_ADC1_PORT &= ~(1<<IN_ADC1_PIN);
	IN_ADC2_DDREG &= ~(1<<IN_ADC2_PIN);
	IN_ADC2_PORT &= ~(1<<IN_ADC2_PIN);

	/* Configure ADMUX register */
    ADMUX = (ADMUX & ~ADC_REF_VOLTAGE_MSK) | (adc_config_hal.ref << REFS0);         /* set reference voltage */
    ADMUX = (ADMUX & ~ADC_RESULT_ADJUST_MSK) | (ADC_RIGT_ALIGNED_RESULT << ADLAR);  /* set result adjustment */
    ADCSRA = (ADCSRA & ~ADC_PRESCALER_MSK) | adc_config_hal.prescaler;              /* set precaler */
    ADCSRB = 0;

	/* enable adc peripheral and initialises analog circuitry
	 (takes 25 clock cycles 1rst time */
    ADCSRA |= 1U << ADEN | ADSC;
    adc_is_initialised = true;
}

error_t adc_process(void)
{
    error_t ret = ERROR_NONE;
    if (!is_adc_enabled() || !adc_is_initialised)
    {
        ret = ERROR_CONFIG;
    }
    else
    {
        if(adc_conversion_has_ended())
        {
            uint8_t low = ADCL;
            uint16_t result = (uint16_t)(ADCH << 8U) | low;

            adc_buffer.results[adc_buffer.cur_index] = result;
            adc_buffer.cur_index++;
            adc_buffer.cur_index %= ANALOG_DEVICES_CNT;
            /* Reset interrupt flag manually */
            ADCSRA |= (1U << ADIF);

            /* Start next conversion */
            set_mux(adc_config_hal.devices[adc_buffer.cur_index]);
            ADCSRA |= 1U << ADSC ;
        }
    }

    return ret;
}

error_t adc_get_result(const uint8_t index, adc_result_t * const result)
{
    error_t ret = ERROR_NONE;
    if (NULL == result || ANALOG_DEVICES_CNT <= index) {
        ret = ERROR_ARGUMENT;
    }

    if (ERROR_NONE == ret && !adc_is_initialised)
    {
        ret = ERROR_NOT_INITIALISED;
    }

    /* Extract last conversion result for this item */
    if (ERROR_NONE == ret) {
        *result = adc_buffer.results[index];
    }

    return ret;
}

error_t adc_read_millivolt(const uint8_t index, adc_millivolts_t * const reading)
{
    error_t ret = ERROR_NONE;
    if (NULL == reading || ANALOG_DEVICES_CNT <= index)
    {
        ret = ERROR_ARGUMENT;
    }

    if (ERROR_NONE == ret && !adc_is_initialised)
    {
        ret = ERROR_NOT_INITIALISED;
    }
    else
    {
        adc_result_t result = 0;
        ret = adc_get_result(index, &result);
        if (ERROR_NONE == ret)
        {
            switch (adc_config_hal.ref)
            {
                case ADC_VOLTAGE_REF_INTERNAL_1V1:
                    *reading =  (uint16_t)(((uint32_t)result * (uint32_t)ADC_1V1_MILLIVOLT) / ADC_MAX_VALUE);
                    break;
                case ADC_VOLTAGE_REF_AREF_PIN:
                    *reading = (uint16_t)(((uint32_t)result * (uint32_t)ADC_AREF_VOLTAGE_MILLIVOLT) / ADC_MAX_VALUE);
                    break;
                default:
                    *reading = 0;
                    ret = ERROR_ARGUMENT;
                    break;
            }
        }

    }

    return ret;
}



static inline void set_mux(adc_mux_t mux)
{
    ADMUX = (ADMUX & ~ADC_MUX_MSK) | mux;
}

static inline bool is_adc_enabled(void)
{
    return (bool)((ADCSRA & (1U << ADEN)) >> ADEN);
}


static inline bool adc_conversion_has_ended(void)
{
    bool ret = false;
    /* Check if interrupt flag is set or if no conversion is running*/
    if ((ADCSRA & (1U << ADIF ))
    ||  !(ADCSRA & (1U << ADSC)))
    {
        ret = true;
    }
    return ret;
}
