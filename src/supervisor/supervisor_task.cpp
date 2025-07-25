#include <algorithm>

#include "ITask.hpp"
#include "fsm/fsm.hpp"
#include "supervisor.hpp"
#include "supervisor_task.hpp"

namespace asb
{
namespace supervisor
{
ITask *createTask()
{
  static Supervisor supervisor;
  static SupervisorTask task(&supervisor);
  return static_cast<ITask *>(&task);
}

SupervisorTask::SupervisorTask(Supervisor *const supervisorPtr)
: ITask(asb::db::eTaskID::SUPERVISOR), d_supervisorPtr(supervisorPtr)
{
  d_tasks.reserve(d_maxTaskNumber);
}

ITask::State SupervisorTask::registerTask(asb::task::ITask *const task)
{
  using namespace asb::task;

  ITask::State res = State::TASK_INIT_ERROR;

  auto taskToBeRegistered = [task](const TaskRecord &taskRecord) {
    return taskRecord.task == task;
  };

  if (task != nullptr) {
    if (task->init() == ITask::State::TASK_OK) {
      if (find_if(d_tasks.begin(), d_tasks.end(), taskToBeRegistered) == d_tasks.end())
      {  // if the task is not found in the d_tasks vector
        if (d_tasks.size() < d_maxTaskNumber) {
          (void)d_tasks.emplace_back(task);  // creates a task at the end of the d_tasks vector
          res = State::TASK_OK;
        } else {
          /*---do nothin'---*/
        }
      } else {
        /*---fatal task id logic---*/
      }
    }
  }

  return res;
}

ITask::State SupervisorTask::init()
{
  return ITask::State::TASK_OK;
}

ITask::State SupervisorTask::run(UNUSED_ARG brt::types::time_us now_us)
{
  using namespace asb::fsm;
  ITask::State res = State::TASK_OK;

  for (auto &taskRecord : d_tasks) {
    ubyte4 error = taskRecord.task->errors();

    if (AsbLogic::is_in_state<state::DriverlessRun>()) {
      const ubyte4 errorMask = taskRecord.task->driverlessErrorMask().mask;
      if ((error & errorMask) > 0U) {
        // clang-format off
        bool isSdcStateSet = d_supervisorPtr->setShutdownState(CircuitState::OPEN);  // SDC is closed initially by TS Activator
        // clang-format on
      } else {
        /*---do nothin'---*/
      }
    } else if (AsbLogic::is_in_state<state::ManualRun>()) {
      const ubyte4 errorMask = taskRecord.task->manualErrorMask().mask;
      if ((error & errorMask) > 0U) {
        bool isSdcStateSet = d_supervisorPtr->setShutdownState(CircuitState::OPEN);  
      } else {
        /*---do nothin'---*/
      }
    } else {
      bool isSdcStateSet = d_supervisorPtr->setShutdownState(CircuitState::CLOSED);
    }

    ubyte1 errorBitNumber = 0U;
    while (error > 0U) {
      if (error & 0x1U) {
        ErrorCode errorCode(BrtDevice::ASB, taskRecord.task->id(), errorBitNumber++);
        bool isErrorSent = d_supervisorPtr->sendError(errorCode.code());
      } else {
        /*---do nothin'---*/
      }
      error >>= 1U;
    }
  }

  return res;
}
}  // namespace supervisor
}  // namespace asb