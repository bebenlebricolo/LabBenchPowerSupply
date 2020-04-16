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
        config.force_compare.force_comp_match_a = false;
        config.force_compare.force_comp_match_b = false;
        config.interrupt_config.it_comp_match_a = false;
        config.interrupt_config.it_comp_match_b = false;
        config.interrupt_config.it_timer_overflow = false;
        config.timing_config.comp_match_a = TIMER8BIT_CMOD_NORMAL;
        config.timing_config.comp_match_b = TIMER8BIT_CMOD_NORMAL;
        config.timing_config.prescaler = TIMERxBIT_CLK_NO_CLOCK;
        config.timing_config.waveform_mode = TIMER8BIT_WG_NORMAL;
        timer_8_bit_registers_stub_init_handle(&config.handle);
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


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}