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
  Fake(Method(mock_lcd, InitializeScreen));
  // Virtual methods must be stubbed
  Fake(Method(mock_lcd, Initialize));
  Fake(Method(mock_lcd, WriteByte));

  SECTION("Initialize Screen")
  {
    // Stub Initialize to always return successfull
    When(Method(mock_lcd, InitializeScreen)).AlwaysReturn(Status::kSuccess);
    // Execute the method under test
    Status init_status = test_lcd.InitializeScreen();
    // Test that the return type is successfull
    CHECK(Status::kSuccess == init_status);
  }

  SECTION("Write Command")
  {
    uint8_t test_data     = 0b1111'1111;
    uint8_t expected_data = 0b1111'1111;
     When(Method(mock_lcd, WriteCommand))
      .AlwaysDo([&](uint8_t data) -> void {
        memcpy(&test_data, &data, sizeof(data));
      });

    test_lcd.WriteCommand(expected_data);

    CHECK(expected_data == test_data);
  }

  SECTION("Write Data") {}
}
}  // namespace sjsu
