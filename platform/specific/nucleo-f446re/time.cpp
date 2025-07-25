#include "mbed.h"

#include "../time.hpp"

namespace asb::time
{
using namespace brt::types;

static time_us s_startTime = 0;

time_us getStartTime_us()
{
  return s_startTime;
}

void initStartTime_us()
{
  static bool timeIsSet = false;

  if (!timeIsSet) {
    s_startTime = getMonotonicTime_us();
    timeIsSet   = true;
  }
}

time_us getMonotonicTime_us()
{
  return static_cast<time_us>(us_ticker_read());
}

}  // namespace asb::time
