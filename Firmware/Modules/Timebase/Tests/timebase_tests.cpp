#include "gtest/gtest.h"

#include <string.h>

#include "config.h"
#include "timebase.h"
#include "timer_8_bit_stub.h"
#include "timer_8_bit_async_stub.h"
#include "timer_16_bit_stub.h"


class TimebaseModuleFixture : public ::testing::Test
{
public:
    void SetUp(void) override
    {
        timer_8_bit_stub_reset();
        timer_8_bit_async_stub_reset();
        timer_16_bit_stub_reset();

        config.cpu_freq = 16'000'000;
        config.timescale = TIMEBASE_TIMESCALE_MILLISECONDS;
        config.timer.type = TIMEBASE_TIMER_16_BIT;
        config.timer.index = 0U;
    }

    void TearDown(void) override
    {

    }
    timebase_config_t config;
};

TEST(timebase_module_tests, test_compute_timer_parameters)
{
    timebase_config_t config;
    config.cpu_freq = 16'000'000;
    config.timescale = TIMEBASE_TIMESCALE_MILLISECONDS;
    config.timer.type = TIMEBASE_TIMER_16_BIT;
    config.timer.index = 0U;

    uint16_t prescaler = 0;
    uint16_t ocr_value = 0;
    uint32_t accumulator = 0;

    timer_16_bit_stub_set_next_parameters(TIMER16BIT_CLK_PRESCALER_1, 15999U, 0U);

    timebase_error_t err = timebase_compute_timer_parameters(&config, &prescaler, &ocr_value, &accumulator);
    ASSERT_EQ(TIMEBASE_ERROR_OK, err);
    ASSERT_EQ(0U, accumulator);
    ASSERT_EQ(prescaler, 1U);
    ASSERT_EQ(ocr_value, 15999U);

    config.timer.type = TIMEBASE_TIMER_8_BIT;
    config.timer.index = 0U;
    timer_8_bit_stub_set_next_parameters(TIMER8BIT_CLK_PRESCALER_64, 250, 3U);
    err = timebase_compute_timer_parameters(&config, &prescaler, &ocr_value, &accumulator);
    ASSERT_EQ(TIMEBASE_ERROR_OK, err);
    ASSERT_EQ(3U, accumulator);
    ASSERT_EQ(prescaler, 64U);
    ASSERT_EQ(ocr_value, 250U);

    config.timer.type = TIMEBASE_TIMER_8_BIT_ASYNC;
    config.timer.index = 0U;
    timer_8_bit_async_stub_set_next_parameters(TIMER8BIT_ASYNC_CLK_PRESCALER_1024, 127, 4U);
    err = timebase_compute_timer_parameters(&config, &prescaler, &ocr_value, &accumulator);
    ASSERT_EQ(TIMEBASE_ERROR_OK, err);
    ASSERT_EQ(4U, accumulator);
    ASSERT_EQ(prescaler, 1024);
    ASSERT_EQ(ocr_value, 127U);
}

TEST(timebase_module_tests, test_wrong_index_error_forwarding)
{
    timebase_config_t config;
    config.cpu_freq = 16'000'000;
    config.timescale = TIMEBASE_TIMESCALE_MILLISECONDS;

    // This index should break execution as this timer driver does not exist (only '0' is declared)
    config.timer.index = 1U;

    config.timer.type = TIMEBASE_TIMER_16_BIT;
    timebase_error_t err = timebase_init(0U, &config);
    ASSERT_EQ(TIMEBASE_ERROR_TIMER_ERROR, err);

    config.timer.type = TIMEBASE_TIMER_8_BIT;
    err = timebase_init(0U, &config);
    ASSERT_EQ(TIMEBASE_ERROR_TIMER_ERROR, err);

    config.timer.type = TIMEBASE_TIMER_8_BIT_ASYNC;
    err = timebase_init(0U, &config);
    ASSERT_EQ(TIMEBASE_ERROR_TIMER_ERROR, err);

    // Repeat this with a wrong index for timebase module this time
    config.timer.index = 0U;
    err = timebase_init(TIMEBASE_MAX_MODULES, &config);
    ASSERT_EQ(TIMEBASE_ERROR_INVALID_INDEX, err);
}

TEST(timebase_module_tests, test_uninitialised_timer_error)
{
    timebase_config_t config;
    config.cpu_freq = 16'000'000;
    config.timescale = TIMEBASE_TIMESCALE_MILLISECONDS;
    config.timer.type = TIMEBASE_TIMER_16_BIT;
    config.timer.index = 0U;

    timebase_error_t err = timebase_init(0U, &config);
    ASSERT_EQ(TIMEBASE_ERROR_TIMER_UNINITIALISED, err);

    config.timer.type = TIMEBASE_TIMER_8_BIT;
    err = timebase_init(0U, &config);
    ASSERT_EQ(TIMEBASE_ERROR_TIMER_UNINITIALISED, err);

    config.timer.type = TIMEBASE_TIMER_8_BIT_ASYNC;
    err = timebase_init(0U, &config);
    ASSERT_EQ(TIMEBASE_ERROR_TIMER_UNINITIALISED, err);
}

TEST_F(TimebaseModuleFixture, test_timer_initialisation)
{
    timer_8_bit_stub_set_initialised(true);
    config.timer.type = TIMEBASE_TIMER_8_BIT;
    config.timer.index = 0U;

    timer_8_bit_prescaler_selection_t prescaler = TIMER8BIT_CLK_PRESCALER_64;
    uint32_t accumulator = 5U;
    uint8_t ocra = 0U;

    timer_8_bit_stub_set_next_parameters(prescaler, ocra, accumulator);

    timebase_error_t err = timebase_init(0U, &config);
    ASSERT_EQ(TIMEBASE_ERROR_OK, err);
    timer_8_bit_config_t driver_config;
    memset(&driver_config, 0, sizeof(timer_8_bit_config_t));

    timer_8_bit_stub_get_driver_configuration(&driver_config);
    ASSERT_EQ(driver_config.timing_config.ocra_val, ocra);
    ASSERT_EQ(driver_config.timing_config.prescaler, prescaler);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}