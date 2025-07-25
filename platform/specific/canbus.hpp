#pragma once

#include "types.h"

namespace asb
{
namespace platform
{
namespace canbus
{

#define CAN_RX_QUEUE_SIZE (256U)
#define CAN_BAUD_RATE (500000U)

using namespace brt::types;

/**
 * @brief CAN Message class
 *
 */
class CANMsg
{
public:
  CANMsg() : data{}, length(0), id(0) {}
  ubyte1 data[8];
  ubyte1 length;
  ubyte4 id;
};

/**
 * @brief List of the available CAN channels
 *
 * @attention The channels must be numbered from 0U sequentially. The CH_COUNT constant must contain
 * the total number of channels.
 *
 */
enum class CANChannel : ubyte1
{
  MAIN     = 0U,
  CONTROL  = 1U,
  CH_COUNT = 2U
};

/**
 * @brief Enables all CAN bus sockets
 *
 * @return bool
 * @retval true if all CAN sockets have been initialized successfully
 * @retval false if at least one CAN socket has not been initialized
 */
bool init();

/**
 * @brief Receives the CAN message from RX buffer
 *
 * @param ch channel of the CAN bus to receive the message
 * @param[in] msg pointer to the CAN message
 * @return bool
 * @retval true if message has been read successfully
 * @retval false if message has not been read
 */
bool recv(const CANChannel ch, CANMsg *const msg);

/**
 * @brief Sends the CAN message
 *
 * @param ch channel of the CAN bus to send the message
 * @param[in] msg pointer to the CAN message
 * @return bool
 * @retval true if message has been sent successfully
 * @retval false if message has not been sent
 */
bool send(const CANChannel ch, CANMsg *const msg);

}  // namespace canbus
}  // namespace platform
}  // namespace asb
