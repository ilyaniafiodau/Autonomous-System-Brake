#include "acs_task.hpp"

#include "types.h"

#include "platform.hpp"

#include "rte/generated/rte.hpp"

#include "fsm/fsm.hpp"

namespace asb
{
namespace acs
{
using namespace brt::types;
using namespace asb::fsm;

ITask *createTask()
{
  static AcsIO acsIoPtr;
  static AcsTask task(&acsIoPtr);
  return static_cast<ITask *>(&task);
}

AcsTask::AcsTask(AcsIO *const acsIoPtr) : ITask(asb::db::eTaskID::RESOLVER), d_acsIoPtr(acsIoPtr) {}

ITask::State AcsTask::run(UNUSED_ARG brt::types::time_us time_now_us)
{
  if (AsbLogic::is_in_state<state::DriverlessRun>()) {
    const torque_Nm targetBrakeTorque_Nm          = asb::rte::REQUIRED_TORQUE_Nm::ro();
    const pressure_Bar pneumoValvePressure_Bar    = d_acsIoPtr->getValvePneumoPressure_Bar();
    const pressure_Bar frontHydraulicPressure_Bar = d_acsIoPtr->getFrontHydraulicPressure_Bar();
    const pressure_Bar rearHydraulicPressure_Bar  = d_acsIoPtr->getRearHydraulicPressure_Bar();

    // clang-format off
    const voltage_V targetValveVoltage = resolve(
      targetBrakeTorque_Nm, 
      frontHydraulicPressure_Bar, 
      rearHydraulicPressure_Bar,
      pneumoValvePressure_Bar
    );
    // clang-format on

    bool hasSetVoltage = d_acsIoPtr->setValveVoltage(targetValveVoltage);
    if (hasSetVoltage) {
      d_state = State::TASK_OK;
    } else {
      d_state = State::TASK_RUN_ERROR;
    }
  }
  return d_state;
};

}  // namespace acs
}  // namespace asb
