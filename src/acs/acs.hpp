#pragma once

#include "types.h"

#include "platform.hpp"

namespace asb
{
namespace acs
{

using namespace brt::types;

class AcsIO
{
public:
/**
   * @brief returns pressure value in the pneumatic line 
   * measured at the output of the proportional valve
   * 
   * @return brt::types::pressure_Bar
   */
  virtual pressure_Bar getValvePneumoPressure_Bar() const
  {
    return asb::platform::io::getValvePneumoPressure_Bar();
  }

  /**
   * @brief returns pressure value in the hydraulic line 
   * measured at the front axis
   * 
   * @return brt::types::pressure_Bar
   */
  virtual pressure_Bar getFrontHydraulicPressure_Bar() const
  {
    return asb::platform::io::getFrontHydraulicPressure_Bar();
  }

  /**
   * @brief returns pressure value in the hydraulic line 
   * measured at the rear axis
   * 
   * @return brt::types::pressure_Bar
   */
  virtual pressure_Bar getRearHydraulicPressure_Bar() const
  {
    return asb::platform::io::getRearHydraulicPressure_Bar();
  }
  
   /**
   * @brief returns true if the input voltage is less than 3.3F but greater than 0.0F,
   * passes voltage to DAC, else returns false
   * 
   * @return bool
   * 
   * @retval true if voltage has been set successfully
   * @retval false if voltage has not been set due to incorrect input parameters
   * 
   * @param [in] voltage_V [0; 3.3V]
   */
  virtual bool setValveVoltage(const voltage_V voltage) const
  {
    return asb::platform::io::setValveVoltage_V(voltage);
  }
};

}  // namespace acs
}  // namespace asb
