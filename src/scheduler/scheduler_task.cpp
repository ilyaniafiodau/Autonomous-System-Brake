#include <algorithm>

#include "platform.hpp"
#include "types.h"

#include "scheduler_task.hpp"

namespace asb
{
namespace scheduler
{
using namespace asb::task;

/* ------------------------- Scheduler::Scheduler() ------------------------- */
Scheduler::Scheduler() : ITask(asb::db::eTaskID::SCHEDULER)
{
  d_tasks.reserve(d_maxTaskNumber);
}

/* ------------------------ Scheduler::registerTask() ----------------------- */
ITask::State Scheduler::registerTask(ITask *const task, brt::types::time_us period)
{
  using namespace asb::task;

  ITask::State res = State::TASK_INIT_ERROR;

  if (task != nullptr) {
    if (task->init() == ITask::State::TASK_OK) {
      auto taskToBeRegistered = [task](const TaskRecord &taskRecord) {
        return taskRecord.task == task;
      };
      if (find_if(d_tasks.begin(), d_tasks.end(), taskToBeRegistered) == d_tasks.end()) {
        if (d_tasks.size() < d_maxTaskNumber) {
          (void)d_tasks.emplace_back(task, period);
          res = State::TASK_OK;
        }
      } else {
#ifdef DEBUG
        UNUSED_ARG const fatalTaskID = task->id();
        /// TODO: Сюда можно добавить отправку лога с fatalTaskID напрямую, поскольку
        /// хранение ошибок в регистре ошибок не подраузмевает хранение целых переменных, а
        /// обработкой битов ошибок занимается модуль `Error logger`.
#endif
      }
    }
  }

  return res;
}

/* -------------- Scheduler::run() ------------- */
ITask::State Scheduler::run(UNUSED_ARG brt::types::time_us time)
{
  using namespace asb::task;

  ITask::State res = State::TASK_OK;

  using namespace asb::time;

  for (auto &taskRecord : d_tasks) {
    const auto now = getMonotonicTime_us();
    if ((now - taskRecord.lastCall_us) >= taskRecord.period_us) {
      taskRecord.lastCall_us = now;
      res                    = taskRecord.task->run(now);
    } else {
      /* No actions otherwise */
    }
  }

  return res;
}

}  // namespace scheduler
}  // namespace asb
