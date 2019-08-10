#include <chrono>
#include <cstdint>
#include <iterator>

#include "utility/log.hpp"
#include "utility/time.hpp"

std::chrono::microseconds LinuxUptime()
{
  return std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::system_clock::now().time_since_epoch());
}

int main()
{
  sjsu::SetUptimeFunction(LinuxUptime);
  LOG_INFO("Staring Hello World Application");
  while (true)
  {
    for (uint8_t i = 0; i < 16; i++)
    {
      LOG_INFO("Hello World 0x%X", i);
      sjsu::Delay(500ms);
    }
  }

  return -1;
}
