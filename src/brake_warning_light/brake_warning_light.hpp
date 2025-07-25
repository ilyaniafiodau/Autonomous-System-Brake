#pragma once

#include "types.h"

#include "platform.hpp"

namespace asb
{
namespace brakeLight
{

using namespace brt::types;

class BrakeLightIO
{
public:
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
   * @brief sets the brake warning light ON
   * 
   * @return bool
   * 
   * @retval true if the ON signal has been sent to pin
   * @retval false otherwise
   */
  virtual bool turnBrakeLightOn() const //does it really have to be VIRTUAL????
  {
    return asb::platform::io::turnBrakeLightOn();
  }

    /**
   * @brief sets the brake warning light OFF
   * 
   * @return bool
   * 
   * @retval true if the OFF signal has been sent to pin
   * @retval false otherwise
   */
  virtual bool turnBrakeLightOff() const //does it really have to be VIRTUAL????
  {
    return asb::platform::io::turnBrakeLightOff();
  }
};

}  // namespace brakeLight
}  // namespace asb