#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "platform.hpp"

#define WEAK_FUNCTION __attribute__((weak))

namespace asb::time
{
using namespace brt::types;

static time_us s_startTime = 0;

// Функция WEAK во время линковки, чтобы переопредилить ее в тестах. На момент написания
// тестов с помощью gtest так и не понял, можно ли замокать обычную функцию (не виртуальный метод),
// поэтому переопределяю ее прямо в том тесте, где нужно.
WEAK_FUNCTION time_us getStartTime_us()
{
  return s_startTime;
}

// Функция WEAK во время линковки, чтобы переопредилить ее в тестах. На момент написания
// тестов с помощью gtest так и не понял, можно ли замокать обычную функцию (не виртуальный метод),
// поэтому переопределяю ее прямо в том тесте, где нужно.
WEAK_FUNCTION void initStartTime_us()
{
  static bool timeIsSet = false;

  if (!timeIsSet) {
    s_startTime = getMonotonicTime_us();
    timeIsSet   = true;
  }
}

// Функция WEAK во время линковки, чтобы переопредилить ее в тестах. На момент написания
// тестов с помощью gtest так и не понял, можно ли замокать обычную функцию (не виртуальный метод),
// поэтому переопределяю ее прямо в том тесте, где нужно.
WEAK_FUNCTION time_us getMonotonicTime_us()
{
  struct timespec ts;

  (void)clock_gettime(CLOCK_MONOTONIC, &ts);

  return static_cast<time_us>(ts.tv_sec * 1000000 + ts.tv_nsec / 1000);
}

}  // namespace asb::time
