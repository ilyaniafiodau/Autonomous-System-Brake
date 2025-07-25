#include "acs_task.hpp"
#include "fsm_task.hpp"
#include "init_task.hpp"
#include "monitoring_task.hpp"
#include "supervisor_task.hpp"
#include "brake_warning_light_task.hpp"

#include "scheduler_task.hpp"
#include "platform.hpp"

FSM_INITIAL_STATE(asb::fsm::AsbLogic, asb::fsm::state::Init);

static bool _bindTaskToSupervisor(asb::task::ITask *const supervisor, asb::task::ITask *const task)
{
  using namespace asb::task;
  return (
    // clang-format off
    static_cast<asb::supervisor::SupervisorTask *>(supervisor)->
      registerTask(task) == ITask::State::TASK_OK);
  // clang-format on
}

int main()
{
  (void)asb::platform::canbus::init();

  using namespace asb::task;
  ITask *const fsmTask        = asb::fsm::createTask();
  ITask *const initTask       = asb::init::createTask();
  ITask *const acsTask        = asb::acs::createTask();
  ITask *const monitoringTask = asb::monitoring::createTask();
  ITask *const supervisorTask = asb::supervisor::createTask();
  ITask *const brakeLightTask  = asb::brakeLight::createTask();

  bool initStatus = true;
  static asb::scheduler::Scheduler scheduler;

  // clang-format off
  initStatus &= (scheduler.registerTask(fsmTask, 100'000U) == ITask::State::TASK_OK);
  initStatus &= (scheduler.registerTask(initTask, 100'000U) == ITask::State::TASK_OK);
  initStatus &= (scheduler.registerTask(acsTask, 1'000U) == ITask::State::TASK_OK);
  initStatus &= (scheduler.registerTask(monitoringTask, 100'000U) == ITask::State::TASK_OK);  // consult with smb
  
  initStatus &= _bindTaskToSupervisor(supervisorTask, fsmTask);
  initStatus &= _bindTaskToSupervisor(supervisorTask, initTask);
  initStatus &= _bindTaskToSupervisor(supervisorTask, acsTask);
  /*---supervisor does not monitor monitoringTask---*/
  // clang-format on
  initStatus &= (scheduler.registerTask(brakeLightTask, 100'000U) == ITask::State::TASK_OK);

  if (initStatus == true) {
    {
      using namespace asb::fsm;
      AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::DriverlessInit>::value;
      AsbLogic::current_state_ptr->entry();
    }
    asb::time::initStartTime_us();
    while (true) {
      const auto monotonictime_us = asb::time::getMonotonicTime_us();
      (void)scheduler.run(monotonictime_us);
    }
  } else {
    while (true) {
      /* Error handler */
    }
  }

  return 0;
}