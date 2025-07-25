#include "fsm.hpp"

namespace asb
{
namespace fsm
{

static void resetEvent(const brt::types::condition &event)
{
  // Грязная вещь. Сами нарушаем правило, согласно которому, только одна задача имеет право
  // записывать в RTE.
  // В данном случае снимаем константность ссылки и сбрасываем запрос, чтобы не
  // заниматься этим в каждой задаче по отдельности.
  const_cast<brt::types::condition &>(event) = false;
}

/*-------------------------------------STATE Hold BEGIN--------------------------------------*/
void state::Hold::react(event::MissionRequest const &event)
{
  if (event.yes) {
    using namespace asb::fsm::state;
    transit<state::DriverlessSelected>();  // assume the mission is Driverless
    resetEvent(event.yes);
  }
}
/*-------------------------------------STATE Hold END----------------------------------------*/

/*-------------------------------------STATE DriverlessSelected BEGIN------------------------*/
void state::DriverlessSelected::react(event::InitCheckUpRequest const &event)
{
  if (event.yes) {
    using namespace asb::fsm::state;
    transit<state::DriverlessInit>();
    resetEvent(event.yes);
  }
}

void state::DriverlessSelected::react(asb::fsm::event::ResetAsb const &event)
{
  if (event.yes) {
    using namespace asb::fsm::state;
    transit<state::Hold>();
    resetEvent(event.yes);
  }
}
/*-------------------------------------STATE DriverlessSelected END--------------------------*/

/*-------------------------------------STATE DriverlessInit BEGIN----------------------------*/
void state::DriverlessInit::react(event::InitResultEmergency const &event)
{
  if (event.yes) {
    using namespace asb::fsm::state;
    transit<state::DriverlessSelected>();
    resetEvent(event.yes);
  }
}

void state::DriverlessInit::react(asb::fsm::event::InitResultOk const &event)
{
  if (event.yes) {
    using namespace asb::fsm::state;
    transit<state::DriverlessWaitForGo>();
    resetEvent(event.yes);
  }
}

void state::DriverlessInit::react(asb::fsm::event::ResetAsb const &event)
{
  if (event.yes) {
    using namespace asb::fsm::state;
    transit<state::Hold>();
    resetEvent(event.yes);
  }
}
/*-------------------------------------STATE DriverlessInit END------------------------------*/

/*-------------------------------------STATE DriverlessWaitForGo BEGIN-----------------------*/
void state::DriverlessWaitForGo::react(asb::fsm::event::GoSignal const &event)
{
  if (event.yes) {
    using namespace asb::fsm::state;
    transit<state::DriverlessRun>();
    resetEvent(event.yes);
  }
}

void state::DriverlessWaitForGo::react(asb::fsm::event::ResetAsb const &event)
{
  if (event.yes) {
    using namespace asb::fsm::state;
    transit<state::Hold>();
    resetEvent(event.yes);
  }
}
/*-------------------------------------STATE DriverlessWaitForGo END-------------------------*/

/*-------------------------------------STATE DriverlessRun BEGIN-----------------------------*/
void state::DriverlessRun::react(event::AsbFinish const &event)
{
  if (event.yes) {
    using namespace asb::fsm::state;
    transit<state::SafeState>();
    resetEvent(event.yes);
  }
}

void state::DriverlessRun::react(asb::fsm::event::ResetAsb const &event)
{
  if (event.yes) {
    using namespace asb::fsm::state;
    transit<state::Hold>();
    resetEvent(event.yes);
  }
}
/*-------------------------------------STATE DriverlessRun END-------------------------------*/

/*-------------------------------------STATE SafeState BEGIN---------------------------------*/
void state::SafeState::react(asb::fsm::event::ResetAsb const &event)
{
  if (event.yes) {
    using namespace asb::fsm::state;
    transit<state::Hold>();
    resetEvent(event.yes);
  }
}
/*-------------------------------------STATE SafeState END-----------------------------------*/

}  // namespace fsm
}  // namespace asb
