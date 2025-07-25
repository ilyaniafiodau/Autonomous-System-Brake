#pragma once

#include "types.h"

namespace asb
{
namespace db
{
/*! \ingroup Database
 *  \brief Each database variable can be written only by a single task. This enum defines all tasks;
 * its values are used to access the database data with write permissions
 */
enum class eTaskID : uint16_t
{
  /* Service IDs */
  UNDEFINED = 0U,
  SCHEDULER = 1U,
  FSM       = 2U,

  /* Valid IDs */
  INIT     = 10U,
  RESOLVER = 11U,
  BRAKE_LIGHT = 12U,
  MONITORING = 13U,
  SUPERVISOR = 14U
};

}  // namespace db
}  // namespace asb
