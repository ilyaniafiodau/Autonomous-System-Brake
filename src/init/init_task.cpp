#include "init_task.hpp"

#include "platform.hpp"

#include "rte/generated/rte.hpp"
#include "types.h"

#include "fsm/fsm.hpp"

#include "initializer.hpp"

namespace asb
{
namespace init
{
using namespace asb::fsm;
using namespace asb::task;
using namespace brt::types;

// Этот макрос встраивается внутрь метода run() и содержит return, что приводит к нарушению
// правила https://pvs-studio.ru/ru/docs/warnings/v2506/ Других ПРОСТЫХ способов сделать
// неблокирующую задержку для линейного планировщика задач нет.
#define NON_BLOCKING_WAITING_FOR(US)                     \
  do {                                                   \
    static volatile time_us init = 0U;                   \
    if (init == 0U) {                                    \
      init = asb::time::getMonotonicTime_us();           \
    }                                                    \
    if (asb::time::getMonotonicTime_us() - init < US) {  \
      return ITask::State::TASK_OK;                      \
    } else {                                             \
      init = asb::time::getMonotonicTime_us();           \
    }                                                    \
  } while (0)

ITask *createTask()
{
  static InitParamStorage param;
  static InitTask task(&param);
  return static_cast<ITask *>(&task);
}

InitTask::InitTask(InitParamStorage *const initParamPtr)
: ITask(asb::db::eTaskID::INIT), d_initParamPtr(initParamPtr)
{
}

ITask::State InitTask::run(UNUSED_ARG brt::types::time_us time_now_us)
{
  using asb::time::getMonotonicTime_us;

  if (AsbLogic::is_in_state<state::DriverlessInit>()) {
    bool isEbsActivated = asb::platform::io::disableEbs();   //should be true if EBS has been disabled      
    if (!isEbsActivated) {
      _setInitError(eError::EBS_DEACTIVATION_FAILURE);
    }

    static time_us initStartTime_us = getMonotonicTime_us();

    if (!d_initStartFlag) {
      d_initStartFlag  = true;
      initStartTime_us = getMonotonicTime_us();
    }

    constexpr time_us maxInitTime_us = 5'000'000U;  // 5 seconds
    if ((getMonotonicTime_us() - initStartTime_us) > maxInitTime_us) {
      _setInitError(eError::TIME_IS_EXPIRED);
      d_state = State::TASK_RUN_ERROR;
      setInitState(InitState::BEGIN);
    } else {
      switch (getInitState()) {
        case InitState::BEGIN: {
          ITask::clearAllErrors();
          setInitState(InitState::CHECK_WATCHDOG);  // Переходим в следующий стейдж инициализации
          break;
        }
        case InitState::CHECK_WATCHDOG: {
          if (d_initParamPtr->checkWatchdog() == digitalState::ON) {
            const time_us waitingTime_us        = 100'000;  // 100 ms
            const time_us startFreezeTime_us    = getMonotonicTime_us();
            ubyte4 safetyCounter                = 0U;
            static bool watchdogShouldBeStopped = true;
            while ((getMonotonicTime_us() < (startFreezeTime_us + waitingTime_us)) &&
                   (safetyCounter < 100'000'000U) && watchdogShouldBeStopped)
            {
              // Искуственно тормозим ВСЮ программу на 100 мс. В случае какой-либо аппаратной ошибки
              // просто переполнится счетчик, и цикл закончится.
              // ВАЖНО!! Этот счетчик позволяет юнит-тесту [TEST_F(InitTaskTest, CHECK_BEGIN_STATE)]
              // выйти из цикла по переполнению, потому что функция getMonotonicTime_us() не
              // мокается. Без этого счетчика тест бы завис в бесконечном цикле.
              ++safetyCounter;
            }
            watchdogShouldBeStopped = false;
            // Ждем в фоне 100 мс, чтобы другая задача подергала вотчдог
            NON_BLOCKING_WAITING_FOR(waitingTime_us);
            watchdogShouldBeStopped = true;
            if (d_initParamPtr->checkWatchdog() == digitalState::OFF) {
              setInitState(InitState::CHECK_STORAGE);  // Переходим в следующий стейдж
            } else {
              _setInitError(eError::UNEXPECTED_WATCHDOG_STATE);
              d_state = State::TASK_RUN_ERROR;
            }
          } else {
            _setInitError(eError::UNEXPECTED_WATCHDOG_STATE);
            d_state = State::TASK_RUN_ERROR;
          }
          break;
        }
        case InitState::CHECK_STORAGE: {
          bool res = true;

          const auto airStoragePressure_Bar = d_initParamPtr->getAirStoragePressure_Bar();
          if (airStoragePressure_Bar < d_initParamPtr->getMinAirPressure_Bar()) {
            _setInitError(eError::LOW_AIR_STORAGE_PRESSURE);
            res = false;
          }
          if (airStoragePressure_Bar > d_initParamPtr->getMaxAirPressure_Bar()) {
            _setInitError(eError::HIGH_AIR_STORAGE_PRESSURE);
            res = false;
          }

          if (true == res) {
            setInitState(InitState::CHECK_RELEASE_PERFORMANCE);  // Переходим в следующий стейдж
          } else {
            d_state = State::TASK_RUN_ERROR;
          }

          break;
        }
        case InitState::CHECK_RELEASE_PERFORMANCE: {
          auto releaseBrake = [this]() {
            return d_initParamPtr->setHydraulicPressure_fraction(0.0F);
          };

          bool res = true;

          if (true == releaseBrake()) {
            NON_BLOCKING_WAITING_FOR(1'000'000U);  // Ждем окончания всех переходных процессов.
                                                   // Время 1 секунда здесь используется интуитивно.
            constexpr pressure_Bar maxHydraulicPressureForDisarmedBrake_Bar = 0.1F;
            if (
              d_initParamPtr->getFrontHydraulicPressure_Bar() >
              maxHydraulicPressureForDisarmedBrake_Bar) {
              _setInitError(eError::HIGH_FRONT_HYDRAULIC_PRESSURE);
              res = false;
            }
            if (
              d_initParamPtr->getRearHydraulicPressure_Bar() >
              maxHydraulicPressureForDisarmedBrake_Bar) {
              _setInitError(eError::HIGH_REAR_HYDRAULIC_PRESSURE);
              res = false;
            }
          } else {
            _setInitError(eError::BRAKE_RELEASING_HARDWARE_ERROR);
            res = false;
          }

          if (true == res) {
            setInitState(InitState::CHECK_ACQUIRE_PERFORMANCE);  // Переходим в следующий стейдж
          } else {
            d_state = State::TASK_RUN_ERROR;
          }

          break;
        }
        case InitState::CHECK_ACQUIRE_PERFORMANCE: {
          auto acquireBrake = [this]() {
            return d_initParamPtr->setHydraulicPressure_fraction(1.0F);
          };

          bool res = true;

          if (true == acquireBrake()) {
            NON_BLOCKING_WAITING_FOR(1'000'000U);  // Ждем окончания всех переходных процессов.
                                                   // Время 1 секунда здесь используется интуитивно.
            constexpr pressure_Bar minHydraulicPressureForArmedBrake_Bar = 30.0F;
            if (
              d_initParamPtr->getFrontHydraulicPressure_Bar() <
              minHydraulicPressureForArmedBrake_Bar) {
              _setInitError(eError::LOW_FRONT_HYDRAULIC_PRESSURE);
              res = false;
            }
            if (
              d_initParamPtr->getRearHydraulicPressure_Bar() <
              minHydraulicPressureForArmedBrake_Bar) {
              _setInitError(eError::LOW_REAR_HYDRAULIC_PRESSURE);
              res = false;
            }
          } else {
            _setInitError(eError::BRAKE_ACQUIRING_HARDWARE_ERROR);
            res = false;
          }

          if (true == res) {
            setInitState(InitState::BEGIN);  // Переходим в первый следующий стейдж на случай
                                             // необходимости проведеня повторной инициализации.
          } else {
            d_state = State::TASK_RUN_ERROR;
          }

          // Это последний этап инициализации, поэтому в нем генерируем события для FSM, оповещающие
          // о результате инициализации на основании содержимого регистра ошибок.
          if (0U == ITask::errors()) {
            asb::rte::FSM_INIT_CHECK_UP_OK::rw() = true;
          } else {
            asb::rte::FSM_INIT_CHECK_UP_EMERGENCY::rw() = true;
          }

          break;
        }
        default: {
          /* No actions otherwse */
          break;
        }
      }
    }
  } else {
    d_initStartFlag = false;
  }

  return d_state;
}

bool InitTask::initProcessWasStarted() const
{
  return d_initStartFlag;
}

InitState InitTask::getInitState() const
{
  return d_initState;
}

void InitTask::_setInitError(const asb::init::eError error)
{
  ITask::setError(static_cast<ubyte4>(error));
}

void InitTask::setInitState(const InitState state)
{
  d_initState = state;
}

}  // namespace init
}  // namespace asb
