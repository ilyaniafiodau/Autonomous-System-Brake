#include "mbed.h"

#include "types.h"
#include "utils.h"

#include "../io.hpp"

using namespace brt::types;
using namespace asb::platform::io;

bool asb::platform::io::setValveVoltage_V(const brt::types::voltage_V voltage)
{
  static AnalogOut dac(PA_4);

  bool res = true;

  if (voltage >= 0.0F && voltage <= 3.3F) {
    dac.write(voltage / 3.3F);
  } else {
    res = false;
  }

  return res;
}

brt::types::pressure_Bar asb::platform::io::getValvePneumoPressure_Bar()
{
  static AnalogIn ain(PA_7);
  int sensor_output = static_cast<int>(ain.read());
  return utils::convert(sensor_output, 0, 0xA75, 0, 10);
}

brt::types::pressure_Bar asb::platform::io::getAirStoragePressure_Bar()
{
  static AnalogIn ain(PA_6);
  int sensor_output = static_cast<int>(ain.read());
  return utils::convert(sensor_output, 0, 0xA75, 0, 10);  // consult with Ilyukha
}

bool asb::platform::io::setHydraulicPressure_fraction(brt::types::fraction pressure_fraction)
{
  /// TODO: implemnt this driver
  return true;
}

brt::types::pressure_Bar asb::platform::io::getFrontHydraulicPressure_Bar()
{
  static AnalogIn ain(PB_0);
  int sensor_output = static_cast<int>(ain.read());
  return utils::convert(sensor_output, 0, 0xA75, 0, 100);
}

brt::types::pressure_Bar asb::platform::io::getRearHydraulicPressure_Bar()
{
  static AnalogIn ain(PB_1);
  int sensor_output = static_cast<int>(ain.read());
  return utils::convert(sensor_output, 0, 0xA75, 0, 100);
}

void asb::platform::io::toggleWatchdog()
{
  static DigitalOut dout(PA_1);
  dout = !dout;
}

brt::types::digitalState asb::platform::io::readWatchdog()
{
  static DigitalIn din(PA_0);

  using brt::types::digitalState;

  digitalState res = digitalState::UNDEFINED;

  const auto pinState = din.read();
  if (pinState == 0) {
    res = digitalState::OFF;
  } else if (pinState == 1) {
    res = digitalState::ON;
  }

  return res;
}

bool asb::platform::io::enableEbs()
{
  bool hasSentSignal = false;
  DigitalOut dout(PA_3);
  dout.write(0);  // EBS is activated (Normally Open) when the voltage supply is cut
  hasSentSignal = true;
  return hasSentSignal;
}

bool asb::platform::io::disableEbs()
{
  bool hasSentSignal = false;
  DigitalOut dout(PA_3);
  dout.write(1);  // EBS is disabled (Normally Closed) when the voltage supply is on
  hasSentSignal = true;
  return hasSentSignal;
}

bool asb::platform::io::setSdcState(brt::types::CircuitState desiredState)
{
  using brt::types::CircuitState;
  bool isStateSet           = false;
  CircuitState currentState = getSdcState();
  if (currentState == desiredState) {
    /*---do nothin'---*/
  } else {
    DigitalOut dout(PA_2);
    if (desiredState == CircuitState::CLOSED) {
      dout.write(1);
    } else if (desiredState == CircuitState::OPEN) {
      dout.write(0);
    } else {
      /*---do nothin'---*/
    }
  }

  /*---read SDC Status after setting a new state---*/
  currentState = getSdcState();
  if (currentState == desiredState) {
    isStateSet = true;
  } else {
    /*---do nothin'---*/
  }

  return isStateSet;
}

brt::types::CircuitState asb::platform::io::getSdcState()
{
  using brt::types::CircuitState;

  static DigitalIn din(PB_2);
  CircuitState res = CircuitState::UNDEFINED;

  const auto pinState = din.read();
  if (pinState == 0) {
    res = CircuitState::OPEN;
  } else if (pinState == 1) {
    res = CircuitState::CLOSED;
  } else {
    /* do nothing */
  }

  return res;
}

bool asb::platform::io::turnBrakeLightOn() {
  bool hasSentSignal = false;
  DigitalOut dout(PA_5);    
  dout.write(1);
  hasSentSignal = true;
  return hasSentSignal;
}

bool asb::platform::io::turnBrakeLightOff() {
  bool hasSentSignal = false;
  DigitalOut dout(PA_5);   
  dout.write(0);
  hasSentSignal = true;
  return hasSentSignal;
}