/**
 * @file TaskCPP.h
 * @brief FreeRTOS Task Wrapper
 *
 * This file contains a set of lightweight wrappers for tasks using FreeRTOS
 * 
 * @copyright (c) 2007-2024 Richard Damon
 * @author Richard Damon <richard.damon@gmail.com>
 * @parblock
 * MIT License:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * It is requested (but not required by license) that any bugs found or
 * improvements made be shared, preferably to the author.
 * @endparblock
 *
 * @ingroup FreeRTOSCpp
 * 
 * Tasks are built with a selction of options:
 * 
 * Task Function Type:
 * + Simple void fun(void* parm);
 * + task memberfuncion of a derived class
 * 
 * TasK Allocation:
 * + Static (specified by a template parameter)
 * + Dynamic (specified by that template parameter being 0)
 * + Static with suppied stack 
 * 
 * @todo Look at options to create "restricted" tasks with xTaskCreateRestricted
 * Basically needs another parrallel construciton of this tree.
 */

#ifndef TaskCPP_H
#define TaskCPP_H

#include "FreeRTOScpp.h"

extern "C" {
	extern void taskcpp_task_thunk(void*);
}

#if FREERTOSCPP_USE_NAMESPACE
namespace FreeRTOScpp {
#endif


/**
 * @brief Names for Base set of Priorities.
 *
 * Assigns used based names to priority levels, optimized for configMAX_PRIORITIES = 6 for maximal distinctions
 * with reasonable collapsing for smaller values. If configMAX_PRIORITIES is >6 then some values won't have names here, but
 * values could be created and cast to the enum type.
 *
 * | configMAX_PRIORITIES: | 1 | 2 | 3 | 4 | 5 | 6 | N>6 | Use                                                |
 * | --------------------: | - | - | - | - | - | - | :-: | :------------------------------------------------- |
 * | TaskPrio_Idle         | 0 | 0 | 0 | 0 | 0 | 0 |  0  | Non-Real Time operations, Tasks that don't block   |
 * | TaskPrio_Low          | 0 | 1 | 1 | 1 | 1 | 1 |  1  | Non-Critical operations                            |
 * | TaskPrio_HMI          | 0 | 1 | 1 | 1 | 1 | 2 |  2  | Normal User Interface                              |
 * | TaskPrio_Mid          | 0 | 1 | 1 | 2 | 2 | 3 | N/2 | Semi-Critical, Deadlines, not much processing      |
 * | TaskPrio_High         | 0 | 1 | 2 | 3 | 3 | 4 | N-2 | Urgent, Short Deadlines, not much processing       |
 * | TaskPrio_Highest      | 0 | 1 | 2 | 3 | 4 | 5 | N-1 | Critical, do NOW, must be quick (Used by FreeRTOS) |
 *
 * @ingroup FreeRTOSCpp 
 */
enum TaskPriority {
	TaskPrio_Idle = 0,													///< Non-Real Time operations. tasks that don't block
	TaskPrio_Low = ((configMAX_PRIORITIES)>1),                         	///< Non-Critical operations
	TaskPrio_HMI = (TaskPrio_Low + ((configMAX_PRIORITIES)>5)),			///< Normal User Interface Level
	TaskPrio_Mid = ((configMAX_PRIORITIES)/2),							///< Semi-Critical, have deadlines, not a lot of processing
	TaskPrio_High = ((configMAX_PRIORITIES)-1-((configMAX_PRIORITIES)>4)), ///< Urgent tasks, short deadlines, not much processing
	TaskPrio_Highest = ((configMAX_PRIORITIES)-1) 						///< Critical Tasks, Do NOW, must be quick (Used by FreeRTOS)
};

/**
 * Allow adjment to Task Priority. 
 * 
 * Mostly for configMAX_PRIORITIES > 6
 */
constexpr TaskPriority operator+(TaskPriority p, int offset) {
	configASSERT((static_cast<int>(p) + offset) >=  0);
	configASSERT((static_cast<int>(p) + offset) <  configMAX_PRIORITIES);
	return static_cast<TaskPriority>(static_cast<int>(p) + offset);
}

/**
 * Allow adjment to Task Priority. 
 * 
 * Mostly for configMAX_PRIORITIES > 6
 */
constexpr TaskPriority operator-(TaskPriority p, int offset) {
	configASSERT((static_cast<int>(p) - offset) >=  0);
	configASSERT((static_cast<int>(p) - offset) <  configMAX_PRIORITIES);
	return static_cast<TaskPriority>(static_cast<int>(p) - offset);
}

/**
 * @brief Lowest Level Wrapper.
 * Create the specified task with a provided task function.
 *
 * If the TaskBase object is destroyed, the FreeRTOS Task will be deleted (if deletion has been enabled)
 * @ingroup FreeRTOSCpp
 * @todo Fully implemwent task manipulation functions
 *
 * @ingroup FreeRTOSCpp
 */

class TaskBase {
protected:
	/**
	 * @brief Default Constructor: Needs a subclass to fill in the handle later, so protected.
	*/

	TaskBase() : taskHandle(nullptr) {}

public:
	/**
	 * @brief Constructor
	 *
	 */
	TaskBase(TaskHandle_t handle) : taskHandle(handle) {

	}

	  /**
	   * @brief Destructor.
	   *
	   * If deletion is enabled, delete the task.
	   */
	  virtual ~TaskBase() {
#if INCLUDE_vTaskDelete
		if(taskHandle){
		    vTaskDelete(taskHandle);
		}
#endif
	    return;
	  }
	  /**
	   * @brief Get Task Handle.
	   * @return the task handle.
	   */
	  TaskHandle_t getTaskHandle() const { return taskHandle; }
      /**
       * @brief Delay for a period of time
       * @param time the number of ticks to delay
       *
       * This is a static member function as it affects the CALLING task, not the task it might be called on
       */
      static void   delay(TickType_t time) { vTaskDelay(time); }
#if FREERTOSCPP_USE_CHRONO
      /**
       * @brief Delay for a period of time, specified in milliseconds
       * @param ms The number of milliseconds to delay
       *
       * This is a static member function as it affects the CALLING task, not the task it might be called on
       */
       static void   delay(Time_ms ms) { vTaskDelay(ms2ticks(ms)); }
#endif

#if INCLUDE_xTaskDelayUntil
      /**
       * @brief Delay a periodic task
       * @param prev The time the last unblock was scheduled to happen
       * @param time The time in ticks to unblock next. If all calls use the same value, this will cause the task to run at
       * the given period with long term stablity.
       * @returns true if delay happend (false if fallen behind and no delay occured)
       *
       * Delays the task until the time mark prev + time.
       *
       * @code
       * TickType_t last = xTaskGetTickCount()
       * while(1) {
       *     // Wait for the next cycle
       *     bool delayed = delayUntil(last, 5);
       *     // Do the work for the cycle
       *     // if delayed is false, we have fallen behind, so delay didn't actually delay any time
       *     // so possibly skip some processing
       * }
       * @endcode
       */
      static bool   delayUntil(TickType_t& prev, TickType_t time) { return xTaskDelayUntil(&prev, time);}
#if FREERTOSCPP_USE_CHRONO
      /**
       * @brief Delay a periodic task
       * @param prev The time the last unblock was scheduled to happen
       * @param ms The time in milli-seconds to unblock next. If all calls use the same value, this will cause the task to run at
       * the given period with long term stablity.
       * @returns true if delay happend (false if fallen behind and no delay occured)
       *
       * Delays the task until the time mark prev + ms.
       *
       * @code
       * TickType_t last = xTaskGetTickCount()
       * while(1) {
       *     // Wait for the next cycle
       *     bool delayed = delayUntil(last, 100_ms);
       *     // Do the work for the cycle
       *     // if delayed is false, we have fallen behind, so delay didn't actually delay any time
       *     // so possibly skip some processing
       * }
       * @endcode
       */
      static bool   delayUntil(TickType_t& prev, Time_ms ms) { return xTaskDelayUntil(&prev, ms2ticks(ms)); }
#endif
#elif INCLUDE_vTaskDelayUntil
    /**
       * @brief Delay a periodic task
       * @param prev The time the last unblock was scheduled to happen
       * @param time The time in ticks to unblock next. If all calls use the same value, this will cause the task to run at
       * the given period with long term stablity.
       *
       * Delays the task until the time mark prev + time.
       *
       * @code
       * TickType_t last = xTaskGetTickCount()
       * while(1) {
       *     // Wait for the next cycle
       *     delayUntil(last, 5);
       *     // Do the work for the cycle
       * }
       * @endcode
       */
      static void   delayUntil(TickType_t& prev, TickType_t time) { vTaskDelay(&prev, time); }
#if FREERTOSCPP_USE_CHRONO
    /**
       * @brief Delay a periodic task
       * @param prev The time the last unblock was scheduled to happen
       * @param ms The time in milli-seconds to unblock next. If all calls use the same value, this will cause the task to run at
       * the given period with long term stablity.
       *
       * Delays the task until the time mark prev + ms.
       *
       * @code
       * TickType_t last = xTaskGetTickCount()
       * while(1) {
       *     // Wait for the next cycle
       *     delayUntil(last, 5);
       *     // Do the work for the cycle
       * }
       * @endcode
       */
      static void   delayUntil(TickType_t& prev, Time_ms ms) { vTaskDelay(&prev, ms2ticks(ms)); }
#endif
#endif

#if INCLUDE_xTaskAbortDelay
        /**
         * @brief Abort the Delay of a Task.
         * @returns true if the task was blocking.
         */
        bool abortDelay() { return xTaskAbortDelay(taskHandle); }
#endif

#if INCLUDE_uxTaskPriorityGet
	  /**
	   * @brief Get Task priority
	   *
	   * Only available if INCLUDE_vTaskPriorityGet == 1
	   * @return The priority of the Task.
	   */
	  TaskPriority priority() const { return static_cast<TaskPriority>(uxTaskPriorityGet(taskHandle)); }
	#endif

	#if INCLUDE_vTaskPrioritySet
	  /**
	   * @brief Set Task priority
	   *
	   * Only available if INCLUDE_vTaskPrioritySet == 1
	   * @param priority_ The TaskPriority to give the Task.
	   */
	  void priority(TaskPriority priority_) { vTaskPrioritySet(taskHandle, priority_); }
	#endif

	#if INCLUDE_vTaskSuspend
	  /**
	   * @brief Suspend the Task.
	   *
	   * Only available if INCLUDE_vTaskSuspend == 1
	   */
	  void suspend() { vTaskSuspend(taskHandle); }

	  /**
	   * @brief Resume the Task.
	   *
	   * Only available if INCLUDE_vTaskSuspend == 1
	   */
	  void resume() { vTaskResume(taskHandle); }
	# endif

	# if INCLUDE_xTaskResumeFromISR
	  /**
	   * @brief Resume task from ISR.
	   *
	   * Note: Only functions with _ISR should be used inside Interupt service routines.
	   *
	   * Only available if INCLUDE_vTaskSuspend == 1 and INCLUDE_vTaskResumeFromISR == 1
	   * @returns True if ISR should request a context switch.
	   */
	  bool resume_ISR() { return xTaskResumeFromISR(taskHandle); }
	#endif

	  /**
	   * @brief Notify a Task.
	   *
	   * Generic Task Notification operation
	   */
	  bool 			notify(uint32_t value, eNotifyAction act)
	  	  	  	  	  	  { return xTaskNotify(taskHandle, value, act); }
	  bool 			notify_ISR(uint32_t value, eNotifyAction act, portBASE_TYPE& waswoken)
	  	  	  	  	  	  { return xTaskNotifyFromISR(taskHandle, value, act, &waswoken);}
	  bool 			notify_query(uint32_t value, eNotifyAction act, uint32_t &old)
	  	  	  	  	  	  {return xTaskNotifyAndQuery(taskHandle, value, act, &old); }
	  bool 			notify_query_ISR(uint32_t value, eNotifyAction act, uint32_t &old, portBASE_TYPE& waswoken)
	  	  	  	  	  	  {return xTaskNotifyAndQueryFromISR(taskHandle, value, act, &old, &waswoken); }
#if FREERTOS_VERSION_ALL >= 10'004'000
	  bool 			notifyIndex(UBaseType_t idx, uint32_t value, eNotifyAction act)
	  	  	  	  	  	  { return xTaskNotifyIndexed(taskHandle, idx, value, act); }
	  bool 			notifyIndex_ISR(UBaseType_t idx, uint32_t value, eNotifyAction act, portBASE_TYPE& waswoken)
	  	  	  	  	  	  { return xTaskNotifyIndexedFromISR(taskHandle, idx, value, act, &waswoken);}
	  bool 			notifyIndex_query(UBaseType_t idx, uint32_t value, eNotifyAction act, uint32_t &old)
	  	  	  	  	  	  {return xTaskNotifyAndQueryIndexed(taskHandle, idx, value, act, &old); }
	  bool 			notifyIndex_query_ISR(UBaseType_t idx, uint32_t value, eNotifyAction act, uint32_t &old, portBASE_TYPE& waswoken)
	  	  	  	  	  	  {return xTaskNotifyAndQueryIndexedFromISR(taskHandle, idx, value, act, &old, &waswoken); }
#endif

#if FREERTOS_VERSION_ALL >= 10'003'000
	  bool			notifyStateClear() { return xTaskNotifyStateClear(taskHandle); }
	  uint32_t		notifyValueClear(uint32_t bits)	{ return ulTaskNotifyValueClear(taskHandle, bits); }
#if FREERTOS_VERSION_ALL >= 10'004'000
	  bool			notifyStateClearIndex(UBaseType_t idx) { return xTaskNotifyStateClearIndexed(taskHandle, idx); }
	  uint32_t		notifyValueClearIndex(UBaseType_t idx, uint32_t bits)	{ return ulTaskNotifyValueClearIndexed(taskHandle, idx, bits); }
#endif
#endif
	  /**
	   * @brief Notify a Task as a semaphore
	   *
	   * Sends a notification to a task using a semaphore based protocol. Generally the task should we using
	   * the take() function to receive the notification.
	   */
	  bool 			give() 		{ return xTaskNotifyGive(taskHandle); }
	  void 			give_ISR(portBASE_TYPE& waswoken)
	  	  	  	  	  	  { vTaskNotifyGiveFromISR(taskHandle, &waswoken); }

#if FREERTOS_VERSION_ALL >= 10'004'000
	  bool 			giveIndex(UBaseType_t idx) 	{ return xTaskNotifyGiveIndexed(taskHandle, idx); }
	  void 			giveIndex_ISR(UBaseType_t idx, portBASE_TYPE& waswoken)
	  	  	  	  	  	  { vTaskNotifyGiveIndexedFromISR(taskHandle, idx, &waswoken); }
#endif
	  // Static as always affect the current (calling) task
	  /**
	   * @brief Wait for task notification.
	   */
	  static    uint32_t    wait(uint32_t clearEnter, uint32_t clearExit = 0xFFFFFFFF, uint32_t* value = nullptr, TickType_t ticks = portMAX_DELAY)
	  	  	  	  	  	 { return xTaskNotifyWait(clearEnter, clearExit, value, ticks); }
#if FREERTOSCPP_USE_CHRONO
      static    uint32_t    wait(uint32_t clearEnter, uint32_t clearExit, uint32_t* value, Time_ms ms)
                         { return xTaskNotifyWait(clearEnter, clearExit, value, ms2ticks(ms)); }
#endif
#if FREERTOS_VERSION_ALL >= 10'004'000
	  static    uint32_t	waitIndex(UBaseType_t idx, uint32_t clearEnter, uint32_t clearExit = 0xFFFFFFFF, uint32_t* value = nullptr, TickType_t ticks = portMAX_DELAY)
	  	  	  	  	  	 { return xTaskNotifyWaitIndexed(idx, clearEnter, clearExit, value, ticks); }
#if FREERTOSCPP_USE_CHRONO
 	  static    uint32_t	waitIndex(UBaseType_t idx, uint32_t clearEnter, uint32_t clearExit, uint32_t* value, Time_ms ms)
	  	  	  	  	  	 { return xTaskNotifyWaitIndexed(idx, clearEnter, clearExit, value, ms2ticks(ms)); }
#endif // FREERTOSCPP_USE_CHRONO
#endif // FREERTOS_VERSION_ALL >= 10'004'000

     /**
	   * @brief Wait for a task Give notification
	   *
	   * Specialized wait() designed to work with the give()/give_ISR() notifications.
	   *
	   * @param clear   Flag to indicate if the action on succesful take is to clear (True) or decrement (False) the notification value.
	   * Effectively decides between a binary (True) or counting (False) semaphore behavior.
	   *
	   * @param ticks   The time to wait for the semaphore.
	   *
	   * @returns   Returns the notification word (prior to being adjusted for the take() ), Will be zero if
	   * the take() timed out.
	   */
      static uint32_t   take(bool clear = true, TickType_t ticks = portMAX_DELAY)
                          { return ulTaskNotifyTake(clear, ticks); }
#if FREERTOSCPP_USE_CHRONO
      /**
       * @brief Wait for a task Give notification
       *
       * Specialized wait() designed to work with the give()/give_ISR() notifications.
       *
       * @param clear   Flag to indicate if the action on successful take is to clear (True) or decrement (False) the notification value.
       * Effectively decides between a binary (True) or counting (False) semaphore behavior.
       *
       * @param ticks   The time to wait for the semaphore.
       *
       * @returns   Returns the notification word (prior to being adjusted for the take() ), Will be zero if
       * the take() timed out.
       */
      static uint32_t   take(bool clear, Time_ms ticks)
                          { return ulTaskNotifyTake(clear, ms2ticks(ticks)); }
#endif
    protected:
	  TaskHandle_t taskHandle;  ///< Handle for the task we are managing.

	private:
#if __cplusplus < 201101L
	    TaskBase(TaskBase const&);      ///< We are not copyable.
	    void operator =(TaskBase const&);  ///< We are not assignable.
#else
	    TaskBase(TaskBase const&) = delete;      ///< We are not copyable.
	    void operator =(TaskBase const&) = delete;  ///< We are not assignable.
#endif // __cplusplus

};

/**
 * @brief Statically Created Task Wrapper.
 * Create the specified task with a provided task function.
 *
 * @tparam stackDepth Size of the stack to give to the task
 *
 * If the Task object is destroyed, the class will be deleted (if deletion has been enabled)
 * @ingroup FreeRTOSCpp
 */

template<uint32_t stackDepth
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
    =0
#endif
    > class TaskS : public TaskBase {
public:
	/**
	 * @brief Constructor.
	 *
	 * @param name The name of the task.
	 * @param taskfun The function implementing the task, should have type void (*taskfun)(void *)
	 * @param priority_ The priority of the task. Use the TaskPriority enum values or a related value converted to a TaskPriority
	 * @param myParm the parameter passed to taskFun. Defaults to NULL.
	 *
	 * Upon construction the task will be created.
	 *
	 */
    TaskS(char const*name, void (*taskfun)(void *), TaskPriority priority_, void * myParm = 0) : 
    	TaskBase() {

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
	    	taskHandle = xTaskCreateStatic(taskfun, name, stackDepth, myParm, priority_, stack, &tcb);
#else
	    	xTaskCreate(taskfun, name, stackSize, myParm, priority_, &taskHandle); 
#endif
    }

private:
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
  StaticTask_t tcb;
  StackType_t stack[stackDepth];
#endif
};


/**
 * @brief Dynamically Created Task Wrapper
 */
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

template<> class TaskS<0> : public TaskBase {
public:
	/**
	 * @brief Constructor.
	 *
	 * @param name The name of the task.
	 * @param taskfun The function implementing the task, should have type void (*taskfun)(void *)
	 * @param priority_ The priority of the task. Use the TaskPriority enum values or a related value converted to a TaskPriority
	 * @param stackSize Size of the stack to give to the task
	 * @param myParm the parameter passed to taskFun. Defaults to NULL.
	 *
	 * Upon construction the task will be created.
	 *
	 */
  TaskS(char const*name, void (*taskfun)(void *), TaskPriority priority_,
       unsigned portSHORT stackSize, void * myParm = nullptr) :
	   TaskBase() {
	    xTaskCreate(taskfun, name, stackSize, myParm, priority_, &taskHandle);
  }
};

typedef TaskS<0> Task;
#endif

/**
 * @brief Base Class for all Class based tasks. 
*/
class TaskClassBase {
public:
	TaskClassBase() {} 
    ~TaskClassBase() {}
  /**
   * @brief task function.
   * The member function task needs to
   */
  virtual void task() = 0;

			bool	wait_at_start = false;
};

/**
 * @brief Make a class based task.
 * Derive from TaskClass and the 'task()' member function will get called as the task based on the class.
 *
 * @tparam stackDepth Size of the stack to give to the task
 *
 * If task() returns the task will be deleted if deletion has been enabled.
 * @ingroup FreeRTOSCpp
 */

template<uint32_t stackDepth> class TaskClassS : public TaskClassBase, public TaskS<stackDepth> {
public:
	/**
	 * @brief Constructor
	 *
	 * @param name The name of the task.
	 * @param priority_ The priority of the task. Use the TaskPriority enum values or a related value converted to a TaskPriority
	 * @param stackDepth_ How many words of stack to allocate to the task. Only used if the template parameter stackDepth is 0
	 *
	 * API CHANGE!
	 * If at constrtuction time, the scheduler is not running, we can release the task. If the scheduler is running, because
	 * the task may startup before we finish constructing the task, the most derived constructor will need to give the task.
	 * 
	 * Change from previous API to support SMP mode where the previous trick won't work anymore.
	 */
  TaskClassS(char const*name, TaskPriority priority_, unsigned portSHORT stackDepth_=0) :
    TaskS<stackDepth>(name, &taskcpp_task_thunk, priority_, static_cast<TaskClassBase*>(this))
  {
	(void) stackDepth_;
	// API CHANGE: We give if Scheduer not running, otherwise final constructor needs to give.
	if(xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
		this->TaskBase::give();
	}
  }

  virtual ~TaskClassS() {}

  /**
   * @brief task function.
   * The member function task needs to
   */
  void task() override = 0;

};


#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
template<> class TaskClassS<0> : public TaskClassBase, public TaskS<0> {
public:
	/**
	 * @brief Constructor
	 *
	 * @param name The name of the task.
	 * @param priority_ The priority of the task. Use the TaskPriority enum values or a related value converted to a TaskPriority
	 * @param stackDepth_ How many words of stack to allocate to the task. Only used if the template parameter stackDepth is 0
	 *
	 * API CHANGE!
	 * If at constrtuction time, the scheduler is not running, we can release the task. If the scheduler is running, because
	 * the task may startup before we finish constructing the task, the most derived constructor will need to give the task.
	 * 
	 * Change from previous API to support SMP mode where the previous trick won't work anymore.
	 */
  TaskClassS(char const*name, TaskPriority priority_, unsigned portSHORT stackDepth_) :
    TaskS<0>(name, &taskcpp_task_thunk, priority_, stackDepth_, static_cast<TaskClassBase*>(this))
  {
	// API CHANGE: We give if Scheduer not running, otherwise final constructor needs to give.
	if(xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
		this->TaskBase::give();
	}
  }

  virtual ~TaskClassS() {}

  /**
   * @brief task function.
   * The member function task needs to
   */
  void task() override = 0;

};

typedef TaskClassS<0> TaskClass;
#endif

#if FREERTOSCPP_USE_NAMESPACE
} // namespace FreeRTOScpp
#endif

#endif
