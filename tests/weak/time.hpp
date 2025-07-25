#pragma once

#include "platform.hpp"
#include "types.h"

namespace asb::time
{

using namespace brt::types;

static time_us s_localMonotonicTime_us = 0U;
static time_us s_startTime_us          = 0U;

time_us getStartTime_us()
{
  return s_startTime_us;
}

void initStartTime_us()
{
  s_startTime_us = 0U;
}

time_us getMonotonicTime_us()
{
  return s_localMonotonicTime_us;
}

void increaseMonotonicTime_us(time_us const time)
{
  s_localMonotonicTime_us += time;
}

}  // namespace asb::time