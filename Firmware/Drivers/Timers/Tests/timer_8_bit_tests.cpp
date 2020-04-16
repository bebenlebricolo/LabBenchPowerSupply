#include "gtest/gtest.h"
#include "config.h"
#include "timer_8_bit.h"
#include "timer_8_bit_registers_stub.h"

/* Default timer id */
#define DT_ID 0

class Timer8BitFixture : public ::testing::Test
{
public:
    timer_8_bit_config_t config;
protected:
    void SetUp() override
    {
        timer_8_bit_registers_stub_erase();
        (void) timer_8_bit_get_default_config(DT_ID , &config);
        timer_8_bit_registers_stub_init_handle(&config.handle);
        (void) timer_8_bit_set_handle(DT_ID, &config.handle);
    }
    void TearDown() override
    {
    }
};

TEST(timer_8_bit_driver_tests, guard_null_handle)
{
    timer_8_bit_config_t config;
    timer_error_t ret = timer_8_bit_get_default_config(DT_ID , &config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);

    /* We should have a null handle at the moment */
    ASSERT_TRUE(NULL == (void*) (config.handle.OCRA)   );
    ASSERT_TRUE(NULL == (void*) (config.handle.OCRB)   );
    ASSERT_TRUE(NULL == (void*) (config.handle.TCCRA)  );
    ASSERT_TRUE(NULL == (void*) (config.handle.TCCRB)  );
    ASSERT_TRUE(NULL == (void*) (config.handle.TCNT)   );
    ASSERT_TRUE(NULL == (void*) (config.handle.TIFR)   );
    ASSERT_TRUE(NULL == (void*) (config.handle.TIMSK)  );

    /* Test compare match mode A get/set api */
    ret = timer_8_bit_set_compare_match_A(DT_ID, config.timing_config.comp_match_a);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_8_bit_get_compare_match_A(DT_ID, &config.timing_config.comp_match_a);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test compare match mode B get/set api */
    ret = timer_8_bit_set_compare_match_B(DT_ID, config.timing_config.comp_match_b);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_8_bit_get_compare_match_B(DT_ID, &config.timing_config.comp_match_b);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test handle setting function */
    ret = timer_8_bit_set_handle(DT_ID, &config.handle);
    ASSERT_EQ(TIMER_ERROR_OK, ret);

    /* Test interrupt config get/set api */
    ret = timer_8_bit_set_interrupt_config(DT_ID, &config.interrupt_config);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_8_bit_get_interrupt_config(DT_ID, &config.interrupt_config);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test OCRA get/set api */
    ret = timer_8_bit_set_ocra_register_value(DT_ID, config.timing_config.ocra_val);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_8_bit_get_ocra_register_value(DT_ID, &config.timing_config.ocra_val);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test OCRB get/set api */
    ret = timer_8_bit_set_ocrb_register_value(DT_ID, config.timing_config.ocrb_val);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_8_bit_get_ocrb_register_value(DT_ID, &config.timing_config.ocrb_val);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test counter get/set api */
    ret = timer_8_bit_set_counter_value(DT_ID, config.timing_config.counter);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_8_bit_get_counter_value(DT_ID, &config.timing_config.counter);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test force compare flags get/set api */
    ret = timer_8_bit_set_force_compare_config(DT_ID, &config.force_compare);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_8_bit_get_force_compare_config(DT_ID, &config.force_compare);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test prescaler get/set api */
    ret = timer_8_bit_set_prescaler(DT_ID, config.timing_config.prescaler);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_8_bit_get_prescaler(DT_ID, &config.timing_config.prescaler);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);

    /* Test waveforme generation get/set api */
    ret = timer_8_bit_set_waveform_generation(DT_ID, config.timing_config.waveform_mode);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
    ret = timer_8_bit_get_waveform_generation(DT_ID, &config.timing_config.waveform_mode);
    ASSERT_EQ(TIMER_ERROR_NULL_HANDLE, ret);
}

TEST(timer_8_bit_driver_tests, guard_null_pointer)
{
    timer_8_bit_config_t * nullptr_config = NULL;

    timer_error_t ret = timer_8_bit_get_default_config(DT_ID , nullptr_config);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    /* Test compare match mode A get/set api */
    timer_8_bit_compare_output_mode_t * nullptr_compare_output_mode = NULL;
    ret = timer_8_bit_get_compare_match_A(DT_ID, nullptr_compare_output_mode);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);
    ret = timer_8_bit_get_compare_match_B(DT_ID, nullptr_compare_output_mode);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    timer_8_bit_handle_t * nullptr_handle = NULL;
    /* Test handle setting function */
    ret = timer_8_bit_set_handle(DT_ID, nullptr_handle);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    /* Test interrupt config get/set api */
    timer_8_bit_interrupt_config_t * nullptr_interrupt_config = NULL;
    ret = timer_8_bit_set_interrupt_config(DT_ID, nullptr_interrupt_config);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);
    ret = timer_8_bit_get_interrupt_config(DT_ID, nullptr_interrupt_config);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    /* Test OCRA get/set api */
    uint8_t * nullptr_ocrx_val = NULL;
    ret = timer_8_bit_get_ocra_register_value(DT_ID, nullptr_ocrx_val);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);
    /* Test OCRB get/set api */
    ret = timer_8_bit_get_ocrb_register_value(DT_ID, nullptr_ocrx_val);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);
    /* Test counter get/set api */
    ret = timer_8_bit_get_counter_value(DT_ID, nullptr_ocrx_val);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    /* Test force compare flags get/set api */
    timer_x_bit_force_compare_config_t * nullptr_force_compare = NULL;
    ret = timer_8_bit_set_force_compare_config(DT_ID, nullptr_force_compare);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);
    ret = timer_8_bit_get_force_compare_config(DT_ID, nullptr_force_compare);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    /* Test prescaler get/set api */
    timer_x_bit_prescaler_selection_t * nullptr_prescaler = NULL;
    ret = timer_8_bit_get_prescaler(DT_ID, nullptr_prescaler);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);

    timer_8_bit_waveform_generation_t * nullptr_waveform_mode = NULL;
    ret = timer_8_bit_get_waveform_generation(DT_ID, nullptr_waveform_mode);
    ASSERT_EQ(TIMER_ERROR_NULL_POINTER, ret);
}

TEST_F(Timer8BitFixture, test_handle_is_set_correctly)
{
    // Test a subset of setters to see if values are set correctly within targeted stub registers
    timer_error_t ret = TIMER_ERROR_OK;
    // Data should be reset to its default state when we begin

    // Testing TCCRA register
    config.timing_config.comp_match_a = TIMER8BIT_CMOD_SET_OCnX; // 11
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRA & COMA_MSK, 0U);
    ret = timer_8_bit_set_compare_match_A(DT_ID, config.timing_config.comp_match_a);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRA & COMA_MSK, 0x3 << COMA_BIT);

    // Testing TCCRB register
    config.timing_config.prescaler = TIMERxBIT_CLK_PRESCALER_256; // 11
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRB & CS_MSK, (uint8_t)TIMERxBIT_CLK_NO_CLOCK);
    ret = timer_8_bit_set_prescaler(DT_ID, config.timing_config.prescaler);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRB & CS_MSK, (uint8_t) config.timing_config.prescaler);

    // Testing TCNT register
    uint8_t tcnt_value = 144U;
    ASSERT_EQ(timer_8_bit_registers_stub.TCNT , 0U);
    ret = timer_8_bit_set_counter_value(DT_ID, tcnt_value);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.TCNT , tcnt_value);

    // Testing OCRA register
    uint8_t ocra_value = 100U;
    ASSERT_EQ(timer_8_bit_registers_stub.OCRA , 0U);
    ret = timer_8_bit_set_ocra_register_value(DT_ID, ocra_value);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.OCRA , ocra_value);

    // Testing OCRB register
    uint8_t ocrb_value = 60U;
    ASSERT_EQ(timer_8_bit_registers_stub.OCRB , 0U);
    ret = timer_8_bit_set_ocrb_register_value(DT_ID, ocrb_value);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.OCRB , ocrb_value);

    // Testing TIMSK register
    config.interrupt_config.it_comp_match_a = true;
    config.interrupt_config.it_comp_match_b = true;
    config.interrupt_config.it_timer_overflow = true;

    ASSERT_EQ(timer_8_bit_registers_stub.TIMSK, 0U);
    ret = timer_8_bit_set_interrupt_config(DT_ID, &config.interrupt_config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.TIMSK & OCIEA_MSK, (uint8_t) 1U << OCIEA_BIT);
    ASSERT_EQ(timer_8_bit_registers_stub.TIMSK & OCIEB_MSK, (uint8_t) 1U << OCIEB_BIT);
    ASSERT_EQ(timer_8_bit_registers_stub.TIMSK & TOIE_MSK, (uint8_t) 1U);

    // Testing TIFR register
    config.interrupt_config.it_comp_match_a = false;
    config.interrupt_config.it_comp_match_b = false;
    config.interrupt_config.it_timer_overflow = false;
    ASSERT_EQ(timer_8_bit_registers_stub.TIFR , 0U);
    timer_8_bit_registers_stub.TIFR |= 0x7; // set 3 first bits to 1
    ret = timer_8_bit_get_interrupt_flags(DT_ID, &config.interrupt_config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.TIFR & OCIEA_MSK, 1U << OCIEA_BIT);
    ASSERT_EQ(timer_8_bit_registers_stub.TIFR & OCIEB_MSK, 1U << OCIEB_BIT);
    ASSERT_EQ(timer_8_bit_registers_stub.TIFR & TOIE_MSK, 1U );
}


TEST_F(Timer8BitFixture, test_timing_configuration_unitary_functions)
{
    auto ret = TIMER_ERROR_OK;
    // Random configuration, with bits set everywhere
    timer_8_bit_compare_output_mode_t received_compare_mode_a;
    timer_8_bit_compare_output_mode_t received_compare_mode_b;
    uint8_t received_ocra_val;
    uint8_t received_ocrb_val;
    uint8_t received_counter_val;
    timer_x_bit_prescaler_selection_t received_prescaler;
    timer_8_bit_waveform_generation_t received_waveform;
    memcpy(&received_compare_mode_a, &config.timing_config.comp_match_a, sizeof(timer_8_bit_compare_output_mode_t));
    memcpy(&received_compare_mode_b, &config.timing_config.comp_match_b, sizeof(timer_8_bit_compare_output_mode_t));
    memcpy(&received_ocra_val, &config.timing_config.ocra_val, sizeof(uint8_t));
    memcpy(&received_ocrb_val, &config.timing_config.ocrb_val, sizeof(uint8_t));
    memcpy(&received_counter_val, &config.timing_config.counter, sizeof(uint8_t));
    memcpy(&received_prescaler, &config.timing_config.prescaler, sizeof(timer_x_bit_prescaler_selection_t));
    memcpy(&received_waveform, &config.timing_config.waveform_mode, sizeof(timer_8_bit_waveform_generation_t));

    config.timing_config.comp_match_a = TIMER8BIT_CMOD_TOGGLE_OCnX;
    config.timing_config.comp_match_b = TIMER8BIT_CMOD_SET_OCnX;
    config.timing_config.ocra_val = 33U;
    config.timing_config.ocrb_val = 156U;
    config.timing_config.prescaler = TIMERxBIT_CLK_PRESCALER_64;
    config.timing_config.waveform_mode = TIMER8BIT_WG_PWM_FAST_FULL_RANGE;

    // Compare match A mode
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRA & COMA_MSK, 0U);
    ret = timer_8_bit_set_compare_match_A(DT_ID, config.timing_config.comp_match_a);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRA & COMA_MSK, config.timing_config.comp_match_a << COMA_BIT);
    // Counter check value can also be read back from registers ...
    ret = timer_8_bit_get_compare_match_A(DT_ID,&received_compare_mode_a);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(0, memcmp(&received_compare_mode_a, &config.timing_config.comp_match_a, sizeof(timer_8_bit_compare_output_mode_t)));

    // Compare match B mode
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRA & COMB_MSK, 0U);
    ret = timer_8_bit_set_compare_match_B(DT_ID, config.timing_config.comp_match_b);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRA & COMB_MSK, config.timing_config.comp_match_b << COMB_BIT);
    // Counter check value can also be read back from registers ...
    ret = timer_8_bit_get_compare_match_B(DT_ID,&received_compare_mode_b);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(0, memcmp(&received_compare_mode_b, &config.timing_config.comp_match_b, sizeof(timer_8_bit_compare_output_mode_t)));

    // Waveform modes
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRA & (WGM0_MSK | WGM1_MSK), 0U);
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRB & (WGM2_MSK), 0U);
    ret = timer_8_bit_set_waveform_generation(DT_ID, config.timing_config.waveform_mode);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRA & (WGM0_MSK | WGM1_MSK), config.timing_config.waveform_mode);
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRB & WGM2_MSK, (config.timing_config.waveform_mode & (1 << 2U)) << 1U);
    // Counter check value can also be read back from registers ...
    ret = timer_8_bit_get_waveform_generation(DT_ID, &received_waveform);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(0, memcmp(&received_waveform, &config.timing_config.waveform_mode, sizeof(timer_8_bit_waveform_generation_t)));


    // Prescaler settings
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRB & CS_MSK, 0U);
    ret = timer_8_bit_set_prescaler(DT_ID, config.timing_config.prescaler);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRB & CS_MSK, config.timing_config.prescaler);
    ret = timer_8_bit_get_prescaler(DT_ID, &received_prescaler);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(0, memcmp(&received_prescaler, &config.timing_config.prescaler, sizeof(timer_x_bit_prescaler_selection_t)));


    // Timer counter main register value
    ASSERT_EQ(timer_8_bit_registers_stub.TCNT, 0U);
    ret = timer_8_bit_set_counter_value(DT_ID, config.timing_config.counter);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.TCNT, config.timing_config.counter);
    ret = timer_8_bit_get_counter_value(DT_ID, &received_counter_val);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(0, memcmp(&received_counter_val, &config.timing_config.counter, sizeof(uint8_t)));


    // OCRA register value
    ASSERT_EQ(timer_8_bit_registers_stub.OCRA, 0U);
    ret = timer_8_bit_set_ocra_register_value(DT_ID, config.timing_config.ocra_val);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.OCRA, config.timing_config.ocra_val);
    ret = timer_8_bit_get_ocra_register_value(DT_ID, &received_ocra_val);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(0, memcmp(&received_ocra_val, &config.timing_config.ocra_val, sizeof(uint8_t)));


    // OCRB register value
    ASSERT_EQ(timer_8_bit_registers_stub.OCRB, 0U);
    ret = timer_8_bit_set_ocrb_register_value(DT_ID, config.timing_config.ocrb_val);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.OCRB, config.timing_config.ocrb_val);
    ret = timer_8_bit_get_ocrb_register_value(DT_ID, &received_ocrb_val);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(0, memcmp(&received_ocrb_val, &config.timing_config.ocrb_val, sizeof(uint8_t)));

}

TEST_F(Timer8BitFixture, test_force_compare_flags)
{
    auto ret = TIMER_ERROR_OK;
    timer_x_bit_force_compare_config_t received_config;
    memcpy(&received_config, &config.force_compare, sizeof(timer_x_bit_force_compare_config_t));

    config.force_compare.force_comp_match_a = true;
    config.force_compare.force_comp_match_b = true;

    ASSERT_EQ(timer_8_bit_registers_stub.TCCRB, 0);
    ret = timer_8_bit_set_force_compare_config(DT_ID, &config.force_compare);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.TCCRB & (FOCA_MSK | FOCB_MSK), FOCA_MSK | FOCB_MSK);

    ret = timer_8_bit_get_force_compare_config(DT_ID, &received_config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(0, memcmp(&config.force_compare, &received_config, sizeof(timer_x_bit_force_compare_config_t)));
}

TEST_F(Timer8BitFixture, test_interrupt_enable_flags)
{
    auto ret = TIMER_ERROR_OK;
    timer_8_bit_interrupt_config_t received_interrupt_config;
    // Copy default state of interrupt config
    memcpy(&received_interrupt_config, &config.interrupt_config, sizeof(timer_8_bit_interrupt_config_t));

    config.interrupt_config.it_comp_match_a = true;
    config.interrupt_config.it_comp_match_b = true;
    config.interrupt_config.it_timer_overflow = true;

    ASSERT_EQ(timer_8_bit_registers_stub.TIMSK, 0);
    ret = timer_8_bit_set_interrupt_config(DT_ID, &config.interrupt_config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(timer_8_bit_registers_stub.TIMSK, OCIEA_MSK | OCIEB_MSK | TOIE_MSK);

    // Compare output to input : shall be exactly matching
    ret = timer_8_bit_get_interrupt_config(DT_ID, &received_interrupt_config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(0, memcmp(&config.interrupt_config, &received_interrupt_config, sizeof(timer_8_bit_interrupt_config_t)));

    // set all interrupt flags to true
    received_interrupt_config.it_comp_match_a = false;
    received_interrupt_config.it_comp_match_b = false;
    received_interrupt_config.it_timer_overflow = false;
    timer_8_bit_registers_stub.TIFR = 0x07;
    ret = timer_8_bit_get_interrupt_flags(DT_ID, &received_interrupt_config);
    ASSERT_EQ(TIMER_ERROR_OK, ret);
    ASSERT_EQ(0, memcmp(&config.interrupt_config, &received_interrupt_config, sizeof(timer_8_bit_interrupt_config_t)));

}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}