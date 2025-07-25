#include "types.h"

#include "brake_warning_light_task.hpp"

#include "platform.hpp"

#include "rte/generated/rte.hpp"

namespace asb
{
namespace brakeLight
{
using namespace brt::types;

ITask *createTask()
{
  static BrakeLightIO brakeLightIoPtr;
  static BrakeLightTask task(&brakeLightIoPtr);
  return static_cast<ITask *>(&task);
}

BrakeLightTask::BrakeLightTask(BrakeLightIO *const brakeLightIoPtr)
: ITask(asb::db::eTaskID::BRAKE_LIGHT), d_brakeLightIoPtr(brakeLightIoPtr)
{
}

ITask::State BrakeLightTask::run(UNUSED_ARG brt::types::time_us time_now_us)
{
  bool hasSentSignal          = false;
  static OnOffState prevState = OnOffState::LOW;

  const pressure_Bar frontHydraulicPressure_Bar =
    d_brakeLightIoPtr->getFrontHydraulicPressure_Bar();
  const pressure_Bar rearHydraulicPressure_Bar = d_brakeLightIoPtr->getRearHydraulicPressure_Bar();
  const pressure_Bar totalHydraulicPressure_Bar =
    frontHydraulicPressure_Bar + rearHydraulicPressure_Bar;

  if ((totalHydraulicPressure_Bar >= 0.0F) && (totalHydraulicPressure_Bar <= 3.0F)) {
    hasSentSignal = d_brakeLightIoPtr->turnBrakeLightOff();
    prevState     = OnOffState::LOW;
  } else if ((totalHydraulicPressure_Bar >= 6.0F) && (totalHydraulicPressure_Bar <= 40.0F)) {
    hasSentSignal = d_brakeLightIoPtr->turnBrakeLightOn();
    prevState     = OnOffState::HIGH;
  } else {  // either the pressure is above 40 bar or the pressure sensor output is negative
    hasSentSignal = d_brakeLightIoPtr->turnBrakeLightOn();
    d_state       = State::TASK_RUN_ERROR;
  }

  if ((totalHydraulicPressure_Bar > 3.0F) && (totalHydraulicPressure_Bar < 6.0F)) {
    switch (prevState) {
      case OnOffState::HIGH:
        hasSentSignal = d_brakeLightIoPtr->turnBrakeLightOn();
        break;
      case OnOffState::LOW:
        hasSentSignal = d_brakeLightIoPtr->turnBrakeLightOff();
        break;
    }
  }

  if (hasSentSignal && (d_state != State::TASK_RUN_ERROR)) {
    d_state = State::TASK_OK;
  }

  return d_state;
};

}  // namespace brakeLight
}  // namespace asb
