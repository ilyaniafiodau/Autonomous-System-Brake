#pragma once

#include "fsm_events.hpp"
#include "fsm_states.hpp"
#include "tinyfsm.hpp"

namespace asb
{
namespace fsm
{

/** \addtogroup CommonLogic AsbLogic
 *  \brief AsbLogic is a State Machine to process algorithms
 * \ingroup Tasks
 */

/**
 * \class AsbLogic
 * \ingroup CommonLogic
 * \brief AsbLogic is a Mealy State Machine to process algorithms
 *
 * \details Mealy State Machine is used as a base
 *
 */
class AsbLogic : public tinyfsm::MealyMachine<AsbLogic>
{
public:
  virtual void react(event::MissionRequest const &) {}
  virtual void react(event::GoSignal const &) {}
  virtual void react(event::ResetAsb const &) {}
  virtual void react(event::AsbFinish const &) {}
  virtual void react(event::InitCheckUpRequest const &) {}
  virtual void react(event::InitResultEmergency const &) {}
  virtual void react(event::InitResultOk const &) {}
};

class state::Init : public AsbLogic
{
  //  asb is doin' nothin'
};

/// @
class state::Hold : public AsbLogic
{
public:
  void react(event::MissionRequest const &event) final override;
};

class state::ManualSelected : public AsbLogic
{
public:
};

class state::ManualRun : public AsbLogic
{
public:
};

class state::DriverlessSelected : public AsbLogic
{
public:
  void react(event::InitCheckUpRequest const &event) final override;
  void react(event::ResetAsb const &event) final override;
};

class state::DriverlessInit : public AsbLogic
{
public:
  void react(event::InitResultEmergency const &event) final override;
  void react(event::InitResultOk const &event) final override;
  void react(event::ResetAsb const &event) final override;
};

class state::DriverlessWaitForGo : public AsbLogic
{
public:
  void react(event::GoSignal const &event) final override;
  void react(event::ResetAsb const &event) final override;
};

class state::DriverlessRun : public AsbLogic
{
public:
  void react(event::AsbFinish const &event) final override;
  void react(event::ResetAsb const &event) final override;
};

class state::SafeState : public AsbLogic
{
public:
  void react(event::ResetAsb const &event) final override;
};

}  // namespace fsm
}  // namespace asb
