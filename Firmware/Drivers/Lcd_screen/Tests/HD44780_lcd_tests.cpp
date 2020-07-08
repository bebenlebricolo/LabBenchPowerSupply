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
        sent_data_bytes.clear();
        sent_i2c_buffers.clear();

        stub_timings();
        auto error = HD44780_LCD_ERROR_OK;
        auto state = HD44780_LCD_STATE_READY;
        do
        {
            error = hd44780_lcd_process();
            sent_data_bytes.push_back(get_data_byte());

            uint8_t current_i2c_buffer = 0;
            bool is_new_buffer = false;
            (void) i2c_stub_get_buffer_content(0, &current_i2c_buffer, &is_new_buffer);
            if (is_new_buffer)
            {
                sent_i2c_buffers.push_back(current_i2c_buffer);
            }

            EXPECT_EQ(HD44780_LCD_ERROR_OK, error);
            state = hd44780_lcd_get_state();
        } while (HD44780_LCD_STATE_READY != state);

        filtered_data_bytes_vect = remove_adjacent_duplicates(sent_data_bytes);
        filtered_i2c_buffers_vect = remove_adjacent_duplicates(sent_i2c_buffers);
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

    template <typename T>
    std::vector<T> remove_adjacent_duplicates(const std::vector<T>& vector)
    {
        std::vector<T> out;
        T prev = vector[0];
        out.push_back(prev);
        for (uint8_t i = 1 ; i < vector.size() ; i++)
        {
            if(vector[i] != prev)
            {
                out.push_back(vector[i]);
                prev = vector[i];
            }
        }
        return out;
    }

    std::vector<uint8_t> sent_data_bytes;
    std::vector<uint8_t> sent_i2c_buffers;
    std::vector<uint8_t> filtered_data_bytes_vect;
    std::vector<uint8_t> filtered_i2c_buffers_vect;

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

TEST_F(LcdScreenTestFixture, test_byte_handling)
{
    const uint8_t target_data_byte = 0x28;
    const uint8_t expected_sent_data[4U] =
    {
        (uint8_t) ((target_data_byte & 0xF0) | 0xc),
        (uint8_t) ((target_data_byte & 0xF0) | 0x8),
        (uint8_t) (((target_data_byte & 0x0F) << 4) | 0xc),
        (uint8_t) (((target_data_byte & 0x0F) << 4)| 0x8)
    };

    internal_configuration->display.backlight = true;
    command_sequencer->sequence.count = 0;
    command_sequencer->sequence.first_pass = true;
    command_sequencer->sequence.lower_bits = false;
    command_sequencer->sequence.pulse_sent = false;
    command_sequencer->sequence.waiting = false;

    stub_timings();
    set_data_byte(target_data_byte);
    sent_i2c_buffers.clear();
    bool byte_sent = false;
    prepare_i2c_buffer(TRANSMISSION_MODE_INSTRUCTION);

    while (!byte_sent)
    {
        byte_sent = handle_byte_sending();
        uint8_t value = 0;
        bool is_new = false;
        i2c_stub_get_buffer_content(0, &value, &is_new);
        if (is_new)
        {
            sent_i2c_buffers.push_back(value);
        }
    }

    ASSERT_EQ(sent_i2c_buffers.size(), 4U);

    // Sequence count incrementation shall be handled by the caller
    ASSERT_EQ(command_sequencer->sequence.count, 0U);
    for (uint8_t i = 0; i < 4U ; i++)
    {
        EXPECT_EQ(sent_i2c_buffers[i], expected_sent_data[i]);
    }
}

TEST_F(LcdScreenTestFixture, test_initialisation_command)
{
    stub_timings();
    auto error = hd44780_lcd_init(&config);
    const std::vector<uint8_t> expected_sent_i2c_buffers =
    {
        0x3C, 0x38, // Initialisation sequence, 4 bits sent only
        0x3C, 0x38, // Initialisation sequence, 4 bits sent only
        0x3C, 0x38, // Initialisation sequence, 4 bits sent only

        0x2C, 0x28, // Setting 4 bits mode, 4 bits sent only

        0x2C, 0x28, // Setting font and data length
        0x8C, 0x88, // 2 lines mode, small font

        0x0c, 0x08, // Handles display controls
        0xbc, 0xb8, // display off, cursor visible, cursor blinking

        0x0c, 0x08, // Handles display clear
        0x1c, 0x18,

        0x0c, 0x08, // Handles entry mode set
        0x6c, 0x68  // cursor move to right, no display shift
    };

    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    process_command();

    ASSERT_EQ(command_sequencer->process_command, process_command_idling);
    ASSERT_TRUE(command_sequencer_is_reset());

    // Counter check sent data matches the data we want to actually send
    ASSERT_EQ(sent_i2c_buffers.size(), expected_sent_i2c_buffers.size());
    for (uint8_t i = 0 ; i < sent_i2c_buffers.size() ; i++)
    {
        EXPECT_EQ(sent_i2c_buffers[i], expected_sent_i2c_buffers[i]);
    }
}

TEST_F(LcdScreenTestFixture, test_clear_command)
{
    const uint8_t expected_sent_data[4U] =
    {
        0x0c,
        0x08,
        0x1c,
        0x18
    };
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

    // Counter check sent data matches the data we want to actually send
    ASSERT_EQ(sent_i2c_buffers.size(), 4U);
    for (uint8_t i = 0 ; i < 4U ; i++)
    {
        EXPECT_EQ(sent_i2c_buffers[i], expected_sent_data[i]);
    }
}

TEST_F(LcdScreenTestFixture, test_home_command)
{
    const uint8_t expected_sent_data[4U] =
    {
        0x0c,
        0x08,
        0x2c,
        0x28
    };

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

    // Counter check sent data matches the data we want to actually send
    ASSERT_EQ(sent_i2c_buffers.size(), 4U);
    for (uint8_t i = 0 ; i < 4U ; i++)
    {
        EXPECT_EQ(sent_i2c_buffers[i], expected_sent_data[i]);
    }
}

TEST_F(LcdScreenTestFixture, test_display_on_off)
{
    const uint8_t expected_sent_data[4U] =
    {
        0x0c,
        0x08,
        0xcc,
        0xc8
    };
    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    internal_configuration->display.backlight = true;
    internal_configuration->display.cursor_blinking = false;
    internal_configuration->display.cursor_visible = false;
    internal_configuration->display.enabled = true;

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

    // Counter check sent data matches the data we want to actually send
    ASSERT_EQ(sent_i2c_buffers.size(), 4U);
    for (uint8_t i = 0 ; i < 4U ; i++)
    {
        EXPECT_EQ(sent_i2c_buffers[i], expected_sent_data[i]);
    }
}

TEST_F(LcdScreenTestFixture, test_cursor_visible)
{
    const uint8_t expected_sent_data[4U] =
    {
        0x0c,
        0x08,
        0xac,
        0xa8
    };
    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    internal_configuration->display.backlight = true;
    internal_configuration->display.cursor_blinking = false;
    internal_configuration->display.cursor_visible = false;
    internal_configuration->display.enabled = true;

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

    // Counter check sent data matches the data we want to actually send
    ASSERT_EQ(sent_i2c_buffers.size(), 4U);
    for (uint8_t i = 0 ; i < 4U ; i++)
    {
        EXPECT_EQ(sent_i2c_buffers[i], expected_sent_data[i]);
    }
}

TEST_F(LcdScreenTestFixture, test_blinking_cursor)
{
    const uint8_t expected_sent_data[4U] =
    {
        0x0c,
        0x08,
        0x9c,
        0x98
    };
    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    internal_configuration->display.backlight = true;
    internal_configuration->display.cursor_blinking = false;
    internal_configuration->display.cursor_visible = false;
    internal_configuration->display.enabled = true;

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

    // Counter check sent data matches the data we want to actually send
    ASSERT_EQ(sent_i2c_buffers.size(), 4U);
    for (uint8_t i = 0 ; i < 4U ; i++)
    {
        EXPECT_EQ(sent_i2c_buffers[i], expected_sent_data[i]);
    }
}

TEST_F(LcdScreenTestFixture, test_backlight)
{
    auto error = hd44780_lcd_init(&config);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);

    stub_timings();
    // Initialise driver and screen
    process_command();
    ASSERT_TRUE(command_sequencer_is_reset());

    const bool backlight_enabled = false;
    error = hd44780_lcd_set_backlight(backlight_enabled);
    ASSERT_EQ(HD44780_LCD_ERROR_OK, error);
    ASSERT_EQ(internal_configuration->display.backlight, backlight_enabled);
    ASSERT_EQ(command_sequencer->process_command, internal_command_set_backlight);
    ASSERT_TRUE(command_sequencer_is_reset());

    // We initialise those variables here because the set_backlight function needs to preserve
    // the old (previous) state of I2C_buffer
    const uint8_t previous_buffer = get_i2c_buffer();
    const uint8_t expected_sent_data = (previous_buffer & 0xF7);

    process_command();
    ASSERT_EQ(command_sequencer->process_command, process_command_idling);
    ASSERT_TRUE(command_sequencer_is_reset());

    // Counter check sent data matches the data we want to actually send
    ASSERT_EQ(sent_i2c_buffers.size(), 1U);
    ASSERT_EQ(sent_i2c_buffers[0], expected_sent_data);
}

TEST_F(LcdScreenTestFixture, test_entry_mode)
{
    const uint8_t expected_sent_data[4U] =
    {
        0x0c,
        0x08,
        0x4c,
        0x48
    };
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

    // Counter check sent data matches the data we want to actually send
    ASSERT_EQ(sent_i2c_buffers.size(), 4U);
    for (uint8_t i = 0 ; i < 4U ; i++)
    {
        EXPECT_EQ(sent_i2c_buffers[i], expected_sent_data[i]);
    }
}

TEST_F(LcdScreenTestFixture, test_move_cursor_to_coord)
{
    const uint8_t expected_sent_data[4U] =
    {
        0xbc,   // 55(10) -> 0x37 (16) ; cmd code : 0x8 + 0x3 -> 0xb
        0xb8,
        0x7c,
        0x78
    };

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

    // Counter check sent data matches the data we want to actually send
    ASSERT_EQ(sent_i2c_buffers.size(), 4U);
    for (uint8_t i = 0 ; i < 4U ; i++)
    {
        EXPECT_EQ(sent_i2c_buffers[i], expected_sent_data[i]);
    }
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}