#pragma once

#include "types.h"
#include "platform.hpp"

namespace asb
{
namespace supervisor
{
using namespace brt::types;

class Supervisor
{
public:
  Supervisor() {}

  Supervisor(const Supervisor &)             = delete;
  Supervisor(const Supervisor &&)            = delete;
  Supervisor &operator=(const Supervisor &)  = delete;
  Supervisor &operator=(const Supervisor &&) = delete;

  /**
   * @brief Calls hardware driver to set the state of the SDC circuit,
   * depending on the argument passed, it opens or closes the SDC circuit
   *
   * @param [in] CircuitState which may be OPEN, CLOSED, or UNDEFINED
   */
  virtual bool setShutdownState(brt::types::CircuitState circuitState) const {
    return asb::platform::io::setSdcState(circuitState);
  }

  virtual bool sendError(ubyte2 error) const {
    using namespace asb::platform::canbus;

    CANMsg message = {};

    message.id = asbErrorLoggerCanId;
    message.length = 2U;
    message.data[0U] = static_cast<ubyte1>(error >> 8U);
    message.data[1U] = static_cast<ubyte1>(error);

    return send(CANChannel::MAIN, &message);
  }
};
}  // namespace supervisor
}  // namespace asb