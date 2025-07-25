#pragma once

#include "ITask.hpp"

#include "fsm.hpp"

namespace asb
{
namespace fsm
{
using namespace asb::task;

ITask *createTask();
/**
 * @ingroup Tasks
 * @class FsmTask
 * @brief Task for handling the asb Finite State Machine.
 */
class FsmTask : public ITask
{
public:
  FsmTask();

  /**
   * @brief Main runner for the task.
   * @param [in] time_us Time elapsed since previous run.
   * @return State = TASK_OK - all is fine
   *                 TASK_RUN_ERROR - serious error on low level occurred
   */
  State run(brt::types::time_us time_us = 0) final;
};
}  // namespace fsm
}  // namespace asb
