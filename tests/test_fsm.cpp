#include <iostream>
#include <memory>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "types.h"

#include "fsm/fsm_task.hpp"
#include "rte/generated/rte.hpp"

FSM_INITIAL_STATE(asb::fsm::AsbLogic, asb::fsm::state::Hold);

using namespace brt::types;
using namespace asb::rte;

namespace asb::fsm
{

class FsmTaskTest : public ::testing::Test
{
protected:
  std::unique_ptr<FsmTask> d_task = nullptr;

  template <class T>
  void _writeRte(const condition &state, T value)
  {
    const_cast<condition &>(state) = value;
  }

  template <class T>
  void _writeRte(condition &state, T value)
  {
    const_cast<condition &>(state) = value;
  }

  void SetUp() final override
  {
    d_task = std::make_unique<FsmTask>();
    _writeRte(FSM_INIT_CHECK_UP_REQUEST::ro(), false);
    _writeRte(FSM_ASB_FINISH::ro(), false);
  }
};

}  // namespace asb::fsm
