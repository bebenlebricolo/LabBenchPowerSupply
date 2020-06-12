#include "gtest/gtest.h"
#include "HD44780_lcd.h"
#include <cstring>

// Stubs
#include "i2c.h"
#include "timebase.h"

TEST(HD44780_lcd_driver_tests, test_initialisation_command)
{
    hd44780_lcd_config_t config;
    memset(&config, 0, sizeof(hd44780_lcd_config_t));

    auto error = hd44780_lcd_get_default_config(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    timebase_stub_clear();
    uint16_t stubbed_durations[6U] = {40, 1, 5, 1, 2, 1};
    uint16_t stubbed_ticks[12U] = {0,4,12,35,50,76,82,112,123,145,165,175};
    timebase_stub_set_durations(stubbed_durations, 6U);
    timebase_stub_set_times(stubbed_ticks, 12U);

    error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    auto state = hd44780_lcd_get_state();
    do
    {
        error = hd44780_lcd_process();
        ASSERT_EQ(HD44780_LCD_ERROR_OK, error);
        state = hd44780_lcd_get_state();
    } while (HD44780_LCD_STATE_READY != state);

}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}