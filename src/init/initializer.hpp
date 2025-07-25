#pragma once

#include "types.h"

#include "platform.hpp"

namespace asb
{
namespace init
{

using namespace brt::types;

class InitParamStorage
{
public:
  virtual pressure_Bar getAirStoragePressure_Bar() const
  {
    return asb::platform::io::getAirStoragePressure_Bar();
  }
  virtual pressure_Bar getFrontHydraulicPressure_Bar() const
  {
    return asb::platform::io::getFrontHydraulicPressure_Bar();
  }
  virtual pressure_Bar getRearHydraulicPressure_Bar() const
  {
    return asb::platform::io::getRearHydraulicPressure_Bar();
  }
  virtual brt::types::digitalState checkWatchdog() const
  {
    return asb::platform::io::readWatchdog();
  }
  virtual bool setHydraulicPressure_fraction(fraction val) const
  {
    return asb::platform::io::setHydraulicPressure_fraction(val);
  }

  pressure_Bar getMinAirPressure_Bar() const { return d_minAirPressure_Bar; }
  pressure_Bar getMaxAirPressure_Bar() const { return d_maxAirPressure_Bar; }
  pressure_Bar getMinHydraulicPressure_Bar() const { return d_minHydraulicPressure_Bar; }
  pressure_Bar getMaxHydraulicPressure_Bar() const { return d_maxHydraulicPressure_Bar; }

private:
  const pressure_Bar d_minAirPressure_Bar       = 6.0F;
  const pressure_Bar d_maxAirPressure_Bar       = 8.0F;
  const pressure_Bar d_minHydraulicPressure_Bar = 0.0F;
  const pressure_Bar d_maxHydraulicPressure_Bar = 50.0F;
};

}  // namespace init
}  // namespace asb
