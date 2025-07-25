#include <iostream>
#include <string>

#include "../canbus.hpp"

namespace asb
{
namespace platform
{
namespace canbus
{
//----------------------PUBLIC PART BEGIN------------------------
#include "socketcan/socketcan.h"

static SocketCANFD s_can0 = 0;
static SocketCANFD s_can1 = 0;
static SocketCANFD s_can2 = 0;

bool init()
{
  bool res = false;

  s_can0 = socketcanOpen(static_cast<const char *>("vcan0"), false);
  s_can1 = socketcanOpen(static_cast<const char *>("vcan1"), false);
  s_can2 = socketcanOpen(static_cast<const char *>("vcan2"), false);

  std::string errorMessage = "";
  if (s_can0 < 0) {
    (void)errorMessage.append(static_cast<const char *>("Error opening the vcan0 socket\n\r"));
  }
  if (s_can1 < 0) {
    (void)errorMessage.append(static_cast<const char *>("Error opening the vcan1 socket\n\r"));
  }
  if (s_can2 < 0) {
    (void)errorMessage.append(static_cast<const char *>("Error opening the vcan2 socket\n\r"));
  }

  if (errorMessage.empty()) {
    res = true;
  } else {
    std::cerr << errorMessage << std::endl;
    exit(-1);
  }

  return res;
}

bool recv(const CANChannel ch, CANMsg *const msg)
{
  CanardFrame frame;
  int16_t ok = 0;
  switch (ch) {
    case CANChannel::MAIN: {
      ok = socketcanPop(s_can0, &frame, NULL, 8, &msg->data[0], 0, NULL);
      break;
    }
    case CANChannel::CONTROL: {
      ok = socketcanPop(s_can1, &frame, NULL, 8, &msg->data[0], 0, NULL);
      break;
    }
    default: {
      /* No actions otherwise */
      break;
    }
  }

  bool res = false;

  if (1 == ok) {
    msg->id     = frame.extended_can_id;
    msg->length = frame.payload_size;
    res         = true;
  }

  return res;
}

bool send(const CANChannel ch, CANMsg *const msg)
{
  CanardFrame frame;
  frame.extended_can_id = msg->id;
  frame.payload         = &msg->data[0];
  frame.payload_size    = msg->length;

  int16_t ok = 0;
  switch (ch) {
    case CANChannel::MAIN: {
      ok = socketcanPush(s_can0, &frame, 0);
      break;
    }
    case CANChannel::CONTROL: {
      ok = socketcanPush(s_can1, &frame, 0);
      break;
    }
    default: {
      /* No actions otherwise */
      break;
    }
  }

  bool res = false;

  if (1 == ok) {
    res = true;
  }

  return res;
}
//----------------------PUBLIC PART END--------------------------

}  // namespace canbus
}  // namespace platform
}  // namespace asb
