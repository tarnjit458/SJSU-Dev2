#pragma once

#include "L1_Peripheral/lpc40xx/i2c.hpp"
#include "L2_HAL/displays/lcd/lcd.hpp"

#include "utility/units.hpp"

namespace sjsu
{
class LCD_I2C : public lcd
{
 public:
  // Byte format
  typedef union {
    uint8_t byte;
    struct
    {
      uint8_t rs : 1;
      uint8_t rw : 1;
      uint8_t e : 1;
      uint8_t bl : 1;
      uint8_t d4 : 1;
      uint8_t d5 : 1;
      uint8_t d6 : 1;
      uint8_t d7 : 1;
    } __attribute__((packed));
  } ControlPins_t;

  // Constructor for the display driver with desired configurations.
  //
  // @param display_mode Number of lines used for displaying characters.
  // @param font_style   Character font style
  // @param position     Display dimensions (16x2).
  // @param i2c     	   I2C reference for controlling the I2C bus.
  // @param address      The address of the display for communication.
  LCD_I2C(DisplayMode display_mode,
          FontStyle font_style,
          I2c & i2c,
          uint8_t address)
      : i2c_(i2c),
        addr_(address),
        kDisplayMode(display_mode),
        kFontStyle(font_style)
  {
  }

  Status Initialize() override
  {
    Status init_status = i2c_.Initialize();
    i2c_.Initialize();
    _displaysetting = (util::Value(kBusMode) | util::Value(kDisplayMode) |
                       util::Value(kFontStyle));

    // Initialization sequence according to the data manual
    Delay(1s);
    // Function set 8 bit mode
    Write(RegisterOperation::kCommand, (0x03 << 4));
    Delay(5ms);
    // Function set 8 bit mode
    Write(RegisterOperation::kCommand, (0x03 << 4));
    Delay(5ms);
    // Function set 8 bit mode
    Write(RegisterOperation::kCommand, (0x03 << 4));
    Delay(5ms);
    // Function set: set interface to 4 bits
    Write(RegisterOperation::kCommand, (0x02 << 4));
    Delay(5ms);

    InitializeScreen();
    return init_status;
  }
  // Handles the write operation
  void Write(RegisterOperation operation, uint8_t byte) override
  {
    ControlPins_t pin;
    pin.byte = byte;
    pin.rw   = 0;
    pin.e    = 0;
    pin.bl   = 1;

    // Checks the Register Operation type
    if (operation == RegisterOperation::kData)
    {
      pin.rs = 1;
    }
    else
    {
      pin.rs = 0;
    }

    byte = pin.byte;

    Delay(1ms);
    i2c_.Write(addr_, &byte, sizeof(byte));
    PulseEnable(byte);
  }
  // Handles the write operation for the 4-bit operation
  void WriteByte(RegisterOperation operation, uint8_t byte) override
  {
    Write(operation, static_cast<uint8_t>((byte << 0) & 0xf0));
    Write(operation, static_cast<uint8_t>((byte << 4) & 0xf0));
  }

  // Pulses the enable high then low for writing
  void PulseEnable(uint8_t data)
  {
    ControlPins_t pin;
    pin.byte = data;

    pin.e = 1;
    data  = pin.byte;
    i2c_.Write(addr_, &data, sizeof(data));
    Delay(1ms);

    pin.e = 0;
    data  = pin.byte;
    i2c_.Write(addr_, &data, sizeof(data));
    Delay(50ms);
  }

 private:
  const I2c & i2c_;
  const uint8_t addr_;
  const BusMode kBusMode =
      BusMode::kFourBitMode;  // I2C backpack has only 8 IO pins therefore we
                              // will only use 4 bit mode
  const DisplayMode kDisplayMode;
  const FontStyle kFontStyle;
};
}  // namespace sjsu