#pragma once

#include "ITask.hpp"
#include "monitoring_io.hpp"

namespace asb
{
namespace monitoring
{
using namespace asb::task;
using namespace brt::types;

ITask *createTask();

/**
 * @brief Errors occurring during driverless running
 *
 */
enum class eError : ubyte4
{
  NO_ERRORS            = brt::types::BIT0,  
  ENERGY_STORAGE_EMPTY = brt::types::BIT1,  
  RELEASE_POINT        = brt::types::BIT2,  
  WD_FAILURE           = brt::types::BIT3,  
  INIT_FAILURE         = brt::types::BIT4,  
  SDC_OPEN             = brt::types::BIT5,  
  DRIVING_MODE_ERROR   = brt::types::BIT6,  
  BRAKES_NOT_WORKING   = brt::types::BIT7
};

/**
 * @ingroup Tasks
 * @class MonitoringTask
 * @brief Continuous Monitoring Task.
 * @details
 */
class MonitoringTask : public ITask
{
public:
  MonitoringTask(MonitoringIO *const monitoringIoPtr);

  /**
   * @brief Main runner for the task.
   * @param [in] time_us Time elapsed since previous run.
   * @return State = TASK_OK - all is fine
   *                 TASK_RUN_ERROR - serious error on low level are occurred
   */
  State run(brt::types::time_us time_us) final;

private:
  /**
   * @brief sets an error bit in the error register.
   *
   * @param [in] eError error.
   */
  void _setTaskError(const asb::monitoring::eError error);

private:
  /**
   * @brief Object's pointer that stores monitoring io interface.
   *
   */
  MonitoringIO *const d_monitoringIoPtr;
};

}  // namespace monitoring
}  // namespace asb