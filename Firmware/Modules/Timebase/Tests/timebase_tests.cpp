#include "gtest/gtest.h"

#include <string.h>
#include <limits.h>

#include "config.h"
#include "timebase.h"
#include "timebase_internal.h"
#include "timer_8_bit_stub.h"
#include "timer_8_bit_async_stub.h"
#include "timer_16_bit_stub.h"


class TimebaseModuleBasicConfig : public ::testing::Test
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

class TimebaseModule8BitInitialised : public TimebaseModuleBasicConfig
{
public:
    void SetUp(void)
    {
        TimebaseModuleBasicConfig::SetUp();
        timer_8_bit_stub_set_initialised(true);

        config.timer.type = TIMEBASE_TIMER_8_BIT;
        config.timer.index = 0U;

        timer_8_bit_prescaler_selection_t prescaler = TIMER8BIT_CLK_PRESCALER_64;
        uint32_t accumulator = 5U;
        uint8_t ocra = 0U;

        timer_8_bit_stub_set_next_parameters(prescaler, ocra, accumulator);

        timebase_error_t err = timebase_init(0U, &config);
        ASSERT_EQ(TIMEBASE_ERROR_OK, err);
    }
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
    uint16_t accumulator = 0;

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

TEST(timebase_module_test, test_guard_wrong_parameters)
{
    {
        timebase_config_t config;
        memset(&config, 0, sizeof(timebase_config_t));
        uint16_t * null_prescaler = nullptr;
        uint16_t * null_ocr_value = nullptr;
        uint16_t * null_accumulator = nullptr;
        auto ret = timebase_compute_timer_parameters(&config, null_prescaler, null_ocr_value, null_accumulator);
        ASSERT_EQ(ret, TIMEBASE_ERROR_NULL_POINTER);
        uint16_t prescaler = 0;
        uint16_t accumulator = 0;
        uint16_t ocr_value  = 0;

        // Forcing a wrong timer type
        config.timer.type = TIMEBASE_TIMER_UNDEFINED;
        config.timescale = TIMEBASE_TIMESCALE_MICROSECONDS;
        ret = timebase_compute_timer_parameters(&config, &prescaler, &ocr_value, &accumulator);
        ASSERT_EQ(ret, TIMEBASE_ERROR_UNSUPPORTED_TIMER_TYPE);

        config.timer.type = TIMEBASE_TIMER_16_BIT;
        config.timescale = TIMEBASE_TIMESCALE_UNDEFINED;
        ret = timebase_compute_timer_parameters(&config, &prescaler, &ocr_value, &accumulator);
        ASSERT_EQ(ret, TIMEBASE_ERROR_UNSUPPORTED_TIMESCALE);
    }
    {
        timebase_config_t * null_config = nullptr;
        timebase_config_t config;
        memset(&config, 0, sizeof(timebase_config_t));
        auto ret = timebase_init(0U, null_config);
        ASSERT_EQ(ret, TIMEBASE_ERROR_NULL_POINTER);

        ret = timebase_init(TIMEBASE_MAX_MODULES, &config);
        ASSERT_EQ(ret, TIMEBASE_ERROR_INVALID_INDEX);
    }
    {
        timebase_internal_config[0U].initialised = false;
        bool * null_initialised = nullptr;
        bool initialised = false;
        auto ret = timebase_is_initialised(0U, null_initialised);
        ASSERT_EQ(ret, TIMEBASE_ERROR_NULL_POINTER);

        ret = timebase_is_initialised(TIMEBASE_MAX_MODULES, &initialised);
        ASSERT_EQ(ret, TIMEBASE_ERROR_INVALID_INDEX);
    }
    {
        timebase_internal_config[0U].initialised = true;
        auto ret = timebase_deinit(TIMEBASE_MAX_MODULES);
        ASSERT_EQ(ret, TIMEBASE_ERROR_INVALID_INDEX);

        timebase_internal_config[0U].initialised = false;
        ret = timebase_deinit(0U);
        ASSERT_EQ(ret, TIMEBASE_ERROR_UNINITIALISED);
    }
    {
        uint16_t ticks = 0;
        auto ret = timebase_get_tick(0U, nullptr);
        ASSERT_EQ(ret, TIMEBASE_ERROR_NULL_POINTER);

        ret = timebase_get_tick(TIMEBASE_MAX_MODULES, &ticks);
        ASSERT_EQ(ret, TIMEBASE_ERROR_INVALID_INDEX);

        timebase_internal_config[0U].initialised = false;
        ret = timebase_get_tick(0U, &ticks);
        ASSERT_EQ(ret, TIMEBASE_ERROR_UNINITIALISED);
    }
    {
        auto ret = timebase_get_duration(nullptr, nullptr, nullptr);
        ASSERT_EQ(ret, TIMEBASE_ERROR_NULL_POINTER);
    }
    {
        uint16_t duration = 0;
        uint16_t reference = 0;

        auto ret = timebase_get_duration_now(0U,nullptr, nullptr);
        ASSERT_EQ(ret, TIMEBASE_ERROR_NULL_POINTER);

        ret = timebase_get_duration_now(TIMEBASE_MAX_MODULES, &reference, &duration);
        ASSERT_EQ(ret, TIMEBASE_ERROR_INVALID_INDEX);

        timebase_internal_config[0U].initialised = false;
        ret = timebase_get_duration_now(0U, &reference, &duration);
        ASSERT_EQ(ret, TIMEBASE_ERROR_UNINITIALISED);
    }
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

TEST_F(TimebaseModuleBasicConfig, test_timer_initialisation)
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

TEST_F(TimebaseModule8BitInitialised, test_ticks_and_durations)
{
    uint16_t tick = 0;
    uint16_t duration = 0;
    uint16_t reference = 153U;
    timebase_internal_config[0U].tick = 1652U;

    timebase_error_t err = timebase_get_tick(0U, &tick);
    ASSERT_EQ(err, TIMEBASE_ERROR_OK);
    ASSERT_EQ(tick, timebase_internal_config[0U].tick);

    err = timebase_get_duration(&reference, &tick, &duration);
    ASSERT_EQ(err, TIMEBASE_ERROR_OK);
    ASSERT_EQ(duration, timebase_internal_config[0U].tick - reference);

    err = timebase_get_duration_now(0U, &reference, &duration);
    ASSERT_EQ(err, TIMEBASE_ERROR_OK);
    ASSERT_EQ(duration, timebase_internal_config[0U].tick - reference);

    // Overflowing case
    timebase_internal_config[0U].tick = 356;
    reference = 12563;
    err = timebase_get_duration_now(0U, &reference, &duration);
    ASSERT_EQ(err, TIMEBASE_ERROR_OK);
    ASSERT_EQ(duration,((uint32_t) (timebase_internal_config[0U].tick) + USHRT_MAX) - (uint32_t) reference);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}