#pragma once

namespace sjsu
{
enum class Status
{
  kSuccess,
  kTimedOut,
  kBusError,
  kDeviceNotFound,
  kInvalidSettings,
  kNotImplemented,
  kNotReadyYet,
  kInvalidParameters,
};

constexpr const char * Stringify(Status status)
{
  const char * result = "";
  switch (status)
  {
    case Status::kSuccess: result = "Success"; break;
    case Status::kTimedOut: result = "Timed Out"; break;
    case Status::kBusError: result = "Bus Error"; break;
    case Status::kDeviceNotFound: result = "Device Not Found"; break;
    case Status::kNotImplemented: result = "Not Implemented"; break;
    default: break;
  }
  return result;
}
}  // namespace sjsu
