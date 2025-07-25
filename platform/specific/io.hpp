#pragma once

#include "types.h"

namespace asb
{
namespace platform
{
namespace io
{

bool setValveVoltage_V(const brt::types::voltage_V voltage);

bool turnBrakeLightOn();

bool turnBrakeLightOff();

/**
 * @brief Measures an actual air valve pressure and returns it.
 *
 * @return brt::types::pressure_Bar
 * @retval air pressure [0.0F; ...]
 */
brt::types::pressure_Bar getValvePneumoPressure_Bar();

/**
 * @brief Measures an actual air storage pressure and returns it.
 *
 * @return brt::types::pressure_Bar
 * @retval air pressure [0.0F; ...]
 */
brt::types::pressure_Bar getAirStoragePressure_Bar();

/**
 * @brief Sets a relative hydraulic pressure without real values. Only values [0.0F; 1.0F] are used.
 *
 * @param [in] pressure_fraction Target relative pressure [0.0F; 1.0F].
 *
 * @return bool
 * @retval true if pressure has been sent successfully.
 * @retval false if pressure has not been sent.
 */
bool setHydraulicPressure_fraction(brt::types::fraction pressure_fraction);

/**
 * @brief Measures an actual front hydraulic pressure and returns it.
 *
 * @return brt::types::pressure_Bar
 * @retval hydraulic pressure [0.0F; ...]
 */
brt::types::pressure_Bar getFrontHydraulicPressure_Bar();

/**
 * @brief Measures an actual rear hydraulic pressure and returns it.
 *
 * @return brt::types::pressure_Bar
 * @retval hydraulic pressure [0.0F; ...]
 */
brt::types::pressure_Bar getRearHydraulicPressure_Bar();

/**
 * @brief Function toggles the watchdog's input pin. Hardware PWM driver shouldn't be used, because
 * the main goal of watchdog is checking of correctly working MCU's program. Should be call
 * periodically correcponding the external hardware watchdog.
 */
void toggleWatchdog();

/**
 * @brief Measures an actual watchdog's state returns it.
 *
 * @return brt::types::digitalState
 * @retval OFF if watchdog doesn't work. Error.
 * @retval ON if watchdog works. OK.
 */
brt::types::digitalState readWatchdog();

/**
 * @brief Activates EBS
 *
 * @return bool
 * @retval true, if digital 0 has been sent to EBS Actuator. 
 * Digital zero enables EBS, since EBS is normally open,
 * when it is not connected to voltage supply.
 * @retval false -- if the signal has bot been sent.
 */
bool enableEbs();

/**
 * @brief De-activates EBS
 *
 * @return bool
 * @retval true, if digital 1 has been sent to EBS Actuator. 
 * Digital one disables EBS, since EBS is normally closed,
 * when the power source is active.
 * @retval false -- if the signal has bot been sent.
 */
bool disableEbs();

/**
 * @brief Calls hardware driver to set the state of the SDC circuit,
 * depending on the argument passed, it opens or closes the SDC circuit
 *
 * @param [in] CircuitState which is either OPEN, or CLOSED
 * 
 * @retval bool: true, if the desired signal has been sent, otherwise -- false
 */
bool setSdcState(brt::types::CircuitState circuitState);

/**
 * @brief Returns current SDC state
 * by reading the SDC Status pin under the hood
 * 
 * @retval CircuitState:
 * SDC is closed, if the SDC Status pin returns 1
 * SDC is open, is the SDC Status pin returns 0
 */
brt::types::CircuitState getSdcState();
}  // namespace io
}  // namespace platform
}  // namespace asb
