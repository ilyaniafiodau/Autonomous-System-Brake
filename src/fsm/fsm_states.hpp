#pragma once

namespace asb
{
namespace fsm
{
namespace state
{

/*---ASB states---*/
class Init;  // asb's own initialization

class Hold;  // before events

/// @brief Driverless mission has been selected.
class DriverlessSelected;

/// @brief Init state for driverless mode, i.e. initial check-up.
class DriverlessInit;

/// @brief State that waits the Go signal from VCDU.
class DriverlessWaitForGo;

/// @brief Once the Go signal is received from VCDU.
class DriverlessRun;

/// @brief Manual mission has been selected.
class ManualSelected;

/// @brief Manual mission state. It does nothing usually.
class ManualRun;

class SafeState;  // Finish.

}  // namespace state
}  // namespace fsm
}  // namespace asb