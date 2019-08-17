// Tests for the I2C LCD Driver.
#include "L2_HAL/displays/lcd/implementations/Lcd_I2c.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(LCD_I2C);

TEST_CASE("Testing I2C LCD Driver", "[lcd_i2c]")
{
  Mock<sjsu::I2c> mock_i2c;
  Mock<LCD_I2C> mock_lcd;

  sjsu::I2c & test_i2c(mock_i2c.get());
  LCD_I2C & test_lcd(mock_lcd.get());

  const uint8_t address         = 0x27;
  constexpr uint8_t kPosition   = 16;
  constexpr uint8_t kLineNumber = 2;

  SECTION("4-bit Bus Initialization")
  {
    // Stub Initialize to always return successfull
    When(Method(mock_lcd, Initialize)).AlwaysReturn(Status::kSuccess);
    // Execute the method under test
    Status init_status = test_lcd.Initialize();
    // Test that the return type is successfull
    CHECK(Status::kSuccess == init_status);
    Verify(Method(mock_lcd, Initialize)).Once();

    mock_lcd.Reset();

    // Repeat with different behavior
    When(Method(mock_lcd, Initialize)).AlwaysReturn(Status::kNotInitialized);

    init_status = test_lcd.Initialize();
    CHECK(Status::kNotInitialized == init_status);
    Verify(Method(mock_lcd, Initialize)).Once();
  }

  SECTION("Write Byte")
  {
    LCD_I2C::ControlPins_t local_pins[3];

    constexpr uint8_t kCommandByte = 0b1111'1111;
    size_t control_index           = 0;

    When(Method(mock_i2c, Transaction))
        .AlwaysDo([&](I2c::Transaction_t transaction) -> Status {
          memcpy(&local_pins[control_index++].byte,
                 transaction.data_out,
                 transaction.out_length);
          return Status::kSuccess;
        });
    uint8_t kExpected[] = { 0b1111'1000, 0b1111'1100, 0b1111'1000 };

    LCD_I2C lcd = LCD_I2C(lcd::DisplayMode::kOneLine,
                          lcd::FontStyle::kFont5x8,
                          lcd::CursorPosition_t{ kPosition, kLineNumber },
                          test_i2c,
                          address);
    lcd.WriteByte(sjsu::LCD_I2C::RegisterOperation::kCommand, kCommandByte);
    for (size_t i = 0; i < control_index; i++)
    {
      CHECK(local_pins[i].byte == kExpected[i]);
    }
  }
}

}  // namespace sjsu
