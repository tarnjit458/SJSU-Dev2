#pragma once

#include <cstdint>
#include <cstring>
#include "utility/enum.hpp"
#include "utility/units.hpp"
#include "utility/time.hpp"
#include "utility/status.hpp"

namespace sjsu
{
class lcd
{
 public:
  // Data transfer operation types.
  enum class RegisterOperation : uint8_t
  {
    kCommand = 0,
    kData    = 1
  };

  enum class WriteOperation : uint8_t
  {
    kWrite = 0,
    kRead  = 1
  };

  enum class Command : uint8_t
  {
    // Commands
    kClearDisplay   = 0x01,
    kReturnHome     = 0x02,
    kEntryModeSet   = 0x04,
    kDisplayControl = 0x08,
    kCursorShift    = 0x10,
    kFunctionSet    = 0x20,
    kSetCGRAMAddr   = 0x40,
    kSetDDRAMAddr   = 0x80,

    // Display Entry Mode
    kEntryRight          = 0x00,
    kEntryLeft           = 0x02,
    kEntryShiftIncrement = 0x01,
    kEntryShiftDecrement = 0x00,

    // Display On/Off control
    kDisplayOn  = 0x04,
    kDisplayOff = 0x00,
    kCursorOn   = 0x02,
    kCursorOff  = 0x00,
    kBlinkOn    = 0x01,
    kBlinkOff   = 0x00,

    // Display/Cursor shift
    kDisplayMove = 0x08,
    kCursorMove  = 0x00,
    kMoveRight   = 0x04,
    kMoveLeft    = 0x00,

    // BackLight Control
    kBacklight   = 0x08,
    kNoBackLight = 0x00,
  };

  struct CursorPosition_t
  {
    // Character column position
    uint8_t position;
    // Display line number starting at 0 for the first line.
    uint8_t line_number;
  };

  enum class BusMode : uint8_t
  {
    kFourBitMode  = 0 << 4,
    kEightBitMode = 1 << 4
  };

  enum class DisplayMode : uint8_t
  {
    kOneLine = 0 << 3,
    kTwoLine = 1 << 3
  };

  enum class FontStyle : uint8_t
  {
    kFont5x8  = 0 << 2,
    kFont5x10 = 1 << 2
  };

  // Default cursor position at position 0, line 0.
  static constexpr CursorPosition_t kDefaultCursorPosition =
      CursorPosition_t{ 0, 0 };

  // Constructor for the display driver with desired configurations.
  //
  // @param bus_mode     4-bit or 8-bit data transfer bus mode.
  // @param display_mode Number of lines used for displaying characters.
  // @param font_style   Character font style.
  // @param position     Display dimensions.
  lcd(CursorPosition_t position)
      : _cols(position.position),
        _rows(position.line_number)
  {
  }
  virtual Status Initialize()                                       = 0;
  virtual void WriteByte(RegisterOperation operation, uint8_t byte) = 0;

  Status InitializeScreen()
  {
    // Final function set bus mode, display mode and font style
    WriteCommand(uint8_t(Command::kFunctionSet) | _displaysetting);

    SetDisplayOn();
    ClearDisplay();
    //ReturnHome();
    return Status::kSuccess;
  }

  // Clears all characters on the display by sending the clear display command
  // to the device.
  void ClearDisplay()
  {
    WriteCommand(util::Value(Command::kClearDisplay));
    Delay(2ms);  // Clear display operation requires 2ms
  }

  // @param on Toggles the display on if true.
  void SetDisplayOn(bool on = true)
  {
    WriteCommand(on ? util::Value(Command::kDisplayControl) |
                          util::Value(Command::kDisplayOn)
                    : util::Value(Command::kDisplayControl) |
                          util::Value(Command::kDisplayOff));
  }

  // @param on Toggles the cursor on if true.
  void SetCursorOn(bool on = true)
  {
    WriteCommand(on ? util::Value(Command::kDisplayControl) |
                          util::Value(Command::kCursorOn)
                    : util::Value(Command::kDisplayControl) |
                          util::Value(Command::kCursorOff));
  }

  // @param on Toggles the cursor blink on if true.
  void SetBlinkOn(bool on = true)
  {
    WriteCommand(on ? util::Value(Command::kDisplayControl) |
                          util::Value(Command::kBlinkOn)
                    : util::Value(Command::kDisplayControl) |
                          util::Value(Command::kBlinkOff));
  }

  // Returns the display to the beginning position.
  void ReturnHome()
  {
    WriteCommand(util::Value(Command::kReturnHome));
    Delay(2s);
  }

  // Sets the cursor at a specified position.
  //
  // @param line Line number to move cursor to.
  // @param pos  Character position to move cursor to.
  void SetCursorPosition(CursorPosition_t position)
  {
    const uint8_t kMaxLineNumbers = _rows;
    const uint8_t row_offsets[]   = {
      0x00, 0x40, 0x14, 0x54
    };  // according to data manual
    if (position.line_number > kMaxLineNumbers)
    {
      position.line_number = uint8_t(kMaxLineNumbers - 1);
    }
    WriteCommand(
        util::Value(Command::kSetDDRAMAddr) |
        uint8_t(position.position + row_offsets[position.line_number]));
  }

  // Displays a desired text string on the display.
  //
  // @param text     String to write on the display.
  // @param position Position to start writing the characters
  void DisplayText(const char * text,
                   CursorPosition_t position = kDefaultCursorPosition)
  {
    const uint8_t kMaxDisplayWidth = _cols;
    const uint8_t kStartOffset     = position.position;
    uint8_t termination_index      = uint8_t(strlen(text));
    // calculate the termination_index to stop writing to the display
    // if the string length of text exceeds the display's width
    if (termination_index > kMaxDisplayWidth)
    {
      termination_index = kMaxDisplayWidth;
    }
    if (termination_index > (kMaxDisplayWidth - kStartOffset))
    {
      termination_index = uint8_t(kMaxDisplayWidth - kStartOffset);
    }
    // set cursor start position for writing
    SetCursorPosition(position);
    for (uint8_t i = 0; i < termination_index; i++)
    {
      WriteData(text[i]);
    }
  }

 private:
  // Writes a command byte to the current cursor address position.
  //
  // @param command Byte to send to device.
  void WriteCommand(uint8_t command)
  {
    WriteByte(RegisterOperation::kCommand, command);
  }

  // Writes a data byte to the current cursor address position.
  //
  // @param data Byte to send to device.
  void WriteData(uint8_t data)
  {
    WriteByte(RegisterOperation::kData, data);
  }

 protected:
 /* const BusMode kBusMode;
  const DisplayMode kDisplayMode;
  const FontStyle kFontStyle;*/
  uint8_t _displaysetting;
  const uint8_t _cols;
  const uint8_t _rows;
};
}  // namespace sjsu
