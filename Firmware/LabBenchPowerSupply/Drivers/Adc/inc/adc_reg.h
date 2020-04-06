#ifndef ADC_REG_HEADER
#define ADC_REG_HEADER

/* Expose this API to C++ code without name mangling */
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef enum {
    ADC_VOLTAGE_REF_INTERNAL_1V1 = 0x0,
    ADC_VOLTAGE_REF_AREF_PIN     = 0x1
} adc_voltage_ref_t;

typedef enum {
    ADC_RIGT_ALIGNED_RESULT = 0x0,
    ADC_LEFT_ALIGNED_RESULT = 0x1
} adc_result_align_t;

typedef enum {
    ADC_MUX_ADC0                 = 0x0,
    ADC_MUX_ADC1                 = 0x1,
    ADC_MUX_ADC2                 = 0x2,
    ADC_MUX_ADC3                 = 0x3,
    ADC_MUX_ADC4                 = 0x4,
    ADC_MUX_ADC5                 = 0x5,
    ADC_MUX_ADC6                 = 0x6,
    ADC_MUX_ADC7                 = 0x7,
    ADC_MUX_INTERNAL_TEMPERATURE = 0x8,
    ADC_MUX_1v1_REF              = 0b1110,
    ADC_MUX_GND                  = 0xF,
    ADC_MUX_COUNT                = 11
} adc_mux_t;

typedef enum {
    ADC_PRESCALER_DEFAULT_2 = 0U,
    ADC_PRESCALER_2         = 1U,
    ADC_PRESCALER_4         = 2U,
    ADC_PRESCALER_8         = 3U,
    ADC_PRESCALER_16        = 4U,
    ADC_PRESCALER_32        = 5U,
    ADC_PRESCALER_64        = 6U,
    ADC_PRESCALER_128       = 7U
} adc_prescaler_t;

typedef enum {
    ADC_TRIGGER_FREE_RUNNING        = 0U,
    ADC_TRIGGER_ANALOG_COMPARATOR   = 1U,
    ADC_TRIGGER_EXT_INT_REQUEST_0   = 2U,
    ADC_TRIGGER_TIMER0_COMP_A_INT   = 3U,
    ADC_TRIGGER_TIMER0_OVERFLOW     = 4U,
    ADC_TRIGGER_TIMER1_COMP_B_INT   = 5U,
    ADC_TRIGGER_TIMER1_OVERFLOW     = 6U,
    ADC_TRIGGER_TIMER1_CAPTURE_EVT  = 7U,
} adc_autotrigger_sources_t;

/* digital input register deactivation (for power consumption purposes) */
typedef enum {
    ADC_DIGITAL_IN_0 = 0U,
    ADC_DIGITAL_IN_1 = 1U,
    ADC_DIGITAL_IN_2 = 2U,
    ADC_DIGITAL_IN_3 = 3U,
    ADC_DIGITAL_IN_4 = 4U,
    ADC_DIGITAL_IN_5 = 5U,
    ADC_DIGITAL_IN_6 = 6U,
    ADC_DIGITAL_IN_7 = 7U
} adc_digital_in_t;

/* Expose this API to C++ code without name mangling */
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ADC_REG_HEADER */