#include "gtest/gtest.h"
#include "HD44780_lcd.h"
#include "HD44780_lcd_private.h"
#include <cstring>

// Stubs
#include "i2c.h"
#include "timebase.h"

class LcdScreenTestFixture : public ::testing::Test
{
public:
    hd44780_lcd_config_t config;

    void SetUp() override
    {
        ASSERT_EQ(HD44780_LCD_ERROR_OK, hd44780_lcd_get_default_config(&config));
    }

    void TearDown() override
    {
        hd44780_lcd_driver_reset();
    }

};

TEST(hd44780_lcd_screen_api_tests, test_default_config)
{
    hd44780_lcd_driver_reset();
    hd44780_lcd_config_t config;
    hd44780_lcd_config_t * nullconfig = nullptr;
    memset(&config, 0, sizeof(hd44780_lcd_config_t));

    auto error = hd44780_lcd_get_default_config(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    error = hd44780_lcd_get_default_config(nullconfig);
    ASSERT_EQ(HD44780_LCD_ERROR_NULL_POINTER, error);

    process_commands_sequencer_t * command_sequencer;
    get_process_command_sequencer(&command_sequencer);
    ASSERT_EQ(command_sequencer->process_command, process_command_idling);
    ASSERT_EQ(get_data_byte(), 0);
    ASSERT_EQ(get_i2c_buffer(), 0);
    ASSERT_EQ(hd44780_lcd_get_last_error(), HD44780_LCD_ERROR_OK);
    ASSERT_EQ(hd44780_lcd_get_state(), HD44780_LCD_STATE_NOT_INITIALISED);
}

TEST_F(LcdScreenTestFixture, test_initialisation_command)
{
    timebase_stub_clear();
    uint16_t stubbed_durations[6U] = {40, 1, 5, 1, 2, 1};
    uint16_t stubbed_ticks[12U] = {0,4,12,35,50,76,82,112,123,145,165,175};
    timebase_stub_set_durations(stubbed_durations, 6U);
    timebase_stub_set_times(stubbed_ticks, 12U);

    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    auto state = hd44780_lcd_get_state();
    do
    {
        error = hd44780_lcd_process();
        (void) i2c_stub_buffer;
        ASSERT_EQ(HD44780_LCD_ERROR_OK, error);
        state = hd44780_lcd_get_state();
    } while (HD44780_LCD_STATE_READY != state);

}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}