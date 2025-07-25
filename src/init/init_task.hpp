#pragma once

#include "ITask.hpp"

#include "platform.hpp"

#include "initializer.hpp"

namespace asb
{
namespace init
{
using namespace asb::task;
using namespace brt::types;

ITask *createTask();

/**
 * @brief Errors of the driverless initialization task.
 *
 */
enum class eError : ubyte4
{
  RES_CONNECTION_LOST            = brt::types::BIT0,  //!< No RES connetion (via CAN bus usually).
  LOW_AIR_STORAGE_PRESSURE       = brt::types::BIT1,  //!< Low pressure in brake air storage.
  HIGH_AIR_STORAGE_PRESSURE      = brt::types::BIT2,  //!< High pressure in genbrake air storage.
  LOW_FRONT_HYDRAULIC_PRESSURE   = brt::types::BIT3,  //!< Low pressure in the front hydraulic line.
  LOW_REAR_HYDRAULIC_PRESSURE    = brt::types::BIT4,  //!< Low pressure in the rear hydraulic line.
  HIGH_FRONT_HYDRAULIC_PRESSURE  = brt::types::BIT5,  //!< High pressure in the front hydraulic line.
  HIGH_REAR_HYDRAULIC_PRESSURE   = brt::types::BIT6,  //!< High pressure in the rear hydraulic line.
  BRAKE_ACQUIRING_HARDWARE_ERROR = brt::types::BIT7,  //!< Hardware driver error when trying to acquire brakes.
  BRAKE_RELEASING_HARDWARE_ERROR = brt::types::BIT8,  //!< Hardware driver error when trying to release brakes.
  TIME_IS_EXPIRED                = brt::types::BIT9,  //!< Allowed initialization time has been exceeded.
  UNEXPECTED_WATCHDOG_STATE      = brt::types::BIT10,  //!< Watchdog gives a logic 0 signal.
  EBS_DEACTIVATION_FAILURE       = brt::types::BIT11  //!< Signal to disable the EBS Actuator has not been sent.
};

enum class InitState : ubyte1
{
  BEGIN                     = 0U,
  CHECK_WATCHDOG            = 1U,
  CHECK_STORAGE             = 2U,
  CHECK_RELEASE_PERFORMANCE = 3U,
  CHECK_ACQUIRE_PERFORMANCE = 4U
};

/**
 * @ingroup Tasks
 * @class InitTask
 * @brief Task for handling driverless initialization.
 * @details
 */
class InitTask : public ITask
{
public:
  InitTask(InitParamStorage *const initParamPtr);

  /**
   * @brief Main runner for the task.
   * @param [in] time_us Time elapsed since previous run.
   * @return State = TASK_OK - all is fine
   *                 TASK_RUN_ERROR - serious error on low level are occurred
   */
  State run(brt::types::time_us time_now_us) final;

  InitTask(InitTask const &)            = delete;
  InitTask &operator=(InitTask const &) = delete;

  /**
   * @brief returns flag that indicates start of initialization. It's used for unit-tests only.
   *
   * @return bool
   * @retval true if initialization is being gone.
   * @retval false if no initialization now.
   */
  bool initProcessWasStarted() const;

  /**
   * @brief returns initialization state
   *
   * @return asb::init::InitState
   * @retval InitState::BEGIN
   * @retval InitState::CHECK_WATCHDOG
   * @retval InitState::CHECK_STORAGE
   * @retval InitState::CHECK_RELEASE_PERFORMANCE
   * @retval InitState::CHECK_ACQUIRE_PERFORMANCE
   */
  InitState getInitState() const;

  /**
   * @brief sets an state of initialization
   *
   * @param [in] asb::init::InitState state.
   */
  void setInitState(const InitState state);

private:
  /**
   * @brief sets an error bit in the error register.
   *
   * @param [in] error INIT error.
   */
  void _setInitError(const asb::init::eError error);

private:
  /**
   * @brief Object's pointer that stores initialization parameters and wrapper functions for
   * receiving data from sensor.
   *
   */
  InitParamStorage *const d_initParamPtr;

  /// @brief flag that indicates start of initialization. It's used for unit-tests only.
  bool d_initStartFlag = false;

  /// @brief variable that stores state of current state of initialization.
  InitState d_initState = InitState::BEGIN;
};

}  // namespace init
}  // namespace asb
