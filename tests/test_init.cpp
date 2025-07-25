#include <iostream>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "types.h"

#include "fsm/fsm_task.hpp"
#include "init/init_task.hpp"
#include "rte/generated/rte.hpp"

#include "weak/time.hpp"

namespace asb::init
{

using namespace brt::types;
using namespace asb::rte;
using namespace asb::time;

class InitMock : public asb::init::InitParamStorage
{
public:
  MOCK_METHOD(pressure_Bar, getAirStoragePressure_Bar, (), (const));
  MOCK_METHOD(pressure_Bar, getFrontHydraulicPressure_Bar, (), (const));
  MOCK_METHOD(pressure_Bar, getRearHydraulicPressure_Bar, (), (const));
  MOCK_METHOD(digitalState, checkWatchdog, (), (const));
  MOCK_METHOD(bool, setHydraulicPressure_fraction, (fraction val), (const));
};

class InitTaskTest : public ::testing::Test
{
protected:
  std::unique_ptr<InitTask> d_task = nullptr;
  InitMock d_initMock;

  template <class T>
  void _writeRte(const T &state, T value)
  {
    const_cast<T &>(state) = value;
  }

  template <class T>
  void _writeRte(T &state, T value)
  {
    const_cast<T &>(state) = value;
  }

  void SetUp() final override
  {
    d_task = std::make_unique<InitTask>(&d_initMock);
    d_task->clearAllErrors();
    _writeRte(FSM_INIT_CHECK_UP_EMERGENCY::ro(), false);
    _writeRte(FSM_INIT_CHECK_UP_OK::ro(), false);
  }
};

TEST_F(InitTaskTest, INIT_ONLY_IN_DRIVERLESS_INIT_STATE)
{
  using namespace asb::fsm;

  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::Init>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::Init>());
  d_task->run(0);
  EXPECT_FALSE(d_task->initProcessWasStarted());

  //------------------------------------------------------------------------------------------------------

  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::Hold>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::Hold>());
  d_task->run(0);
  EXPECT_FALSE(d_task->initProcessWasStarted());
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::DriverlessSelected>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::DriverlessSelected>());
  d_task->run(0);
  EXPECT_FALSE(d_task->initProcessWasStarted());
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  // Только в состоянии DriverlessInit инициализация может быть запущена
  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::DriverlessInit>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::DriverlessInit>());
  d_task->run(0);
  EXPECT_TRUE(d_task->initProcessWasStarted());
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::DriverlessWaitForGo>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::DriverlessWaitForGo>());
  d_task->run(0);
  EXPECT_FALSE(d_task->initProcessWasStarted());
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::DriverlessRun>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::DriverlessRun>());
  d_task->run(0);
  EXPECT_FALSE(d_task->initProcessWasStarted());
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::ManualRun>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::ManualRun>());
  d_task->run(0);
  EXPECT_FALSE(d_task->initProcessWasStarted());
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::ManualSelected>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::ManualSelected>());
  d_task->run(0);
  EXPECT_FALSE(d_task->initProcessWasStarted());
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);
}

TEST_F(InitTaskTest, INIT_TIMEOUT)
{
  using namespace asb::fsm;
  using ::testing::AnyNumber;
  using ::testing::Return;

  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::DriverlessInit>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::DriverlessInit>());

  EXPECT_CALL(d_initMock, getAirStoragePressure_Bar).WillRepeatedly(Return(7.0F));

  initStartTime_us();
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  increaseMonotonicTime_us(5'100'000U);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_RUN_ERROR);
  EXPECT_EQ(d_task->errors(), static_cast<ubyte4>(eError::TIME_IS_EXPIRED));
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);
}

TEST_F(InitTaskTest, CHECK_BEGIN_STATE)
{
  using namespace asb::fsm;
  using ::testing::AnyNumber;
  using ::testing::Return;

  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::DriverlessInit>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::DriverlessInit>());

  EXPECT_EQ(d_task->getInitState(), InitState::BEGIN);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_WATCHDOG);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);
}

TEST_F(InitTaskTest, CHECK_WATCHDOG)
{
  using namespace asb::fsm;
  using ::testing::AnyNumber;
  using ::testing::Return;

  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::DriverlessInit>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::DriverlessInit>());

  //------------------------------------------------------------------------------------------------------

  d_task->clearAllErrors();

  EXPECT_CALL(d_initMock, checkWatchdog).WillOnce(Return(digitalState::ON));
  d_task->setInitState(InitState::CHECK_WATCHDOG);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_WATCHDOG);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  EXPECT_CALL(d_initMock, checkWatchdog)
    .WillOnce(Return(digitalState::ON))
    .WillOnce(Return(digitalState::OFF));
  increaseMonotonicTime_us(200'000U);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_STORAGE);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  d_task->clearAllErrors();

  EXPECT_CALL(d_initMock, checkWatchdog).WillOnce(Return(digitalState::OFF));
  d_task->setInitState(InitState::CHECK_WATCHDOG);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_WATCHDOG);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_RUN_ERROR);
  EXPECT_EQ(d_task->errors(), static_cast<ubyte4>(eError::UNEXPECTED_WATCHDOG_STATE));
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_WATCHDOG);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  d_task->clearAllErrors();

  EXPECT_CALL(d_initMock, checkWatchdog).WillOnce(Return(digitalState::ON));
  d_task->setInitState(InitState::CHECK_WATCHDOG);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_WATCHDOG);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  EXPECT_CALL(d_initMock, checkWatchdog)
    .WillOnce(Return(digitalState::ON))
    .WillOnce(Return(digitalState::ON));
  increaseMonotonicTime_us(200'000U);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_RUN_ERROR);
  EXPECT_EQ(d_task->errors(), static_cast<ubyte4>(eError::UNEXPECTED_WATCHDOG_STATE));
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_WATCHDOG);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------
}

TEST_F(InitTaskTest, CHECK_STORAGE)
{
  using namespace asb::fsm;
  using ::testing::AnyNumber;
  using ::testing::Return;

  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::DriverlessInit>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::DriverlessInit>());
  //------------------------------------------------------------------------------------------------------
  d_task->setInitState(InitState::CHECK_STORAGE);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_STORAGE);
  EXPECT_CALL(d_initMock, getAirStoragePressure_Bar).WillOnce(Return(6.0F));
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);
  //------------------------------------------------------------------------------------------------------
  d_task->setInitState(InitState::CHECK_STORAGE);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_STORAGE);
  EXPECT_CALL(d_initMock, getAirStoragePressure_Bar).WillOnce(Return(8.0F));
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);
  //------------------------------------------------------------------------------------------------------
  d_task->setInitState(InitState::CHECK_STORAGE);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_STORAGE);
  EXPECT_CALL(d_initMock, getAirStoragePressure_Bar).WillOnce(Return(5.99F));
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_RUN_ERROR);
  EXPECT_EQ(d_task->errors(), static_cast<ubyte4>(eError::LOW_AIR_STORAGE_PRESSURE));
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_STORAGE);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);
  //------------------------------------------------------------------------------------------------------
  d_task->clearAllErrors();
  d_task->setInitState(InitState::CHECK_STORAGE);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_STORAGE);
  EXPECT_CALL(d_initMock, getAirStoragePressure_Bar).WillOnce(Return(8.01F));
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_RUN_ERROR);
  EXPECT_EQ(d_task->errors(), static_cast<ubyte4>(eError::HIGH_AIR_STORAGE_PRESSURE));
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_STORAGE);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);
}

TEST_F(InitTaskTest, CHECK_RELEASE_PERFORMANCE)
{
  using namespace asb::fsm;
  using ::testing::AnyNumber;
  using ::testing::Return;

  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::DriverlessInit>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::DriverlessInit>());

  EXPECT_CALL(d_initMock, setHydraulicPressure_fraction).WillRepeatedly(Return(true));

  //------------------------------------------------------------------------------------------------------

  d_task->clearAllErrors();
  d_task->setInitState(InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  increaseMonotonicTime_us(1'000'001U);  // 1.000001 sec
  EXPECT_CALL(d_initMock, getFrontHydraulicPressure_Bar).WillOnce(Return(0.09F));
  EXPECT_CALL(d_initMock, getRearHydraulicPressure_Bar).WillOnce(Return(0.09F));
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_ACQUIRE_PERFORMANCE);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  d_task->setInitState(InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  increaseMonotonicTime_us(1'000'001U);  // 1.000001 sec
  EXPECT_CALL(d_initMock, getFrontHydraulicPressure_Bar).WillOnce(Return(0.11F));
  EXPECT_CALL(d_initMock, getRearHydraulicPressure_Bar).WillOnce(Return(0.09F));
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_RUN_ERROR);
  EXPECT_EQ(d_task->errors(), static_cast<ubyte4>(eError::HIGH_FRONT_HYDRAULIC_PRESSURE));
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  d_task->clearAllErrors();

  d_task->setInitState(InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  increaseMonotonicTime_us(1'000'001U);  // 1.000001 sec
  EXPECT_CALL(d_initMock, getFrontHydraulicPressure_Bar).WillOnce(Return(0.09F));
  EXPECT_CALL(d_initMock, getRearHydraulicPressure_Bar).WillOnce(Return(0.11F));
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_RUN_ERROR);
  EXPECT_EQ(d_task->errors(), static_cast<ubyte4>(eError::HIGH_REAR_HYDRAULIC_PRESSURE));
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  d_task->clearAllErrors();

  EXPECT_CALL(d_initMock, setHydraulicPressure_fraction).WillRepeatedly(Return(false));
  d_task->setInitState(InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_RELEASE_PERFORMANCE);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_RUN_ERROR);
  EXPECT_EQ(d_task->errors(), static_cast<ubyte4>(eError::BRAKE_RELEASING_HARDWARE_ERROR));
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_RELEASE_PERFORMANCE);

  //------------------------------------------------------------------------------------------------------
}

TEST_F(InitTaskTest, CHECK_ACQUIRE_PERFORMANCE)
{
  using namespace asb::fsm;
  using ::testing::AnyNumber;
  using ::testing::Return;

  AsbLogic::current_state_ptr = &tinyfsm::_state_instance<state::DriverlessInit>::value;
  AsbLogic::current_state_ptr->entry();
  EXPECT_TRUE(AsbLogic::is_in_state<state::DriverlessInit>());

  EXPECT_CALL(d_initMock, setHydraulicPressure_fraction).WillRepeatedly(Return(true));

  //------------------------------------------------------------------------------------------------------

  d_task->clearAllErrors();
  d_task->setInitState(InitState::CHECK_ACQUIRE_PERFORMANCE);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_ACQUIRE_PERFORMANCE);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_ACQUIRE_PERFORMANCE);

  increaseMonotonicTime_us(1'000'001U);  // 1.000001 sec
  EXPECT_CALL(d_initMock, getFrontHydraulicPressure_Bar).WillOnce(Return(30.01F));
  EXPECT_CALL(d_initMock, getRearHydraulicPressure_Bar).WillOnce(Return(30.01F));
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(d_task->getInitState(), InitState::BEGIN);

  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), true);

  _writeRte(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  d_task->setInitState(InitState::CHECK_ACQUIRE_PERFORMANCE);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_ACQUIRE_PERFORMANCE);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_ACQUIRE_PERFORMANCE);

  increaseMonotonicTime_us(1'000'001U);  // 1.000001 sec
  EXPECT_CALL(d_initMock, getFrontHydraulicPressure_Bar).WillOnce(Return(30.01F));
  EXPECT_CALL(d_initMock, getRearHydraulicPressure_Bar).WillOnce(Return(29.99F));
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_RUN_ERROR);
  EXPECT_EQ(d_task->errors(), static_cast<ubyte4>(eError::LOW_REAR_HYDRAULIC_PRESSURE));
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_ACQUIRE_PERFORMANCE);

  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  d_task->clearAllErrors();

  d_task->setInitState(InitState::CHECK_ACQUIRE_PERFORMANCE);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_ACQUIRE_PERFORMANCE);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_OK);
  EXPECT_EQ(d_task->errors(), 0U);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_ACQUIRE_PERFORMANCE);

  increaseMonotonicTime_us(1'000'001U);  // 1.000001 sec
  EXPECT_CALL(d_initMock, getFrontHydraulicPressure_Bar).WillOnce(Return(29.99F));
  EXPECT_CALL(d_initMock, getRearHydraulicPressure_Bar).WillOnce(Return(30.01F));
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_RUN_ERROR);
  EXPECT_EQ(d_task->errors(), static_cast<ubyte4>(eError::LOW_FRONT_HYDRAULIC_PRESSURE));
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_ACQUIRE_PERFORMANCE);

  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------

  d_task->clearAllErrors();

  EXPECT_CALL(d_initMock, setHydraulicPressure_fraction).WillRepeatedly(Return(false));
  d_task->setInitState(InitState::CHECK_ACQUIRE_PERFORMANCE);
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_ACQUIRE_PERFORMANCE);
  EXPECT_EQ(d_task->run(0), ITask::State::TASK_RUN_ERROR);
  EXPECT_EQ(d_task->errors(), static_cast<ubyte4>(eError::BRAKE_ACQUIRING_HARDWARE_ERROR));
  EXPECT_EQ(d_task->getInitState(), InitState::CHECK_ACQUIRE_PERFORMANCE);

  EXPECT_EQ(asb::rte::FSM_INIT_CHECK_UP_OK::ro(), false);

  //------------------------------------------------------------------------------------------------------
}

}  // namespace asb::init
