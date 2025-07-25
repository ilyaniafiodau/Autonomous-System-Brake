#include "types.h"

#include "fsm.hpp"
#include "fsm_task.hpp"
#include "rte/generated/rte.hpp"

namespace asb
{
namespace fsm
{
using namespace asb::task;

ITask *createTask()
{
  static FsmTask task;
  return static_cast<ITask *>(&task);
}

FsmTask::FsmTask() : ITask(asb::db::eTaskID::FSM)
{
  AsbLogic::start();
}

ITask::State FsmTask::run(UNUSED_ARG brt::types::time_us time_us)
{
  d_state = State::TASK_OK;

  AsbLogic::dispatch(event::MissionRequest(asb::rte::FSM_MISSION_REQUEST::ro()));
  AsbLogic::dispatch(event::GoSignal(asb::rte::FSM_GO_SIGNAL::ro()));
  AsbLogic::dispatch(event::ResetAsb(asb::rte::FSM_RESET_ASB::ro()));
  AsbLogic::dispatch(event::AsbFinish(asb::rte::FSM_ASB_FINISH::ro()));
  
  AsbLogic::dispatch(event::InitCheckUpRequest(asb::rte::FSM_INIT_CHECK_UP_REQUEST::ro()));
  AsbLogic::dispatch(event::InitResultEmergency(asb::rte::FSM_INIT_CHECK_UP_EMERGENCY::ro()));
  AsbLogic::dispatch(event::InitResultOk(asb::rte::FSM_INIT_CHECK_UP_OK::ro()));

  return d_state;
}

}  // namespace fsm
}  // namespace asb
