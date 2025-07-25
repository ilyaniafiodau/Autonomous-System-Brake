#include "types.h"

#include "../io.hpp"

bool asb::platform::io::setValveVoltage_V(const brt::types::voltage_V voltage)
{
  return true;
}

brt::types::pressure_Bar asb::platform::io::getValvePneumoPressure_Bar()
{
  /// TODO: make Getting a virtual pressure signal via any service
  return 7.0F;
}

brt::types::pressure_Bar asb::platform::io::getAirStoragePressure_Bar()
{
  /// TODO: make Getting a virtual pressure signal via any service
  return 7.0F;
}

bool asb::platform::io::setHydraulicPressure_fraction(brt::types::fraction pressure_fraction)
{
  /// TODO: make setting a virtual pressure signal via any service
  return true;
}

brt::types::pressure_Bar asb::platform::io::getFrontHydraulicPressure_Bar()
{
  /// TODO: make detting a virtual pressure signal via any service
  return 30.0F;
}

brt::types::pressure_Bar asb::platform::io::getRearHydraulicPressure_Bar()
{
  /// TODO: make detting a virtual pressure signal via any service
  return 30.0F;
}

void asb::platform::io::toggleWatchdog() {}

brt::types::digitalState asb::platform::io::readWatchdog()
{
  return brt::types::digitalState::ON;
}

bool asb::platform::io::enableEbs() {
  return true;
}

bool asb::platform::io::disableEbs() {
  return true;
}

bool asb::platform::io::setSdcState(brt::types::CircuitState circuitState) {
  return true;
}

brt::types::CircuitState getSdcState() {
  return brt::types::CircuitState::UNDEFINED;
}



bool asb::platform::io::turnBrakeLightOn() {
  return true;
}

bool asb::platform::io::turnBrakeLightOff() {
  return true;
}