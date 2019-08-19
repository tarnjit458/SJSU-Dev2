#pragma once

#include "L1_Peripheral/gpio.hpp"
#include "L2_HAL/displays/lcd/lcd.hpp"

namespace sjsu
{
class LCD_GPIO : public lcd
{
 public:
  struct ControlPins_t
  {
    sjsu::Gpio & rs;  // Register Selection
    sjsu::Gpio & rw;  // Read/Write
    sjsu::Gpio & e;   // Chip Enable
    sjsu::Gpio & d7;
    sjsu::Gpio & d6;
    sjsu::Gpio & d5;
    sjsu::Gpio & d4;
    sjsu::Gpio & d3;
    sjsu::Gpio & d2;
    sjsu::Gpio & d1;
    sjsu::Gpio & d0;
  };

  // Constructor for the display driver with desired configurations.
  //
  // @param bus_mode     4-bit or 8-bit data transfer bus mode.
  // @param display_mode Number of lines used for displaying characters.
  // @param font_style   Character font style.
  // @param position     Display dimensions (16x2).
  // @param pins         GPIO control pins for controlling the display.
  LCD_GPIO(BusMode bus_mode,
           DisplayMode display_mode,
           FontStyle font_style,
           CursorPosition_t position,
           const ControlPins_t & pins)
      : lcd(position),
        kBusMode(bus_mode),
        kDisplayMode(display_mode),
        kFontStyle(font_style),
        kControlPins(pins)
  {
  }

  Status Initialize() override
  {
    _displaysetting = (util::Value(kBusMode) | util::Value(kDisplayMode) |
                       util::Value(kFontStyle));
    kControlPins.rs.SetDirection(sjsu::Gpio::Direction::kOutput);
    kControlPins.rw.SetDirection(sjsu::Gpio::Direction::kOutput);
    kControlPins.e.SetDirection(sjsu::Gpio::Direction::kOutput);
    kControlPins.e.Set(sjsu::Gpio::kHigh);
    kControlPins.d7.SetDirection(sjsu::Gpio::Direction::kOutput);
    kControlPins.d6.SetDirection(sjsu::Gpio::Direction::kOutput);
    kControlPins.d5.SetDirection(sjsu::Gpio::Direction::kOutput);
    kControlPins.d4.SetDirection(sjsu::Gpio::Direction::kOutput);

    if (kBusMode == BusMode::kEightBitMode)
    {
      kControlPins.d3.SetDirection(sjsu::Gpio::Direction::kOutput);
      kControlPins.d2.SetDirection(sjsu::Gpio::Direction::kOutput);
      kControlPins.d1.SetDirection(sjsu::Gpio::Direction::kOutput);
      kControlPins.d0.SetDirection(sjsu::Gpio::Direction::kOutput);
    }

    InitializeScreen();
    return Status::kSuccess;
  }
  void Write(RegisterOperation operation, uint8_t byte)
  {
    kControlPins.e.Set(sjsu::Gpio::State::kHigh);
    kControlPins.rs.Set(sjsu::Gpio::State(operation));
    kControlPins.rw.Set(sjsu::Gpio::State::kLow);
    // set byte on 8-bit data bus
    if (kBusMode == BusMode::kEightBitMode)
    {
      kControlPins.d7.Set(sjsu::Gpio::State((byte >> 7) & 0x01));
      kControlPins.d6.Set(sjsu::Gpio::State((byte >> 6) & 0x01));
      kControlPins.d5.Set(sjsu::Gpio::State((byte >> 5) & 0x01));
      kControlPins.d4.Set(sjsu::Gpio::State((byte >> 4) & 0x01));
      kControlPins.d3.Set(sjsu::Gpio::State((byte >> 3) & 0x01));
      kControlPins.d2.Set(sjsu::Gpio::State((byte >> 2) & 0x01));
      kControlPins.d1.Set(sjsu::Gpio::State((byte >> 1) & 0x01));
      kControlPins.d0.Set(sjsu::Gpio::State((byte >> 0) & 0x01));
    }
    else
    {
      kControlPins.d7.Set(sjsu::Gpio::State((byte >> 3) & 0x01));
      printf("Bit 7: %i\n", ((byte >> 3) & 0x01));
      kControlPins.d6.Set(sjsu::Gpio::State((byte >> 2) & 0x01));
      printf("Bit 6: %i\n", ((byte >> 2) & 0x01));
      kControlPins.d5.Set(sjsu::Gpio::State((byte >> 1) & 0x01));
      printf("Bit 5: %i\n", ((byte >> 1) & 0x01));
      kControlPins.d4.Set(sjsu::Gpio::State((byte >> 0) & 0x01));
      printf("Bit 4: %i\n", ((byte >> 0) & 0x01));      
    }
    Delay(2ms);
    // Toggle chip enable to trigger write on falling edge
    kControlPins.e.Set(sjsu::Gpio::State::kLow);
    kControlPins.e.Set(sjsu::Gpio::State::kHigh);
  }

  void WriteByte(RegisterOperation operation, uint8_t byte) override
  {
    // uint8_t highnib, lownib = 0;
    switch (kBusMode)
    {
      case BusMode::kFourBitMode:
        // highnib = (byte & 0xf0);
        // lownib  = uint8_t((byte << 4) & 0xf0);
        printf("Highnib: %i\n", ((byte >> 4) & 0x0F));
        Write(operation, (byte >> 4) & 0x0F);
        printf("Lownib: %i\n", ((byte >> 0) & 0x0F));
        Write(operation, (byte >> 0) & 0x0F);
        break;
      case BusMode::kEightBitMode: Write(operation, byte); break;
    }
  }

 private:
  const BusMode kBusMode;
  const DisplayMode kDisplayMode;
  const FontStyle kFontStyle;
  const ControlPins_t & kControlPins;
};
}  // namespace sjsu