#include <array>

#include "mbed.h"

#include "../canbus.hpp"

namespace asb
{
namespace platform
{
namespace canbus
{
//----------------------PRIVATE PART BEGIN-----------------------
#include "circularbuffer.hpp"

static std::array<
  CircularBuffer<mbed::CANMessage, CAN_RX_QUEUE_SIZE>, static_cast<ubyte1>(CANChannel::CH_COUNT)>
  s_rxBuf;

static CAN s_can0(PB_8, PB_9, CAN_BAUD_RATE);
static CAN s_can1(PB_5, PB_6, CAN_BAUD_RATE);

static EventQueue s_eventCAN;
static Thread s_canThread;

static bool _keepMessage(const CANChannel ch, const mbed::CANMessage *msg)
{
  const size_t channel = static_cast<size_t>(ch);
  return s_rxBuf.at(channel).put(msg, 1);
}

static void _canRecv0_event()
{
  static mbed::CANMessage mbedMsg;
  if (s_can0.read(mbedMsg) == 1) {
    (void)_keepMessage(CANChannel::MAIN, &mbedMsg);
  }
}

static void _canRecv1_event()
{
  static mbed::CANMessage mbedMsg;
  if (s_can1.read(mbedMsg) == 1) {
    (void)_keepMessage(CANChannel::CONTROL, &mbedMsg);
  }
}

//----------------------PRIVATE PART END-------------------------

//----------------------PUBLIC PART BEGIN------------------------
bool init()
{
  s_canThread.start(callback(&s_eventCAN, &EventQueue::dispatch_forever));

  s_can0.reset();
  s_can1.reset();

  s_can0.attach(s_eventCAN.event(_canRecv0_event));
  s_can1.attach(s_eventCAN.event(_canRecv1_event));

  for (size_t i = 0; i < static_cast<size_t>(CANChannel::CH_COUNT); ++i) {
    s_rxBuf.at(i).reset();
  }

  return true;
}

bool recv(const CANChannel ch, CANMsg *const msg)
{
  bool res = false;

  const size_t channel = static_cast<uint16_t>(ch);
  if (true != s_rxBuf.at(channel).empty()) {
    static mbed::CANMessage mbedMsg;
    res = s_rxBuf.at(channel).get(1, &mbedMsg);
    if (res) {
      msg->id     = mbedMsg.id;
      msg->length = mbedMsg.len;
      memcpy(&msg[0], &mbedMsg.data[0], mbedMsg.len);
    }
  }
  return res;
}

bool send(const CANChannel ch, CANMsg *const msg)
{
  static mbed::CANMessage mbedMsg = mbed::CANMessage();

  mbedMsg.id  = msg->id;
  mbedMsg.len = msg->length;
  memcpy(&mbedMsg.data[0], &msg->data[0], mbedMsg.len);

  bool res = false;

  switch (ch) {
    case CANChannel::MAIN: {
      res = s_can0.write(mbedMsg);
      break;
    }
    case CANChannel::CONTROL: {
      res = s_can1.write(mbedMsg);
      break;
    }
    default: {
      /* No actions otherwise */
      break;
    }
  }

  return res;
}
//----------------------PUBLIC PART END--------------------------

}  // namespace canbus
}  // namespace platform
}  // namespace asb
