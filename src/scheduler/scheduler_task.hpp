#pragma once

#include <vector>

#include "ITask.hpp"

namespace asb
{
namespace scheduler
{
using namespace asb::task;
/** \addtogroup scheduler Scheduler
 *  \brief Scheduler decides, which tasks need to run
 * \ingroup Tasks
 */

/**
 * \class Scheduler
 * \ingroup scheduler
 * \brief Scheduler decides, which tasks need to run
 *
 * \details Runs in while (1) and provides periodic call of all tasks.
 * Tasks may have different periods, that are set during initialisation.
 *
 */
class Scheduler : public asb::task::ITask
{
public:
  Scheduler();

  /*! \brief Adds a task to the scheduler
        \param task - the executable task
        \param period -  task execution period
        \return Task_OK normally
             */
  State registerTask(asb::task::ITask *const task, brt::types::time_us period);

  /*! \brief main function for process execution

        Implementation of asb::task::ITask::run

        \param time - time elapsed since previous run
        \return Task_OK - normal operation
                Task_RunError - critical error
    */
  State run(brt::types::time_us time) final;

  Scheduler(Scheduler const &)            = delete;
  Scheduler &operator=(Scheduler const &) = delete;

private:
  /*! \brief Holds timer data for each task    */
  struct TaskRecord {
    /*! \brief Default constructor    */
    TaskRecord(asb::task::ITask *task, brt::types::time_us period)
    : task(task), period_us(period), lastCall_us(0)
    {
    }

    asb::task::ITask *const task;   /*! A task, for which the record is valid */
    brt::types::time_us period_us;   /*! Task calling period [us] */
    brt::types::time_us lastCall_us; /*! Last call moment [us] */
  };

  /*! \brief Stores data about all tasks    */
  std::vector<TaskRecord> d_tasks;

  /**
   * \brief Maximum count of tasks, registered to the scheduler
   */
  const size_t d_maxTaskNumber = 32;
};

}  // namespace scheduler
}  // namespace asb
