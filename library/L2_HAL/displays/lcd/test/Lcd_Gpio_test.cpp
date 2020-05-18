// Tests for the Gpio LCD Driver.
#include "L2_HAL/displays/lcd/implementations/Lcd_Gpio.hpp"
#include "L4_Testing/testing_frameworks.hpp"

namespace sjsu
{
EMIT_ALL_METHODS(LCD_GPIO);

TEST_CASE("Testing Gpio LCD Driver", "[lcd_gpio]")
{
  Mock<Gpio> mock_rs;  // RS:    Register Select
  Mock<Gpio> mock_rw;  // RW:    Read / Write
  Mock<Gpio> mock_e;   // E      Chip Enable
  Mock<Gpio> mock_d7;  // D7-D0: Parallel Data Pins
  Mock<Gpio> mock_d6;
  Mock<Gpio> mock_d5;
  Mock<Gpio> mock_d4;
  Mock<Gpio> mock_d3;
  Mock<Gpio> mock_d2;
  Mock<Gpio> mock_d1;
  Mock<Gpio> mock_d0;

  Fake(Method(mock_rs, SetDirection), Method(mock_rs, Set));
  Fake(Method(mock_rw, SetDirection), Method(mock_rw, Set));
  Fake(Method(mock_e, SetDirection), Method(mock_e, Set));
  Fake(Method(mock_d7, SetDirection), Method(mock_d7, Set));
  Fake(Method(mock_d6, SetDirection), Method(mock_d6, Set));
  Fake(Method(mock_d5, SetDirection), Method(mock_d5, Set));
  Fake(Method(mock_d4, SetDirection), Method(mock_d4, Set));
  Fake(Method(mock_d3, SetDirection), Method(mock_d3, Set));
  Fake(Method(mock_d2, SetDirection), Method(mock_d2, Set));
  Fake(Method(mock_d1, SetDirection), Method(mock_d1, Set));
  Fake(Method(mock_d0, SetDirection), Method(mock_d0, Set));

  LCD_GPIO::ControlPins_t pins = { mock_rs.get(), mock_rw.get(), mock_e.get(),
                                   mock_d7.get(), mock_d6.get(), mock_d5.get(),
                                   mock_d4.get(), mock_d3.get(), mock_d2.get(),
                                   mock_d1.get(), mock_d0.get() };

  constexpr uint8_t kPosition   = 16;
  constexpr uint8_t kLineNumber = 2;

  SECTION("4-bit Bus Initialization")
  {
    LCD_GPIO lcd = LCD_GPIO(lcd::BusMode::kFourBitMode,
                            lcd::DisplayMode::kOneLine,
                            lcd::FontStyle::kFont5x8,
                            lcd::CursorPosition_t{ kPosition, kLineNumber },
                            pins);
    lcd.Initialize();
    Verify(Method(mock_rs, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_rw, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_e, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d7, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d6, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d5, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d4, SetDirection).Using(Gpio::Direction::kOutput));
  }

  SECTION("8-bit Bus Initialization")
  {
    LCD_GPIO lcd = LCD_GPIO(lcd::BusMode::kEightBitMode,
                            lcd::DisplayMode::kTwoLine,
                            lcd::FontStyle::kFont5x8,
                            lcd::CursorPosition_t{ kPosition, kLineNumber },
                            pins);
    lcd.Initialize();
    // All control pins RS, RW, and D7-D0 pin direction should be set to
    // output.
    Verify(Method(mock_rs, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_rw, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_e, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d7, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d6, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d5, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d4, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d3, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d2, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d1, SetDirection).Using(Gpio::Direction::kOutput),
           Method(mock_d0, SetDirection).Using(Gpio::Direction::kOutput))
        .Exactly(1);
  }

  SECTION("Write")
  {
    constexpr uint8_t kCommandByte = 0b1010'1010;
    constexpr uint8_t kDataByte    = static_cast<uint8_t>(~kCommandByte);

    LCD_GPIO lcd = LCD_GPIO(lcd::BusMode::kEightBitMode,
                            lcd::DisplayMode::kTwoLine,
                            lcd::FontStyle::kFont5x8,
                            lcd::CursorPosition_t{ kPosition, kLineNumber },
                            pins);
    lcd.Initialize();

    lcd.Write(lcd::RegisterOperation::kCommand, kCommandByte);
    // pins RS, RW, and D7-D0 should all be set before the Chip Enable
    // falling edge write trigger.
    Verify(Method(mock_e, Set),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(lcd::RegisterOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set).Using(Gpio::State((kCommandByte >> 7) & 0x01)),
           Method(mock_d6, Set).Using(Gpio::State((kCommandByte >> 6) & 0x01)),
           Method(mock_d5, Set).Using(Gpio::State((kCommandByte >> 5) & 0x01)),
           Method(mock_d4, Set).Using(Gpio::State((kCommandByte >> 4) & 0x01)),
           Method(mock_d3, Set).Using(Gpio::State((kCommandByte >> 3) & 0x01)),
           Method(mock_d2, Set).Using(Gpio::State((kCommandByte >> 2) & 0x01)),
           Method(mock_d1, Set).Using(Gpio::State((kCommandByte >> 1) & 0x01)),
           Method(mock_d0, Set).Using(Gpio::State((kCommandByte >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));

    lcd.Write(lcd::RegisterOperation::kData, kDataByte);
    Verify(Method(mock_e, Set),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(
        Method(mock_rs, Set).Using(Gpio::State(lcd::RegisterOperation::kData)),
        Method(mock_rw, Set).Using(Gpio::State::kLow),
        Method(mock_d7, Set).Using(Gpio::State((kDataByte >> 7) & 0x01)),
        Method(mock_d6, Set).Using(Gpio::State((kDataByte >> 6) & 0x01)),
        Method(mock_d5, Set).Using(Gpio::State((kDataByte >> 5) & 0x01)),
        Method(mock_d4, Set).Using(Gpio::State((kDataByte >> 4) & 0x01)),
        Method(mock_d3, Set).Using(Gpio::State((kDataByte >> 3) & 0x01)),
        Method(mock_d2, Set).Using(Gpio::State((kDataByte >> 2) & 0x01)),
        Method(mock_d1, Set).Using(Gpio::State((kDataByte >> 1) & 0x01)),
        Method(mock_d0, Set).Using(Gpio::State((kDataByte >> 0) & 0x01)),
        Method(mock_e, Set).Using(Gpio::State::kLow));
  }

  SECTION("Write Byte")
  {
    constexpr uint8_t kCommandByte = 0b1010'1010;
    constexpr uint8_t kDataByte    = static_cast<uint8_t>(~kCommandByte);

    LCD_GPIO lcd_8bit =
        LCD_GPIO(lcd::BusMode::kEightBitMode,
                 lcd::DisplayMode::kTwoLine,
                 lcd::FontStyle::kFont5x8,
                 lcd::CursorPosition_t{ kPosition, kLineNumber },
                 pins);
    lcd_8bit.Initialize();

    lcd_8bit.WriteByte(lcd::RegisterOperation::kCommand, kCommandByte);
    // pins RS, RW, and D7-D0 should all be set before the Chip Enable
    // falling edge write trigger.
    Verify(Method(mock_e, Set),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(Method(mock_rs, Set)
               .Using(Gpio::State(lcd::RegisterOperation::kCommand)),
           Method(mock_rw, Set).Using(Gpio::State::kLow),
           Method(mock_d7, Set).Using(Gpio::State((kCommandByte >> 7) & 0x01)),
           Method(mock_d6, Set).Using(Gpio::State((kCommandByte >> 6) & 0x01)),
           Method(mock_d5, Set).Using(Gpio::State((kCommandByte >> 5) & 0x01)),
           Method(mock_d4, Set).Using(Gpio::State((kCommandByte >> 4) & 0x01)),
           Method(mock_d3, Set).Using(Gpio::State((kCommandByte >> 3) & 0x01)),
           Method(mock_d2, Set).Using(Gpio::State((kCommandByte >> 2) & 0x01)),
           Method(mock_d1, Set).Using(Gpio::State((kCommandByte >> 1) & 0x01)),
           Method(mock_d0, Set).Using(Gpio::State((kCommandByte >> 0) & 0x01)),
           Method(mock_e, Set).Using(Gpio::State::kLow));

    LCD_GPIO lcd_4bit =
        LCD_GPIO(lcd::BusMode::kFourBitMode,
                 lcd::DisplayMode::kTwoLine,
                 lcd::FontStyle::kFont5x8,
                 lcd::CursorPosition_t{ kPosition, kLineNumber },
                 pins);
    lcd_4bit.Initialize();

    lcd_4bit.WriteByte(lcd::RegisterOperation::kData, kDataByte);
    Verify(Method(mock_e, Set),
           Method(mock_e, Set).Using(Gpio::State::kLow),
           Method(mock_e, Set));
    Verify(
        Method(mock_rs, Set).Using(Gpio::State(lcd::RegisterOperation::kData)),
        Method(mock_rw, Set).Using(Gpio::State::kLow),
        Method(mock_d7, Set).Using(Gpio::State((kDataByte >> 7) & 0x01)),
        Method(mock_d6, Set).Using(Gpio::State((kDataByte >> 6) & 0x01)),
        Method(mock_d5, Set).Using(Gpio::State((kDataByte >> 5) & 0x01)),
        Method(mock_d4, Set).Using(Gpio::State((kDataByte >> 4) & 0x01)),
        Method(mock_e, Set).Using(Gpio::State::kLow),
        Method(mock_rs, Set).Using(Gpio::State(lcd::RegisterOperation::kData)),
        Method(mock_rw, Set).Using(Gpio::State::kLow),
        Method(mock_d7, Set).Using(Gpio::State((kDataByte >> 3) & 0x01)),
        Method(mock_d6, Set).Using(Gpio::State((kDataByte >> 2) & 0x01)),
        Method(mock_d5, Set).Using(Gpio::State((kDataByte >> 1) & 0x01)),
        Method(mock_d4, Set).Using(Gpio::State((kDataByte >> 0) & 0x01)),
        Method(mock_e, Set).Using(Gpio::State::kLow));
  }
}
}  // namespace sjsu