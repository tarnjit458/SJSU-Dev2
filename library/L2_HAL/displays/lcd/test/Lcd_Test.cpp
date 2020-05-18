// Tests for the LCD Interface Driver.
#include "L2_HAL/displays/lcd/lcd.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
// EMIT_ALL_METHODS(lcd);

TEST_CASE("Testing LCD Interface Driver", "[lcd]")
{
  Mock<lcd> mock_lcd;
  lcd & test_lcd(mock_lcd.get());

  // Virtual methods must be stubbed
  Fake(Method(mock_lcd, Initialize));

  uint8_t test_data = 0;
//*************************************
  When(Method(mock_lcd, WriteByte))
      .AlwaysDo([&](lcd::RegisterOperation, uint8_t data) -> void {
        test_data = data;
      });
//*************************************

  SECTION("Initialize Screen")
  {
    // Stub Initialize to always return successfull
    When(Method(mock_lcd, InitializeScreen)).AlwaysReturn(Status::kSuccess);
    // Execute the method under test
    Status init_status = test_lcd.InitializeScreen();
    // Test that the return type is successfull
    CHECK(Status::kSuccess == init_status);
  }

  SECTION("Clear Screen")
  {
    uint8_t expected_data = util::Value(lcd::Command::kClearDisplay);

    test_lcd.ClearDisplay();

    Verify(Method(mock_lcd, WriteByte));
    CHECK(test_data == expected_data);
  }

  SECTION("Set Display On")
  {
    uint8_t expected_data = (util::Value(lcd::Command::kDisplayControl) |
                             util::Value(lcd::Command::kDisplayOn));

    test_lcd.SetDisplayOn(true);

    Verify(Method(mock_lcd, WriteByte));
    CHECK(test_data == expected_data);

    mock_lcd.Reset();

    When(Method(mock_lcd, WriteByte))
        .AlwaysDo([&](lcd::RegisterOperation, uint8_t data) -> void {
          test_data = data;
        });

    test_lcd.SetDisplayOn(false);
    expected_data = (util::Value(lcd::Command::kDisplayControl) |
                     util::Value(lcd::Command::kDisplayOff));
    Verify(Method(mock_lcd, WriteByte));
    CHECK(test_data == expected_data);
  }

  SECTION("Set Cursor On")
  {
    uint8_t expected_data = (util::Value(lcd::Command::kDisplayControl) |
                             util::Value(lcd::Command::kCursorOn));

    test_lcd.SetCursorOn(true);

    Verify(Method(mock_lcd, WriteByte));
    CHECK(test_data == expected_data);

    mock_lcd.Reset();

    When(Method(mock_lcd, WriteByte))
        .AlwaysDo([&](lcd::RegisterOperation, uint8_t data) -> void {
          memcpy(&test_data, &data, sizeof(data));
        });

    test_lcd.SetCursorOn(false);
    expected_data = (util::Value(lcd::Command::kDisplayControl) |
                     util::Value(lcd::Command::kCursorOff));
    Verify(Method(mock_lcd, WriteByte));
    CHECK(test_data == expected_data);
  }

  SECTION("Set Blink On")
  {
    uint8_t expected_data = (util::Value(lcd::Command::kDisplayControl) |
                             util::Value(lcd::Command::kBlinkOn));

    test_lcd.SetBlinkOn(true);

    Verify(Method(mock_lcd, WriteByte));
    CHECK(test_data == expected_data);

    mock_lcd.Reset();

    When(Method(mock_lcd, WriteByte))
        .AlwaysDo([&](lcd::RegisterOperation, uint8_t data) -> void {
          memcpy(&test_data, &data, sizeof(data));
        });

    test_lcd.SetBlinkOn(false);
    expected_data = (util::Value(lcd::Command::kDisplayControl) |
                     util::Value(lcd::Command::kBlinkOff));
    Verify(Method(mock_lcd, WriteByte));
    CHECK(test_data == expected_data);
  }

  SECTION("Return Home")
  {
    uint8_t expected_data = util::Value(lcd::Command::kReturnHome);

    test_lcd.ReturnHome();

    Verify(Method(mock_lcd, WriteByte));
    CHECK(test_data == expected_data);
  }

  SECTION("Set Cursor Position")
  {
    /*
      0x80 + (position.position ) + row_offset
      0x80 + 4 + 0x40 = 0xC4
    */
    constexpr uint8_t kLineNumber = 1;
    constexpr uint8_t kPosition   = 4;
    // Address should be 0xC4
    constexpr uint8_t kCursorAddress =
        util::Value(lcd::Command::kSetDDRAMAddr) | uint8_t(kPosition + 0x40);
    lcd::CursorPosition_t position{ kPosition, kLineNumber };

    test_lcd.SetCursorPosition(position);

    Verify(Method(mock_lcd, WriteByte));
    CHECK(test_data == kCursorAddress);
    printf("Test Data: %i\n", test_data);
    printf("Cursor Address: %i\n", kCursorAddress);
  }

  SECTION("Display Text")
  {
    const char kText[] = "Test";
    uint8_t test_array[10];
    // lcd::RegisterOperation test_operation;
    size_t control_index = 0;

    constexpr uint8_t kLineNumber = 0;
    constexpr uint8_t kPosition   = 0;
    constexpr uint8_t kCursorAddress =
        util::Value(lcd::Command::kSetDDRAMAddr) | uint8_t(kPosition);
    // lcd::CursorPosition_t position{ kPosition, kLineNumber };

    mock_lcd.Reset();

    When(Method(mock_lcd, WriteByte))
        .AlwaysDo([&](lcd::RegisterOperation, uint8_t data) -> void {
          test_array[control_index++] = data;
        });

    test_lcd.DisplayText(kText);

    // Verify(Method(mock_lcd, WriteByte));
    CHECK(test_array[0] == kCursorAddress);

    for (uint8_t i = 0; i < strlen(kText); i++)
    {
      // Verify(Method(mock_lcd, WriteByte));
      CHECK(uint8_t(test_array[i + 1]) == uint8_t(kText[i]));
      printf("Test Data: %i\n", uint8_t(test_array[i + 1]));
      printf("Text Data: %i\n", uint8_t(kText[i]));
    }
  }
}
}  // namespace sjsu
