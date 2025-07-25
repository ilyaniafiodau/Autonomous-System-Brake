#pragma once

#include <vector>

#include "ITask.hpp"
#include "supervisor.hpp"

namespace asb
{
namespace supervisor
{
using namespace asb::task;

ITask *createTask();

/**
 * @ingroup Tasks
 * @class SupervisorTask
 * @brief Task for supervising.
 * @details
 */
class SupervisorTask : public ITask
{
public:
  SupervisorTask(Supervisor *const supervisorPtr);
  
  SupervisorTask(SupervisorTask const &)             = delete;
  SupervisorTask(SupervisorTask const &&)            = delete;
  SupervisorTask &operator=(SupervisorTask const &)  = delete;
  SupervisorTask &operator=(SupervisorTask const &&) = delete;

  State registerTask(ITask *const task);

  /**
   * @brief Main runner for the task.
   * @param [in] time_us Time elapsed since previous run.
   * @return State = TASK_OK - everything is fine
   *                 TASK_RUN_ERROR - serious error on low level has occurred
   */
  State run(time_us time_us) final;

  /**
   * @brief Initializer of the external network setup. Sets the errors (critical and not critical).
   * @return State = TASK_OK - everything is fine
   *                 TASK_RUN_ERROR - serious error on low level has occurred
   */
  State init() final;

private:
  /**
   * @brief points to the Supervisor's instance 
   */
  Supervisor *const d_supervisorPtr;

  /**
   * @brief stores tasks' pointers that are monitored by the supervisor
   */
  struct TaskRecord {
    TaskRecord(ITask *task) : task(task) {}
    ITask *const task;
  };

  /**
   * @brief vector that stores task data
   */
  std::vector<TaskRecord> d_tasks;

  /**
   * @brief Max number of tasks that can be linked to this supervisor
   */
  const size_t d_maxTaskNumber = 32;
};

}  // namespace supervisor
}  // namespace asb