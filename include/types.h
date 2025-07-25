#pragma once

#ifdef __cplusplus
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <stdint.h>

#include "task_ids.hpp"

#define UNUSED_ARG [[maybe_unused]]

namespace brt
{
namespace types
{

using byte1  = int8_t;
using byte2  = int16_t;
using byte4  = int32_t;
using byte8  = int64_t;
using float4 = float;
using ubyte1 = uint8_t;
using ubyte2 = uint16_t;
using ubyte4 = uint32_t;
using ubyte8 = uint64_t;

using condition     = bool;    //!< Event type for the FSM
using current_A     = float4;  //!< Electric current in the SI system.
using fraction      = float4;  //!< Fraction [0.0F; 1.0F]
using percent       = float4;  //!< Percent [0.0F; 100.0F]
using pressure_Pa   = float4;  //!< Pressure in the SI system [Pascal].
using pressure_Bar  = float4;  //!< Pressure [Bar].
using temperature_C = byte1;   //!< Temperature in degrees Celcius.
using time_us       = ubyte8;  //!< Time in microseconds.
using torque_Nm     = float4;  //!< Torque in the SI system.
using voltage_V     = float4;  //!< Voltage in the SI system.
using CAN_STD_ID    = ubyte2;  //!< 11-bit-long standard CAN id

constexpr ubyte2 errorBitNumberPosition = 0U;
constexpr ubyte2 errorBitNumberMask     = 0b00000000'00011111U;
constexpr ubyte2 taskIdPosition       = 5U;
constexpr ubyte2 taskIdMask           = 0b00000011'11100000U;
constexpr ubyte2 deviceIdPosition     = 12U;
constexpr ubyte2 deviceIdMask         = 0b00111100'00000000U;

constexpr CAN_STD_ID asbErrorLoggerCanId = 0x010U;

enum class BrtDevice : ubyte1
{
  AMI        = 0U,
  AMS        = 1U,
  ASB        = 2U,
  ASSI       = 3U,
  CMUX_FRONT = 4U,
  CMUX_REAR  = 5U,
  EPS        = 6U,
  PRECHARGE  = 7U,
  VCDU       = 8U,
  VCU        = 9U,
};

enum class CircuitState : ubyte1
{
  UNDEFINED = 0U,
  OPEN      = 1U,
  CLOSED    = 2U
};

enum class digitalState : int8_t
{
  UNDEFINED = -1,  //!< Undefined GPIO state
  OFF       = 0,   //!< GPIO pin in logic `0`
  ON        = 1,   //!< GPIO pin in logic `1`
};

enum eBits8 : uint8_t
{
  BIT0 = 1U << 0,
  BIT1 = 1U << 1,
  BIT2 = 1U << 2,
  BIT3 = 1U << 3,
  BIT4 = 1U << 4,
  BIT5 = 1U << 5,
  BIT6 = 1U << 6,
  BIT7 = 1U << 7,

};

enum eBits16 : uint16_t
{
  BIT8  = 1U << 8,
  BIT9  = 1U << 9,
  BIT10 = 1U << 10,
  BIT11 = 1U << 11,
  BIT12 = 1U << 12,
  BIT13 = 1U << 13,
  BIT14 = 1U << 14,
  BIT15 = 1U << 15,
};

enum eBits32 : uint32_t
{
  BIT16 = 1U << 16,
  BIT17 = 1U << 17,
  BIT18 = 1U << 18,
  BIT19 = 1U << 19,
  BIT20 = 1U << 20,
  BIT21 = 1U << 21,
  BIT22 = 1U << 22,
  BIT23 = 1U << 23,
  BIT24 = 1U << 24,
  BIT25 = 1U << 25,
  BIT26 = 1U << 26,
  BIT27 = 1U << 27,
  BIT28 = 1U << 28,
  BIT29 = 1U << 29,
  BIT30 = 1U << 30,
  BIT31 = 1U << 31,
};

class ErrorCode {
  public:
    explicit ErrorCode(BrtDevice deviceId, asb::db::eTaskID taskId, ubyte1 errorBitNumber) : d_code(
      ((static_cast<ubyte2>(deviceId) << deviceIdPosition) & deviceIdMask) | 
      (((static_cast<ubyte2>(taskId) << taskIdPosition) & taskIdMask)) | 
      (((static_cast<ubyte2>(errorBitNumber) << errorBitNumberPosition) & errorBitNumberMask))) {}
      ubyte2 code() const {
        return d_code;
      }
  private:
    const ubyte2 d_code;
};



}  // namespace types
}  // namespace brt

#endif
