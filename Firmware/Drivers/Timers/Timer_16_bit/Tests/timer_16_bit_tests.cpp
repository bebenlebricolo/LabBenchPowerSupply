#include "gtest/gtest.h"
#include "config.h"
#include "timer_16_bit.h"
#include "timer_generic.h"
#include "timer_16_bit_registers_stub.h"

/* Default timer id */
#define DT_ID 0

class Timer16BitFixture : public ::testing::Test
{
public:
    timer_16_bit_config_t config;
protected:
    void SetUp() override
    {
        timer_16_bit_registers_stub_erase();
        (void) timer_16_bit_get_default_config(&config);
        timer_16_bit_registers_stub_init_handle(&config.handle);
        (void) timer_16_bit_set_handle(DT_ID, &config.handle);
    }
    void TearDown() override
    {
    }
};

TEST(timer_16_bit_driver_tests, guard_null_handle)
{
    timer_16_bit_config_t config;
    timer_error_t ret = timer_16_bit_get_default_config(&config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);

    /* We should have a null handle at the moment */
    ASSERT_TRUE(NULL == (void*) (config.handle.OCRA_H));
    ASSERT_TRUE(NULL == (void*) (config.handle.OCRA_L));
    ASSERT_TRUE(NULL == (void*) (config.handle.OCRB_H));
    ASSERT_TRUE(NULL == (void*) (config.handle.OCRB_L));
    ASSERT_TRUE(NULL == (void*) (config.handle.TCCRA));
    ASSERT_TRUE(NULL == (void*) (config.handle.TCCRB));
    ASSERT_TRUE(NULL == (void*) (config.handle.TCCRC));
    ASSERT_TRUE(NULL == (void*) (config.handle.TCNT_H));
    ASSERT_TRUE(NULL == (void*) (config.handle.TCNT_L));
    ASSERT_TRUE(NULL == (void*) (config.handle.ICR_H));
    ASSERT_TRUE(NULL == (void*) (config.handle.ICR_L));
    ASSERT_TRUE(NULL == (void*) (config.handle.TIFR));
    ASSERT_TRUE(NULL == (void*) (config.handle.TIMSK));

    /* Test compare match mode A get/set api */
    ret = timer_16_bit_set_compare_match_A(DT_ID, config.timing_config.comp_match_a);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_16_bit_get_compare_match_A(DT_ID, &config.timing_config.comp_match_a);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test compare match mode B get/set api */
    ret = timer_16_bit_set_compare_match_B(DT_ID, config.timing_config.comp_match_b);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_16_bit_get_compare_match_B(DT_ID, &config.timing_config.comp_match_b);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test handle setting function */
    ret = timer_16_bit_set_handle(DT_ID, &config.handle);
    ASSERT_EQ(TIMER_ERROR_OK, ret);

    /* Test interrupt config get/set api */
    ret = timer_16_bit_set_interrupt_config(DT_ID, &config.interrupt_config);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_16_bit_get_interrupt_config(DT_ID, &config.interrupt_config);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test OCRA get/set api */
    ret = timer_16_bit_set_ocra_register_value(DT_ID, &config.timing_config.ocra_val);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_16_bit_get_ocra_register_value(DT_ID, &config.timing_config.ocra_val);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test OCRB get/set api */
    ret = timer_16_bit_set_ocrb_register_value(DT_ID, &config.timing_config.ocrb_val);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_16_bit_get_ocrb_register_value(DT_ID, &config.timing_config.ocrb_val);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test counter get/set api */
    ret = timer_16_bit_set_counter_value(DT_ID, &config.timing_config.counter);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_16_bit_get_counter_value(DT_ID, &config.timing_config.counter);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test force compare flags get/set api */
    ret = timer_16_bit_set_force_compare_config(DT_ID, &config.force_compare);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_16_bit_get_force_compare_config(DT_ID, &config.force_compare);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test prescaler get/set api */
    ret = timer_16_bit_set_prescaler(DT_ID, config.timing_config.prescaler);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_16_bit_get_prescaler(DT_ID, &config.timing_config.prescaler);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test waveform generation get/set api */
    ret = timer_16_bit_set_waveform_generation(DT_ID, config.timing_config.waveform_mode);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_16_bit_get_waveform_generation(DT_ID, &config.timing_config.waveform_mode);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test input capture edge select get/set api */
    ret = timer_16_bit_set_input_compare_edge_select(DT_ID, config.input_capture.edge_select);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_16_bit_get_input_compare_edge_select(DT_ID, &config.input_capture.edge_select);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test input capture noise canceler get/set api */
    ret = timer_16_bit_set_input_compare_noise_canceler(DT_ID, config.input_capture.use_noise_canceler);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_16_bit_get_input_compare_noise_canceler(DT_ID, &config.input_capture.use_noise_canceler);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
}

TEST(timer_16_bit_driver_tests, guard_null_pointer)
{
    timer_16_bit_config_t * nullptr_config = NULL;

    timer_error_t ret = timer_16_bit_get_default_config(nullptr_config);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    /* Test compare match mode A get/set api */
    timer_16_bit_compare_output_mode_t * nullptr_compare_output_mode = NULL;
    ret = timer_16_bit_get_compare_match_A(DT_ID, nullptr_compare_output_mode);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);
    ret = timer_16_bit_get_compare_match_B(DT_ID, nullptr_compare_output_mode);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    timer_16_bit_handle_t * nullptr_handle = NULL;
    /* Test handle setting function */
    ret = timer_16_bit_set_handle(DT_ID, nullptr_handle);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    /* Test interrupt config get/set api */
    timer_16_bit_interrupt_config_t * nullptr_interrupt_config = NULL;
    ret = timer_16_bit_set_interrupt_config(DT_ID, nullptr_interrupt_config);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);
    ret = timer_16_bit_get_interrupt_config(DT_ID, nullptr_interrupt_config);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    /* Test OCRA get/set api */
    uint16_t * nullptr_ocrx_val = NULL;
    ret = timer_16_bit_get_ocra_register_value(DT_ID, nullptr_ocrx_val);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);
    /* Test OCRB get/set api */
    ret = timer_16_bit_get_ocrb_register_value(DT_ID, nullptr_ocrx_val);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);
    /* Test counter get/set api */
    ret = timer_16_bit_get_counter_value(DT_ID, nullptr_ocrx_val);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    /* Test force compare flags get/set api */
    timer_16_bit_force_compare_config_t * nullptr_force_compare = NULL;
    ret = timer_16_bit_set_force_compare_config(DT_ID, nullptr_force_compare);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);
    ret = timer_16_bit_get_force_compare_config(DT_ID, nullptr_force_compare);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    /* Test prescaler get/set api */
    timer_16_bit_prescaler_selection_t * nullptr_prescaler = NULL;
    ret = timer_16_bit_get_prescaler(DT_ID, nullptr_prescaler);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    timer_16_bit_waveform_generation_t * nullptr_waveform_mode = NULL;
    ret = timer_16_bit_get_waveform_generation(DT_ID, nullptr_waveform_mode);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    /* Test input capture edge select get/set api */
    timer_16_bit_input_capture_edge_select_flag_t * nullptr_edge_select = NULL;
    ret = timer_16_bit_get_input_compare_edge_select(DT_ID, nullptr_edge_select);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    /* Test input capture noise canceler get/set api */
    bool * nullptr_use_input_capture_noise_canceler = NULL;
    ret = timer_16_bit_get_input_compare_noise_canceler(DT_ID, nullptr_use_input_capture_noise_canceler);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);
}

TEST(timer_16_bit_driver_tests, guard_wrong_id)
{
    timer_16_bit_config_t config;
    uint8_t targeted_id = DT_ID + 1;

    timer_error_t ret = timer_16_bit_get_default_config(&config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);

    /* Test compare match mode A get/set api */
    timer_16_bit_compare_output_mode_t * nullptr_compare_output_mode = NULL;
    ret = timer_16_bit_get_compare_match_A(targeted_id, nullptr_compare_output_mode);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);
    ret = timer_16_bit_get_compare_match_B(targeted_id, nullptr_compare_output_mode);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);

    /* Test handle setting function */
    ret = timer_16_bit_set_handle(targeted_id, &config.handle);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);

    /* Test interrupt config get/set api */
    ret = timer_16_bit_set_interrupt_config(targeted_id, &config.interrupt_config);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);
    ret = timer_16_bit_get_interrupt_config(targeted_id, &config.interrupt_config);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);

    /* Test OCRA get/set api */
    ret = timer_16_bit_get_ocra_register_value(targeted_id, &config.timing_config.ocra_val);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);
    ret = timer_16_bit_set_ocra_register_value(targeted_id, &config.timing_config.ocra_val);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);

    /* Test OCRB get/set api */
    ret = timer_16_bit_get_ocrb_register_value(targeted_id, &config.timing_config.ocra_val);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);
    ret = timer_16_bit_set_ocrb_register_value(targeted_id, &config.timing_config.ocra_val);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);

    /* Test counter get/set api */
    ret = timer_16_bit_get_counter_value(targeted_id, &config.timing_config.ocra_val);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);
    ret = timer_16_bit_set_counter_value(targeted_id, &config.timing_config.ocra_val);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);

    /* Test force compare flags get/set api */
    ret = timer_16_bit_set_force_compare_config(targeted_id, &config.force_compare);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);
    ret = timer_16_bit_get_force_compare_config(targeted_id, &config.force_compare);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);

    /* Test prescaler get/set api */
    ret = timer_16_bit_get_prescaler(targeted_id, &config.timing_config.prescaler);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);
    ret = timer_16_bit_set_prescaler(targeted_id, config.timing_config.prescaler);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);

    /* Test waveform generation mode get/set api */
    ret = timer_16_bit_get_waveform_generation(targeted_id, &config.timing_config.waveform_mode);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);
    ret = timer_16_bit_set_waveform_generation(targeted_id, config.timing_config.waveform_mode);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);

    /* Test input capture edge select get/set api */
    ret = timer_16_bit_set_input_compare_edge_select(targeted_id, config.input_capture.edge_select);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);
    ret = timer_16_bit_get_input_compare_edge_select(targeted_id, &config.input_capture.edge_select);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);

    /* Test input capture noise canceler get/set api */
    ret = timer_16_bit_set_input_compare_noise_canceler(targeted_id, config.input_capture.use_noise_canceler);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);
    ret = timer_16_bit_get_input_compare_noise_canceler(targeted_id, &config.input_capture.use_noise_canceler);
    ASSERT_EQ(TIMER_ERROR_UNKNOWN_TIMER, ret);
}

TEST_F(Timer16BitFixture, test_handle_is_set_correctly)
{
    // Test a subset of setters to see if values are set correctly within targeted stub registers
    timer_error_t ret = TIMER_ERROR_OK;
    // Data should be reset to its default state when we begin

    // Testing TCCRA register
    config.timing_config.comp_match_a = TIMER16BIT_CMOD_SET_OCnX; // 11
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRA & COMA_MSK, 0U);
    ret = timer_16_bit_set_compare_match_A(DT_ID, config.timing_config.comp_match_a);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRA & COMA_MSK, 0x3 << COMA0_BIT);

    // Testing TCCRB register
    config.timing_config.prescaler = TIMER16BIT_CLK_PRESCALER_256; // 11
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRB & CS_MSK, (uint8_t)TIMER16BIT_CLK_NO_CLOCK);
    ret = timer_16_bit_set_prescaler(DT_ID, config.timing_config.prescaler);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRB & CS_MSK, (uint8_t) config.timing_config.prescaler);

    // Testing TCNT register
    uint16_t tcnt_value = 1440U;
    ASSERT_EQ(timer_16_bit_registers_stub.TCNT_H , 0U);
    ASSERT_EQ(timer_16_bit_registers_stub.TCNT_L , 0U);
    ret = timer_16_bit_set_counter_value(DT_ID, &tcnt_value);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TCNT_H , (tcnt_value & 0xFF00) >> 8U);
    ASSERT_EQ(timer_16_bit_registers_stub.TCNT_L , (tcnt_value & 0xFF));

    // Testing OCRA register
    uint16_t ocra_value = 1000U;
    ASSERT_EQ(timer_16_bit_registers_stub.OCRA_H , 0U);
    ASSERT_EQ(timer_16_bit_registers_stub.OCRA_L , 0U);
    ret = timer_16_bit_set_ocra_register_value(DT_ID, &ocra_value);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.OCRA_H , (ocra_value & 0xFF00) >> 8U);
    ASSERT_EQ(timer_16_bit_registers_stub.OCRA_L , ocra_value & 0xFF);

    // Testing OCRB register
    uint16_t ocrb_value = 6000U;
    ASSERT_EQ(timer_16_bit_registers_stub.OCRB_H , 0U);
    ASSERT_EQ(timer_16_bit_registers_stub.OCRB_L , 0U);
    ret = timer_16_bit_set_ocrb_register_value(DT_ID, &ocrb_value);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.OCRB_H , (ocrb_value & 0xFF00) >> 8U);
    ASSERT_EQ(timer_16_bit_registers_stub.OCRB_L , ocrb_value & 0xFF);

    // Testing TIMSK register
    config.interrupt_config.it_comp_match_a = true;
    config.interrupt_config.it_comp_match_b = true;
    config.interrupt_config.it_timer_overflow = true;
    config.interrupt_config.it_input_capture = true;

    ASSERT_EQ(timer_16_bit_registers_stub.TIMSK, 0U);
    ret = timer_16_bit_set_interrupt_config(DT_ID, &config.interrupt_config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TIMSK & ICIE_MSK, (uint8_t) 1U << ICIE_BIT);
    ASSERT_EQ(timer_16_bit_registers_stub.TIMSK & OCIEA_MSK, (uint8_t) 1U << OCIEA_BIT);
    ASSERT_EQ(timer_16_bit_registers_stub.TIMSK & OCIEB_MSK, (uint8_t) 1U << OCIEB_BIT);
    ASSERT_EQ(timer_16_bit_registers_stub.TIMSK & TOIE_MSK, (uint8_t) 1U);

    // Testing TIFR register
    config.interrupt_config.it_comp_match_a = false;
    config.interrupt_config.it_comp_match_b = false;
    config.interrupt_config.it_timer_overflow = false;
    config.interrupt_config.it_input_capture = false;
    ASSERT_EQ(timer_16_bit_registers_stub.TIFR , 0U);
    timer_16_bit_registers_stub.TIFR |= ICF_MSK | TOV_MSK | OCFA_MSK | OCFB_MSK; // set all interrupt flags
    ret = timer_16_bit_get_interrupt_flags(DT_ID, &config.interrupt_config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TIFR & ICIE_MSK, 1U << ICIE_BIT);
    ASSERT_EQ(timer_16_bit_registers_stub.TIFR & OCIEA_MSK, 1U << OCIEA_BIT);
    ASSERT_EQ(timer_16_bit_registers_stub.TIFR & OCIEB_MSK, 1U << OCIEB_BIT);
    ASSERT_EQ(timer_16_bit_registers_stub.TIFR & TOIE_MSK, 1U );

}


TEST_F(Timer16BitFixture, test_timing_configuration_unitary_functions)
{
    auto ret = TIMER_ERROR_OK;
    // Random configuration, with bits set everywhere
    timer_16_bit_compare_output_mode_t received_compare_mode_a = config.timing_config.comp_match_a;
    timer_16_bit_compare_output_mode_t received_compare_mode_b = config.timing_config.comp_match_b;
    uint16_t received_ocra_val = config.timing_config.ocra_val;
    uint16_t received_ocrb_val = config.timing_config.ocrb_val;
    uint16_t received_counter_val = config.timing_config.counter;
    timer_16_bit_prescaler_selection_t received_prescaler = config.timing_config.prescaler;
    timer_16_bit_waveform_generation_t received_waveform = config.timing_config.waveform_mode;

    config.timing_config.comp_match_a = TIMER16BIT_CMOD_TOGGLE_OCnX;
    config.timing_config.comp_match_b = TIMER16BIT_CMOD_SET_OCnX;
    config.timing_config.ocra_val = 13520U;
    config.timing_config.ocrb_val = 1560U;
    config.timing_config.prescaler = TIMER16BIT_CLK_PRESCALER_64;
    config.timing_config.waveform_mode = TIMER16BIT_WG_PWM_PHASE_AND_FREQ_CORRECT_ICR_MAX;

    // Compare match A mode
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRA & COMA_MSK, 0U);
    ret = timer_16_bit_set_compare_match_A(DT_ID, config.timing_config.comp_match_a);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRA & COMA_MSK, config.timing_config.comp_match_a << COMA0_BIT);
    // Counter check value can also be read back from registers ...
    ret = timer_16_bit_get_compare_match_A(DT_ID,&received_compare_mode_a);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(received_compare_mode_a, config.timing_config.comp_match_a);

    // Compare match B mode
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRA & COMB_MSK, 0U);
    ret = timer_16_bit_set_compare_match_B(DT_ID, config.timing_config.comp_match_b);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRA & COMB_MSK, config.timing_config.comp_match_b << COMB0_BIT);
    // Counter check value can also be read back from registers ...
    ret = timer_16_bit_get_compare_match_B(DT_ID,&received_compare_mode_b);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(received_compare_mode_b, config.timing_config.comp_match_b);

    // Waveform modes
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRA & (WGM0_MSK | WGM1_MSK), 0U);
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRB & (WGM2_MSK | WGM3_MSK), 0U);
    ret = timer_16_bit_set_waveform_generation(DT_ID, config.timing_config.waveform_mode);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRA & (WGM0_MSK | WGM1_MSK), (config.timing_config.waveform_mode & 0x3));
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRB & (WGM2_MSK | WGM3_MSK), (config.timing_config.waveform_mode & 0xC) << 1U);
    // Counter check value can also be read back from registers ...
    ret = timer_16_bit_get_waveform_generation(DT_ID, &received_waveform);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(received_waveform, config.timing_config.waveform_mode);


    // Prescaler settings
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRB & CS_MSK, 0U);
    ret = timer_16_bit_set_prescaler(DT_ID, config.timing_config.prescaler);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRB & CS_MSK, config.timing_config.prescaler);
    ret = timer_16_bit_get_prescaler(DT_ID, &received_prescaler);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(received_prescaler, config.timing_config.prescaler);


    // Timer counter main register value
    ASSERT_EQ(timer_16_bit_registers_stub.TCNT_H, 0U);
    ASSERT_EQ(timer_16_bit_registers_stub.TCNT_L, 0U);
    ret = timer_16_bit_set_counter_value(DT_ID, &config.timing_config.counter);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TCNT_H, (config.timing_config.counter & 0xFF00) >> 8U );
    ASSERT_EQ(timer_16_bit_registers_stub.TCNT_L, (config.timing_config.counter & 0xFF));
    ret = timer_16_bit_get_counter_value(DT_ID, &received_counter_val);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(received_counter_val, config.timing_config.counter);


    // OCRA register value
    ASSERT_EQ(timer_16_bit_registers_stub.OCRA_H, 0U);
    ASSERT_EQ(timer_16_bit_registers_stub.OCRA_L, 0U);
    ret = timer_16_bit_set_ocra_register_value(DT_ID, &config.timing_config.ocra_val);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.OCRA_H, (config.timing_config.ocra_val & 0xFF00) >> 8U);
    ASSERT_EQ(timer_16_bit_registers_stub.OCRA_L, config.timing_config.ocra_val & 0xFF);
    ret = timer_16_bit_get_ocra_register_value(DT_ID, &received_ocra_val);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(received_ocra_val, config.timing_config.ocra_val);


    // OCRB register value
    ASSERT_EQ(timer_16_bit_registers_stub.OCRB_H, 0U);
    ASSERT_EQ(timer_16_bit_registers_stub.OCRB_L, 0U);
    ret = timer_16_bit_set_ocrb_register_value(DT_ID, &config.timing_config.ocrb_val);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.OCRB_H, (config.timing_config.ocrb_val & 0xFF00) >> 8U);
    ASSERT_EQ(timer_16_bit_registers_stub.OCRB_L, config.timing_config.ocrb_val & 0xFF);
    ret = timer_16_bit_get_ocrb_register_value(DT_ID, &received_ocrb_val);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(received_ocrb_val, config.timing_config.ocrb_val);

}

TEST_F(Timer16BitFixture, test_force_compare_flags)
{
    auto ret = TIMER_ERROR_OK;
    timer_16_bit_force_compare_config_t received_config;
    memcpy(&received_config, &config.force_compare, sizeof(timer_16_bit_force_compare_config_t));

    config.force_compare.force_comp_match_a = true;
    config.force_compare.force_comp_match_b = true;

    ASSERT_EQ(timer_16_bit_registers_stub.TCCRC, 0);
    ret = timer_16_bit_set_force_compare_config(DT_ID, &config.force_compare);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRC & (FOCA_MSK | FOCB_MSK), FOCA_MSK | FOCB_MSK);

    ret = timer_16_bit_get_force_compare_config(DT_ID, &received_config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(0, memcmp(&config.force_compare, &received_config, sizeof(timer_16_bit_force_compare_config_t)));
}

TEST_F(Timer16BitFixture, test_interrupt_enable_flags)
{
    auto ret = TIMER_ERROR_OK;
    timer_16_bit_interrupt_config_t received_interrupt_config;
    // Copy default state of interrupt config
    memcpy(&received_interrupt_config, &config.interrupt_config, sizeof(timer_16_bit_interrupt_config_t));

    config.interrupt_config.it_comp_match_a = true;
    config.interrupt_config.it_comp_match_b = true;
    config.interrupt_config.it_timer_overflow = true;
    config.interrupt_config.it_input_capture = true;

    ASSERT_EQ(timer_16_bit_registers_stub.TIMSK, 0);
    ret = timer_16_bit_set_interrupt_config(DT_ID, &config.interrupt_config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TIMSK, ICIE_MSK | OCIEA_MSK | OCIEB_MSK | TOIE_MSK);

    // Compare output to input : shall be exactly matching
    ret = timer_16_bit_get_interrupt_config(DT_ID, &received_interrupt_config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(0, memcmp(&config.interrupt_config, &received_interrupt_config, sizeof(timer_16_bit_interrupt_config_t)));

    // set all interrupt flags to true
    received_interrupt_config.it_comp_match_a = false;
    received_interrupt_config.it_comp_match_b = false;
    received_interrupt_config.it_timer_overflow = false;
    received_interrupt_config.it_input_capture = false;
    timer_16_bit_registers_stub.TIFR = TOV_MSK | ICF_MSK | OCFA_MSK | OCFB_MSK;
    ret = timer_16_bit_get_interrupt_flags(DT_ID, &received_interrupt_config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(0, memcmp(&config.interrupt_config, &received_interrupt_config, sizeof(timer_16_bit_interrupt_config_t)));
}

TEST_F(Timer16BitFixture, test_input_capture_peripheral)
{
    timer_error_t ret = TIMER_ERROR_OK;

    /* Test input capture edge select get/set api */
    config.input_capture.edge_select = TIMER16BIT_INPUT_CAPTURE_EDGE_RISING_EDGE;
    timer_16_bit_registers_stub.TCCRB &= ~(INC_MSK | ICES_MSK);
    timer_16_bit_input_capture_edge_select_flag_t edge_select;
    ret = timer_16_bit_set_input_compare_edge_select(DT_ID, config.input_capture.edge_select);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRB & (ICES_MSK), ICES_MSK);
    ret = timer_16_bit_get_input_compare_edge_select(DT_ID, &edge_select);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(edge_select , config.input_capture.edge_select);

    config.input_capture.edge_select = TIMER16BIT_INPUT_CAPTURE_EDGE_FALLING_EDGE;
    ret = timer_16_bit_set_input_compare_edge_select(DT_ID, config.input_capture.edge_select);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRB & (ICES_MSK), 0);
    ret = timer_16_bit_get_input_compare_edge_select(DT_ID, &edge_select);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(edge_select , config.input_capture.edge_select);

    /* Test input capture noise canceler get/set api */
    config.input_capture.use_noise_canceler = true;
    ret = timer_16_bit_set_input_compare_noise_canceler(DT_ID, config.input_capture.use_noise_canceler);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRB & (INC_MSK), INC_MSK);
    bool use_noise_canceler = false;
    ret = timer_16_bit_get_input_compare_noise_canceler(DT_ID, &use_noise_canceler);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(config.input_capture.use_noise_canceler, use_noise_canceler);

    config.input_capture.use_noise_canceler = false;
    ret = timer_16_bit_set_input_compare_noise_canceler(DT_ID, config.input_capture.use_noise_canceler);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_16_bit_registers_stub.TCCRB & (INC_MSK), 0);
    ret = timer_16_bit_get_input_compare_noise_canceler(DT_ID, &use_noise_canceler);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(config.input_capture.use_noise_canceler, use_noise_canceler);
}

TEST_F(Timer16BitFixture, test_initialisation_deinitialisation)
{
    timer_error_t ret = TIMER_ERROR_OK;

    // We should not be able to start/stop the timer as it hasn't been configured yet
    ret = timer_16_bit_start(DT_ID);
    ASSERT_EQ(ret, TIMER_ERROR_NOT_INITIALISED);

    ret = timer_16_bit_stop(DT_ID);
    ASSERT_EQ(ret, TIMER_ERROR_NOT_INITIALISED);

    ret = timer_16_bit_init(DT_ID, &config);
    ASSERT_EQ(ret, TIMER_ERROR_OK);

    // Now we should be able to start/stop the timer
    ret = timer_16_bit_start(DT_ID);
    ASSERT_EQ(ret, TIMER_ERROR_OK);

    ret = timer_16_bit_stop(DT_ID);
    ASSERT_EQ(ret, TIMER_ERROR_OK);

    ret = timer_16_bit_deinit(DT_ID);
    ASSERT_EQ(ret, TIMER_ERROR_OK);

    // We should not be able to start/stop the timer as it hasn't been configured yet
    ret = timer_16_bit_start(DT_ID);
    ASSERT_EQ(ret, TIMER_ERROR_NOT_INITIALISED);

    ret = timer_16_bit_stop(DT_ID);
    ASSERT_EQ(ret, TIMER_ERROR_NOT_INITIALISED);

}

TEST(timer_16_bit_driver_tests, test_parameters_computation_prescaler)
{
    uint32_t cpu_freq = 16'000'000;
    uint32_t target_freq = 1'000;
    uint16_t ocra = 0;
    uint16_t accumulator = 0;

    timer_16_bit_prescaler_selection_t prescaler = TIMER16BIT_CLK_PRESCALER_1;
    timer_16_bit_compute_matching_parameters(&cpu_freq, &target_freq, &prescaler, &ocra, &accumulator);
    ASSERT_EQ(prescaler, TIMER16BIT_CLK_PRESCALER_1);
    ASSERT_EQ(ocra, 15999U);
    ASSERT_EQ(accumulator, 0U);

    target_freq = 12'000'000;
    timer_16_bit_compute_matching_parameters(&cpu_freq, &target_freq, &prescaler, &ocra, &accumulator);
    // This timer could not achieve such a target frequency because even in its fastest configuration,
    // with a prescaler of 1 and ocra of 0, the timer will tick at 16 MHz.
    // As output frequency is dependent on ocra, an ocra of 1 divides input cpu frequency by 2, which gives 8 MHz.
    // So 12 MHz is simply unachievable by this timer because clock speeds are just too close to one another.
    ASSERT_EQ(prescaler, TIMER16BIT_CLK_PRESCALER_1);
    ASSERT_EQ(ocra, 0U);
    ASSERT_EQ(accumulator, 0U);

    target_freq = 8'000'000;
    timer_16_bit_compute_matching_parameters(&cpu_freq, &target_freq, &prescaler, &ocra, &accumulator);
    ASSERT_EQ(prescaler, TIMER16BIT_CLK_PRESCALER_1);
    ASSERT_EQ(ocra, 1U);
    ASSERT_EQ(accumulator, 0U);

    cpu_freq = 8'000'000;
    target_freq = 3'000;
    timer_16_bit_compute_matching_parameters(&cpu_freq, &target_freq, &prescaler, &ocra, &accumulator);
    ASSERT_EQ(prescaler, TIMER16BIT_CLK_PRESCALER_1);
    ASSERT_EQ(ocra, 2665U);
    ASSERT_EQ(accumulator, 0U);

    cpu_freq = 16'000'000;
    target_freq = 1U;
    timer_16_bit_compute_matching_parameters(&cpu_freq, &target_freq, &prescaler, &ocra, &accumulator);
    ASSERT_EQ(prescaler, TIMER16BIT_CLK_PRESCALER_256);
    ASSERT_EQ(ocra, 62499U);
    ASSERT_EQ(accumulator, 0U);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}