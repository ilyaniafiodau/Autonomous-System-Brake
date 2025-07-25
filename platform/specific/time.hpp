#pragma once

#include "types.h"

namespace asb
{
namespace time
{

/**
 * @brief Returns the absolute current time of the application.
 *
 * @return brt::types::time_us
 * @retval absolute current time of the application.
 */
brt::types::time_us getMonotonicTime_us();

/**
 * @brief Returns the absolute start time of the application which was previously set using the
 * function getStartTime_us().
 *
 * @return brt::types::time_us
 * @retval absolute start time of the application.
 */
brt::types::time_us getStartTime_us();

/**
 * @brief Sets and saves the absolute start time of the application.
 *
 * @note Should be called once at the begin of the application.
 */
void initStartTime_us();

}  // namespace time
}  // namespace asb