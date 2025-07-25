#pragma once

#include <type_traits>

#include "platform.hpp"
#include "task_ids.hpp"
#include "types.h"

namespace asb::task
{
/** \defgroup Tasks Tasks, running on the system */

/**  \ingroup Tasks
    \interface ITask
    \brief Abstract class for all processes

    Declares interface for all processes, running on the device.
*/
class ITask
{
public:
  /* -------------------------------------------------------------------------- */
  /*                               Helper types                                 */
  /* -------------------------------------------------------------------------- */

  /**  Return value for process after execution     */
  enum class State : uint8_t
  {
    TASK_OK = 0U,          //!< Everything is fine */
    TASK_INIT_ERROR,  //!< An error occurred during task initialization */
    TASK_RUN_ERROR    //!< An error occurred during task execution */
  };

  /**
   * @brief This struct helps to avoid argument collisions at ITask constructors.
   */
  struct ErrorMask {
    constexpr ErrorMask(const ErrorMask &) = default;
    constexpr explicit ErrorMask(uint32_t arg) : mask(arg) {}
    uint32_t mask = 0;
  };

  /* -------------------------------------------------------------------------- */
  /*                                Constructors                                */
  /* -------------------------------------------------------------------------- */
  /**
   * @brief Construct a new ITask object with zero error mask
   * @param [in] id       - ID of task.
   */
  constexpr explicit ITask(asb::db::eTaskID id) : d_id(id) {}

  /**
   * @brief Construct a new ITask object with static error mask
   * @param [in] id       - ID of task.
   * @param [in] driverlessMask - New \c ubyte4 error mask for driverless mode
   * @param [in] manualMask - New \c ubyte4 error mask for manual mode
   */
  constexpr ITask(asb::db::eTaskID id, const uint32_t driverlessMask, const uint32_t manualMask)
  : d_id(id), d_driverlessErrorMask(driverlessMask), d_manualErrorMask(manualMask)
  {
  }

  /* -------------------------------------------------------------------------- */
  /*                                Main routines                               */
  /* -------------------------------------------------------------------------- */

  /** \brief Task initialization routine. Normally, redefined in each task. The Init function is
     called by scheduler when task is registered. \return State of the task \retval TASK_OK - Normal
     operation \retval TASK_INIT_ERROR  - Critical error during initialization
  */
  virtual State init() { return d_state; }

  /** \brief Main function for process execution. Normally it is called periodically by scheduler.
      \param time - time now
      \return State
      \retval TASK_OK         - Normal operation
      \retval TASK_RUN_ERROR  - Critical error
  */
  virtual State run(brt::types::time_us time) = 0;

  /* -------------------------------------------------------------------------- */
  /*                                   Getters                                  */
  /* -------------------------------------------------------------------------- */
  /**
   * \brief Get state of the task
   * @return State -Task state
   */
  State state() const { return d_state; }

  /**
   * @brief Get the error register of the task.
   * @return Error register.
   */
  virtual uint32_t errors() const { return d_errorRegister; }

  /**
   * @brief Get critical errors mask for the task active in a driverless mode.
   * @return Error mask.
   */
  const ErrorMask driverlessErrorMask() const { return d_driverlessErrorMask; }

  /**
   * @brief Get critical errors mask for the task active in a manual mode.
   * @return Error mask.
   */
  const ErrorMask manualErrorMask() const { return d_manualErrorMask; }

  /**
   * @brief Get ID of the task.
   * @return \c eTaskID of task.
   */
  asb::db::eTaskID id() const { return d_id; }

  /* -------------------------------------------------------------------------- */
  /*                              Error management                              */
  /* -------------------------------------------------------------------------- */
  /** \brief Set an error bit in the task error register
   *
   * @tparam T - type of the flag to set (should be uint32_t)
   * \param err - bit mask to set
   */
  template <typename T>
  void setError(T err)
    requires std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> ||
             std::is_same_v<T, uint32_t>
  {
    d_errorRegister |= static_cast<uint32_t>(err);
  }

  /** \brief Clear an error bit in the task error register
   *
   * @tparam T - type of the flag to set (should be uint32_t)
   * \param err - bit mask to clear
   */
  template <typename T>
  void clearError(T err)
    requires std::is_same_v<T, uint8_t> || std::is_same_v<T, uint16_t> ||
             std::is_same_v<T, uint32_t>
  {
    d_errorRegister &= ~static_cast<uint32_t>(err);
  }

  /**
   * @brief Clear critical errors of the task
   */
  virtual void clearCriticalErrors()
  {
    d_errorRegister &= ~(d_driverlessErrorMask.mask);
    d_errorRegister &= ~(d_manualErrorMask.mask);
  }

  /**
   * @brief Clear non-critical errors of the task
   */
  virtual void clearNonCriticalErrors()
  {
    d_errorRegister &= d_driverlessErrorMask.mask;
    d_errorRegister &= d_manualErrorMask.mask;
  }

  /**
   * @brief Clear all errors of the task.
   */
  virtual void clearAllErrors() { d_errorRegister = 0; }

  virtual ~ITask()                = default;
  ITask(ITask const &)            = delete;
  ITask &operator=(ITask const &) = delete;

  /* -------------------------------------------------------------------------- */
  /*                                   Private                                  */
  /* -------------------------------------------------------------------------- */
private:
  const asb::db::eTaskID d_id;                //!< Task ID
  const ErrorMask d_driverlessErrorMask = ErrorMask(0U);  //!< Task's mutable critical error mask (driverless mode)
  const ErrorMask d_manualErrorMask = ErrorMask(0U); //!< Task's mutable critical error mask (manual mode)

protected:
  uint32_t d_errorRegister = 0;               //!< Task's error register
  State d_state            = State::TASK_OK;  //!< State of the task
};
}  // namespace asb::task
