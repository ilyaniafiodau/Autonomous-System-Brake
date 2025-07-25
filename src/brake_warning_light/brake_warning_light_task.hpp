#pragma once

#include "ITask.hpp"
#include "brake_warning_light.hpp"

namespace asb
{
namespace brakeLight
{
using namespace asb::task;

ITask *createTask();

enum class OnOffState: uint8_t {
    HIGH = 0U,
    LOW
};

class BrakeLightTask : public ITask
{
public:
  BrakeLightTask(BrakeLightIO *const brakeLightIoPtr);

  /**
   * @brief Main runner for the task.
   * @param [in] time_us Time elapsed since previous run.
   * @return State = TASK_OK - everything is fine
   *                 TASK_RUN_ERROR - serious error on low level has occurred
   */
  State run(brt::types::time_us time_us) final;

private:
  /**
   * @brief Object's pointer that stores acs io interface.
   *
   */
  BrakeLightIO *const d_brakeLightIoPtr;
};
}  // namespace acs
}  // namespace asb