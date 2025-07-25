#include "types.h"

#include "monitoring_task.hpp"

#include "platform.hpp"

#include "rte/generated/rte.hpp"

namespace asb
{
namespace monitoring
{
using namespace brt::types;

ITask *createTask()
{
  static MonitoringIO monitoringIoPtr;
  static MonitoringTask task(&monitoringIoPtr);
  return static_cast<ITask *>(&task);
}

MonitoringTask::MonitoringTask(MonitoringIO *const monitoringIoPtr)
: ITask(asb::db::eTaskID::MONITORING), d_monitoringIoPtr(monitoringIoPtr)
{
}

ITask::State MonitoringTask::run(UNUSED_ARG brt::types::time_us time_now_us)
{
  bool isEbsActivated = false;
  ubyte4 acsError = asb::rte::ACS_ERROR::ro();
  if (acsError != BIT0) {
    isEbsActivated = d_monitoringIoPtr->enableEbs();
    d_state = State::TASK_RUN_ERROR;
  } else {
    d_state = State::TASK_OK;
  }
  if (!isEbsActivated) {
    d_state = State::TASK_RUN_ERROR;
  }

  return d_state;
};

}  // namespace monitoring
}  // namespace asb