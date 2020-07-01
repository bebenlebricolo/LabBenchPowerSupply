#include "gtest/gtest.h"
#include "HD44780_lcd.h"
#include "HD44780_lcd_private.h"

#include <cstring>
#include <vector>

// Stubs
#include "i2c.h"
#include "timebase.h"

class LcdScreenTestFixture : public ::testing::Test
{
public:
    hd44780_lcd_config_t config;
    process_commands_sequencer_t * command_sequencer;
    internal_configuration_t * internal_configuration;

    void SetUp() override
    {
        ASSERT_EQ(HD44780_LCD_ERROR_OK, hd44780_lcd_get_default_config(&config));
        get_process_command_sequencer(&command_sequencer);
        get_internal_configuration(&internal_configuration);
    }

    void TearDown() override
    {
        hd44780_lcd_driver_reset();
        command_sequencer = nullptr;
        internal_configuration = nullptr;
    }

    void process_command()
    {
        stub_timings();
        auto error = HD44780_LCD_ERROR_OK;
        auto state = HD44780_LCD_STATE_READY;
        do
        {
            error = hd44780_lcd_process();
            EXPECT_EQ(HD44780_LCD_ERROR_OK, error);
            state = hd44780_lcd_get_state();
        } while (HD44780_LCD_STATE_READY != state);
    }

    void stub_timings()
    {
        timebase_stub_clear();
        uint16_t stubbed_durations[6U] = {40, 1, 5, 1, 2, 1};
        uint16_t stubbed_ticks[12U] = {0,4,12,35,50,76,82,112,123,145,165,175};
        timebase_stub_set_durations(stubbed_durations, 6U);
        timebase_stub_set_times(stubbed_ticks, 12U);
    }

    bool command_sequencer_is_reset()
    {
        bool out = true;
        out &= command_sequencer->sequence.count == 0;
        out &= command_sequencer->sequence.first_pass == true;
        out &= command_sequencer->sequence.lower_bits == false;
        out &= command_sequencer->sequence.pulse_sent == false;
        out &= command_sequencer->sequence.waiting == false;
        return out;
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
    stub_timings();
    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);
    std::vector<uint8_t> sent_data_bytes;
    std::vector<uint8_t> sent_i2c_buffers;

    auto state = hd44780_lcd_get_state();
    do
    {
        error = hd44780_lcd_process();
        sent_data_bytes.push_back(get_data_byte());
        sent_i2c_buffers.push_back(get_i2c_buffer());
        ASSERT_EQ(HD44780_LCD_ERROR_OK, error);
        state = hd44780_lcd_get_state();
    } while (HD44780_LCD_STATE_READY != state);

    ASSERT_EQ(command_sequencer->process_command, process_command_idling);
    ASSERT_TRUE(command_sequencer_is_reset());
}

TEST_F(LcdScreenTestFixture, test_clear_command)
{
    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    stub_timings();
    // Initialise driver and screen
    process_command();
    ASSERT_TRUE(command_sequencer_is_reset());

    error = hd44780_lcd_clear();
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);
    ASSERT_EQ(command_sequencer->process_command, internal_command_clear);
    ASSERT_TRUE(command_sequencer_is_reset());

    process_command();
    ASSERT_EQ(command_sequencer->process_command, process_command_idling);
    ASSERT_TRUE(command_sequencer_is_reset());
}

TEST_F(LcdScreenTestFixture, test_home_command)
{
    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    stub_timings();
    // Initialise driver and screen
    process_command();
    ASSERT_TRUE(command_sequencer_is_reset());

    error = hd44780_lcd_home();
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);
    ASSERT_EQ(command_sequencer->process_command, internal_command_home);
    ASSERT_TRUE(command_sequencer_is_reset());

    process_command();
    ASSERT_EQ(command_sequencer->process_command, process_command_idling);
    ASSERT_TRUE(command_sequencer_is_reset());
}

TEST_F(LcdScreenTestFixture, test_display_on_off)
{
    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    stub_timings();
    // Initialise driver and screen
    process_command();
    ASSERT_TRUE(command_sequencer_is_reset());

    const bool display_enabled = true;
    error = hd44780_lcd_set_display_on_off(display_enabled);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);
    ASSERT_EQ(internal_configuration->display.enabled, display_enabled);
    ASSERT_EQ(command_sequencer->process_command, internal_command_handle_display_controls);
    ASSERT_TRUE(command_sequencer_is_reset());

    process_command();
    ASSERT_EQ(command_sequencer->process_command, process_command_idling);
    ASSERT_TRUE(command_sequencer_is_reset());
}

TEST_F(LcdScreenTestFixture, test_cursor_visible)
{
    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    stub_timings();
    // Initialise driver and screen
    process_command();
    ASSERT_TRUE(command_sequencer_is_reset());

    const bool cursor_visible = true;
    error = hd44780_lcd_set_cursor_visible(cursor_visible);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);
    ASSERT_EQ(internal_configuration->display.cursor_visible, cursor_visible);
    ASSERT_EQ(command_sequencer->process_command, internal_command_handle_display_controls);
    ASSERT_TRUE(command_sequencer_is_reset());

    process_command();
    ASSERT_EQ(command_sequencer->process_command, process_command_idling);
    ASSERT_TRUE(command_sequencer_is_reset());
}

TEST_F(LcdScreenTestFixture, test_blinking_cursor)
{
    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    stub_timings();
    // Initialise driver and screen
    process_command();
    ASSERT_TRUE(command_sequencer_is_reset());

    const bool blinking_cursor = true;
    error = hd44780_lcd_set_blinking_cursor(blinking_cursor);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);
    ASSERT_EQ(internal_configuration->display.cursor_blinking, blinking_cursor);
    ASSERT_EQ(command_sequencer->process_command, internal_command_handle_display_controls);
    ASSERT_TRUE(command_sequencer_is_reset());

    process_command();
    ASSERT_EQ(command_sequencer->process_command, process_command_idling);
    ASSERT_TRUE(command_sequencer_is_reset());
}

TEST_F(LcdScreenTestFixture, test_backlight)
{
    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    stub_timings();
    // Initialise driver and screen
    process_command();
    ASSERT_TRUE(command_sequencer_is_reset());

    const bool backlight_enabled = true;
    error = hd44780_lcd_set_backlight(backlight_enabled);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);
    ASSERT_EQ(internal_configuration->display.backlight, backlight_enabled);
    ASSERT_EQ(command_sequencer->process_command, internal_command_set_backlight);
    ASSERT_TRUE(command_sequencer_is_reset());

    process_command();
    ASSERT_EQ(command_sequencer->process_command, process_command_idling);
    ASSERT_TRUE(command_sequencer_is_reset());
}

TEST_F(LcdScreenTestFixture, test_entry_mode)
{
    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    stub_timings();
    // Initialise driver and screen
    process_command();
    ASSERT_TRUE(command_sequencer_is_reset());

    const hd44780_lcd_entry_mode_t entry_mode = HD44780_LCD_ENTRY_MODE_CURSOR_MOVE_LEFT;
    error = hd44780_lcd_set_entry_mode(entry_mode);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);
    ASSERT_EQ(internal_configuration->display.entry_mode, entry_mode);
    ASSERT_EQ(command_sequencer->process_command, internal_command_set_entry_mode);
    ASSERT_TRUE(command_sequencer_is_reset());

    process_command();
    ASSERT_EQ(command_sequencer->process_command, process_command_idling);
    ASSERT_TRUE(command_sequencer_is_reset());
}

TEST_F(LcdScreenTestFixture, test_move_cursor_to_coord)
{
    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    stub_timings();
    // Initialise driver and screen
    process_command();
    ASSERT_TRUE(command_sequencer_is_reset());

    uint8_t line = 1;
    uint8_t column = 15;
    error = hd44780_lcd_move_cursor_to_coord(line, column);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);
    ASSERT_EQ(command_sequencer->process_command, internal_command_move_cursor_to_coord);
    ASSERT_TRUE(command_sequencer_is_reset());

    process_command();
    ASSERT_EQ(command_sequencer->process_command, process_command_idling);
    ASSERT_TRUE(command_sequencer_is_reset());
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}