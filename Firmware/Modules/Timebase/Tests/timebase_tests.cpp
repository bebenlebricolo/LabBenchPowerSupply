#include "gtest/gtest.h"

#include "config.h"
#include "timebase.h"

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
    timebase_error_t err = timebase_compute_timer_parameters(&config, &prescaler, &ocr_value, &accumulator);
    ASSERT_EQ(TIMEBASE_ERROR_OK, err);
    ASSERT_EQ(0U, accumulator);
    ASSERT_EQ(prescaler, 64U);
    ASSERT_EQ(ocr_value, 249U);

}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}