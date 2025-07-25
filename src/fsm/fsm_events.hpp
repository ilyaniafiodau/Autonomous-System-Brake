#pragma once

#include "types.h"

#include "tinyfsm.hpp"

namespace asb
{
namespace fsm
{
namespace event
{

using brt::types::condition;

/*---events ASB reacts to---*/

struct MissionRequest : public tinyfsm::Event {
  explicit MissionRequest(condition const &arg) : yes(arg) {}
  const condition yes;
};

struct GoSignal : public tinyfsm::Event {
  explicit GoSignal(condition const &arg) : yes(arg) {}
  const condition yes;
};

struct ResetAsb : public tinyfsm::Event {
  explicit ResetAsb(condition const &arg) : yes(arg) {}
  const condition yes;
};

struct AsbFinish : public tinyfsm::Event {
  explicit AsbFinish(condition const &arg) : yes(arg) {}
  const condition yes;
};

struct InitCheckUpRequest : public tinyfsm::Event {
  explicit InitCheckUpRequest(condition const &arg) : yes(arg) {}
  const condition yes;
};

struct InitResultEmergency : public tinyfsm::Event {
  explicit InitResultEmergency(condition const &arg) : yes(arg) {}
  const condition yes;
};

struct InitResultOk : public tinyfsm::Event {
  explicit InitResultOk(condition const &arg) : yes(arg) {}
  const condition yes;
};

}  // namespace event
}  // namespace fsm
}  // namespace asb
