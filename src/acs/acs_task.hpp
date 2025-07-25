#pragma once
#include <iostream>

#include "ITask.hpp"
#include "acs.hpp"
#include "rte/generated/rte.hpp"

namespace asb
{
namespace acs
{
using namespace asb::task;

enum class type_PID : uint8_t
{
  TORQUE = 0U,
  HYDRO,
  PNEUMO
};

enum class eError : uint8_t
{
  NO_ERRORS               = brt::types::BIT0,
  MECHANICAL_LINE_FAILURE = brt::types::BIT1,
  HYDRAULIC_LINE_FAILURE  = brt::types::BIT2,
  PNEUMATIC_LINE_FAILURE  = brt::types::BIT3,
};

ITask *createTask();

class AcsTask : public ITask
{
public:
  AcsTask(AcsIO *const acsIoPtr);

  /**
   * @brief Main runner for the task.
   * @param [in] time_us Time elapsed since previous run.
   * @return State = TASK_OK - everything is fine
   *                 TASK_RUN_ERROR - serious error on low level has occurred
   */
  State run(brt::types::time_us time_us) final;

  /**
   * @brief sets an error bit in the error register,
   *
   * @param [in] error ACS error.
   */
  void setAcsError(const asb::acs::eError error) { ITask::setError(static_cast<ubyte4>(error)); };

  /**
   * @brief Calculates the voltage value
   * required to be set via DAC
   * for the target torque to be reached
   *
   * @return voltage_V
   */
  voltage_V resolve(
    // clang-format off
      torque_Nm targetBrakeTorque_Nm,
      pressure_Bar currentFrontHydraulicPressure_Bar,
      pressure_Bar currentRearHydraulicPressure_Bar,
      pressure_Bar currentValvePneumoPressure_Bar) {
    // clang-format on
    const float4 d_rear_coeff    = 1.0;
    const float4 d_front_coeff   = 1.0;
    float4 currentBrakeTorque_Nm = d_rear_coeff * currentRearHydraulicPressure_Bar +
                                   d_front_coeff * currentFrontHydraulicPressure_Bar;
    float4 currentHydroPressure_Bar =
      currentRearHydraulicPressure_Bar + currentFrontHydraulicPressure_Bar;

    pressure_Bar setHydroValue_Bar =
      _controlViaPID(targetBrakeTorque_Nm, currentBrakeTorque_Nm, 0.0F, 800.0F, type_PID::TORQUE);
    pressure_Bar setPneumoValue_Bar =
      _controlViaPID(setHydroValue_Bar, currentHydroPressure_Bar, 0.0F, 40.0F, type_PID::HYDRO);
    pressure_Bar output_PID = _controlViaPID(
      setPneumoValue_Bar, currentValvePneumoPressure_Bar, 0.0F, 6.2F, type_PID::PNEUMO);

    return _convertToVoltage(output_PID);
  }

private:
  /**
   * @brief Object's pointer that stores acs io interface.
   *
   */
  AcsIO *const d_acsIoPtr;

  /**
   * @brief PID controller logic
   *
   * @return float4
   * @retval output_PID within the frame of [lower_limit; upper_limit]
   */
  float4 _controlViaPID(
    float4 set_point, float4 sensor_input, float4 lower_limit, float4 upper_limit, type_PID type)
  {
    constexpr float t_s           = 0.001F;
    float4 coeff_prop             = 0.0F;
    float4 coeff_int              = 0.0F;
    float4 coeff_deriv            = 0.0F;
    float4 scale                  = 1.0F;
    static float4 int_prev        = 0.0F;
    static float4 error_prev      = 0.0F;
    static float4 output_PID_prev = 0.0F;
    const bool shouldIncrease     = set_point > sensor_input;

    switch (type) {
      case type_PID::TORQUE:
        coeff_prop  = 80.0F;
        coeff_int   = 200.0F;
        coeff_deriv = 1.5F;
        scale       = 20.0F;
        break;
      case type_PID::HYDRO:
        coeff_prop  = 4.8F;
        coeff_int   = 31.89F;
        coeff_deriv = 0.1806F;
        scale       = 6.45F;
        break;
      case type_PID::PNEUMO:
        coeff_prop  = 30.0F;
        coeff_int   = 923.0F;
        coeff_deriv = 0.243F;
        break;
    }

    float4 error = set_point - sensor_input;
    if (type == type_PID::PNEUMO && std::abs(error) <= 0.1F) {
      error = 0.0F;
    }
    float4 proportional = error;
    float4 integral     = int_prev + error * t_s;
    float4 derivative   = (error - error_prev) / t_s;

    if (
      ((proportional + integral + derivative) > upper_limit) ||
      ((proportional + integral + derivative) < lower_limit))
    {
      integral = int_prev;
    }

    float4 output_PID =
      (coeff_prop * proportional + coeff_int * integral + coeff_deriv * derivative) / scale;

    if (
      (shouldIncrease && !_isGreater(output_PID, output_PID_prev)) ||
      (!shouldIncrease && _isGreater(output_PID, output_PID_prev)))
    {
      switch (type) {
        case type_PID::TORQUE:
          setAcsError(eError::MECHANICAL_LINE_FAILURE);
          asb::rte::ACS_ERROR::rw() = brt::types::BIT1;
          break;
        case type_PID::HYDRO:
          setAcsError(eError::HYDRAULIC_LINE_FAILURE);
          asb::rte::ACS_ERROR::rw() = brt::types::BIT2;
          break;
        case type_PID::PNEUMO:
          setAcsError(eError::PNEUMATIC_LINE_FAILURE);
          asb::rte::ACS_ERROR::rw() = brt::types::BIT3;
          break;
      }
    } else {
      // do nothing
    }

    output_PID_prev = output_PID;
    int_prev        = integral;
    error_prev      = error;

    return std::min(std::max(output_PID, lower_limit), upper_limit);
  }

  /**
   * @brief Converts the PID controller output to voltage
   *
   * @return voltage_V
   */
  voltage_V _convertToVoltage(float4 output_PID) { return ((output_PID / 1.89F) * 4095.0F) / 3.3F; }

  /**
   * @brief Returns true if the first float argument
   * is greater than the second considering the 0.01F deadband
   *
   * @return bool
   */
  bool _isGreater(float4 a, float4 b)
  {
    const float4 computationalError = 0.01F;
    const float4 difference         = a - b;
    return (difference > computationalError);
  }
};
}  // namespace acs
}  // namespace asb
