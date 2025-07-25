#pragma once

#include "types.h"

#include "platform.hpp"

namespace asb
{
namespace monitoring
{

using namespace brt::types;

class MonitoringIO
{
public:
  /**
 * @brief Activates EBS
 *
 * @return bool
 * @retval true, if digital 0 has been sent to EBS Actuator. 
 * Digital zero enables EBS, since EBS is normally open,
 * when it is not connected to voltage supply.
 * @retval false, if the signal to the EBS actuator has not been sent
 */
  virtual bool enableEbs() const {
    return asb::platform::io::enableEbs();
  }

  /*implement methods to send data to VCDU*/

};

}  // namespace brakeLight
}  // namespace asb