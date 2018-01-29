/**
 * @file TaskCPP.h
 * @brief FreeRTOS Task Wrapper
 *
 * This file contains a set of lightweight wrappers for tasks using FreeRTOS
 * 
 * @copyright (c) 2007-2015 Richard Damon
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
 * @defgroup FreeRTOSCpp Free RTOS C++ Wrapper 
 */
#ifndef TaskCPP_H
#define TaskCPP_H

#include "FreeRTOS.h"
#include "task.h"

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
 * @brief Lowest Level Wrapper.
 * Create the specified task with a provided task function.
 *
 * If the TaskBase object is destroyed, the FreeRTOS Task will be deleted (if deletion has been enabled)
 * @ingroup FreeRTOSCpp
 * @todo Fully implemwent task manipulation functions

 *
 */

class TaskBase {
protected:
	/**
	 * @brief Constructor
	 *
	 * TaskBase is effectively Abstract, so a sub class is needed to create to Task.
	 */
	TaskBase() : handle(0) {

	}

public:
	  /**
	   * @brief Destructor.
	   *
	   * If deletion is enabled, delete the task.
	   */
	  virtual ~TaskBase() {
#if INCLUDE_vTaskDelete
		if(handle){
		    vTaskDelete(handle);
		}
#endif
	    return;
	  }
	  /**
	   * @brief Get Task Handle.
	   * @return the task handle.
	   */
	  TaskHandle_t getHandle() const { return handle; }

	#if INCLUDE_uxTaskPriorityGet
	  /**
	   * @brief Get Task priority
	   *
	   * Only available if INCLUDE_vTaskPriorityGet == 1
	   * @return The priority of the Task.
	   */
	  TaskPriority priority() const { return static_cast<TaskPriority>(uxTaskPriorityGet(handle)); }
	#endif

	#if INCLUDE_vTaskPrioritySet
	  /**
	   * @brief Set Task priority
	   *
	   * Only available if INCLUDE_vTaskPrioritySet == 1
	   * @param priority_ The TaskPriority to give the Task.
	   */
	  void priority(TaskPriority priority_) { vTaskPrioritySet(handle, priority_); }
	#endif

	#if INCLUDE_vTaskSuspend
	  /**
	   * @brief Suspend the Task.
	   *
	   * Only available if INCLUDE_vTaskSuspend == 1
	   */
	  void suspend() { vTaskSuspend(handle); }

	  /**
	   * @brief Resume the Task.
	   *
	   * Only available if INCLUDE_vTaskSuspend == 1
	   */
	  void resume() { vTaskResume(handle); }
	# endif

#if INCLUDE_xTaskAbortDelay
	  /**
	   * @brief Abort Delay
	   *
	   * Only available if INCLUDE_xTaskAbortDelay == 1
	   */
	  void abortDelay() { xTaskAbortDelay(handle); }

#endif
	# if INCLUDE_xTaskResumeFromISR
	  /**
	   * @brief Resume task from ISR.
	   *
	   * Note: Only functions with _ISR should be used inside Interupt service routines.
	   *
	   * Only available if INCLUDE_vTaskSuspend == 1 and INCLUDE_vTaskResumeFromISR == 1
	   * @returns True if ISR should request a context switch.
	   */
	  bool resume_ISR() { return xTaskResumeFromISR(handle); }
	#endif

	  bool notify(uint32_t value, eNotifyAction act) { return xTaskNotify(handle, value, act); }
	  bool notify_ISR(uint32_t value, eNotifyAction act, portBASE_TYPE& waswoken)
	  	  { return xTaskNotifyFromISR(handle, value, act, &waswoken);}
	  bool notify_query(uint32_t value, eNotifyAction act, uint32_t &old)
	  	  {return xTaskNotifyAndQuery(handle, value, act, &old); }
	  bool notify_query_ISR(uint32_t value, eNotifyAction act, uint32_t &old, portBASE_TYPE& waswoken)
	  	  {return xTaskNotifyAndQueryFromISR(handle, value, act, &old, &waswoken); }
	  bool give() { return xTaskNotifyGive(handle); }
	  void give_ISR(portBASE_TYPE& waswoken) { vTaskNotifyGiveFromISR(handle, &waswoken); }
	protected:
	  TaskHandle_t handle;  ///< Handle for the task we are managing.
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
  TaskS(char const*name, void (*taskfun)(void *), TaskPriority priority_, void * myParm = 0) {
	    handle = xTaskCreateStatic(taskfun, name, stackDepth, myParm, priority_, stack, &tcb);
  }

protected:
  // used by TaskClassS to avoid needing too much complications
  TaskS(char const*name, void (*taskfun)(void *), TaskPriority priority_, unsigned portSHORT stackSize_,
		void * myParm) {
	  (void) stackSize_;
	  handle = xTaskCreateStatic(taskfun, name, stackDepth, myParm, priority_, stack, &tcb);
  }

private:
  StaticTask_t tcb;
  StackType_t stack[stackDepth];
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
       unsigned portSHORT stackSize, void * myParm = 0) {
	    xTaskCreate(taskfun, name, stackSize, myParm, priority_, &handle);
  }
};

typedef TaskS<0> Task;
#endif

/**
 * @brief Make a class based task.
 * Derive from TaskClass and the 'task()' member function will get called as the task based on the class.
 *
 * @tparam stackDepth Size of the stack to give to the task
 *
 * If task() returns the task will be deleted if deletion has been enabled.
 * @ingroup FreeRTOSCpp
 */
template<uint32_t stackDepth> class TaskClassS : public TaskS<stackDepth> {
public:
	/**
	 * @brief Constructor
	 *
	 * @param name The name of the task.
	 * @param priority_ The priority of the task. Use the TaskPriority enum values or a related value converted to a TaskPriority
	 * @param stackDepth_ How many words of stack to allocate to the task. Only used if the template parameter stackDepth is 0
	 *
	 * Note: At construction time the task will be created, so if the the scheduler has been started, the created task needs to
	 * have a priority less than the creating task so it can't start until after the class deriving from TaskClass has finished
	 * its constructor. If this is not true, the task will be created with a lower priority, and when it does get to run,
	 * it will correct its priority. If the task creating the new task is of IDLE priority, and time-slicing is enabled
	 * then there is a small chance that if a timer tick happens in the middle of creating the object, the system
	 * will switch to the incomplete task, so this is not advised.
	 * If INCLUDE_VTaskPrioritySet is not true, then this can't be done so the caller is responsible to enforce.
	 *
	 * This code is sort of ugly due to the conditional compilation. In words:
	 * * If we don't have vTaskPrioritySet, just create the task as specified, and it is the users job to make it work.
	 * * If we have vTaskPrioritySet but not uxTaskPriorityGet, tasks created when running start out at TaskPrio_Idle.
	 * * If we have both vTaskPrioritySet and uxTaskPriorityGet, then if the task is to be created with a priority greater
	 * than or equal to the creating task priority, then it will be created with a priority 1 less than the creating task
	 * (but not less than TaskPrio_Idle)
	 */
  TaskClassS(char const*name, TaskPriority priority_, unsigned portSHORT stackDepth_=0) :
    TaskS<stackDepth>(name, &taskfun,
#if INCLUDE_vTaskPrioritySet
    		((xTaskGetSchedulerState() == taskSCHEDULER_RUNNING)
#if INCLUDE_uxTaskPriorityGet
			&& (uxTaskPriorityGet(0) <= priority_)
#endif
			) ? (
#if INCLUDE_uxTaskPriorityGet
			(uxTaskPriorityGet(0) > TaskPrio_Idle ) ?	static_cast<TaskPriority>(uxTaskPriorityGet(0)-1) :
#endif
    		TaskPrio_Idle) :
#endif
			priority_,
			stackDepth_, this)
#if INCLUDE_vTaskPrioritySet
	, myPriority(priority_)
#endif
  {
  }

  virtual ~TaskClassS() {}

  /**
   * @brief task function.
   * The member function task needs to
   */
  virtual void task() = 0;

private:
  /**
   * Trampoline for task.
   *
   * @todo Note, is a static function so normally compatible by calling convention
   * with an ordinary C function, like FreeRTOS expects. For maximum portablity
   * we could change this to a free function declared as extern "C", but might need so
   * tricks to excape out of template space.
   */
  static void taskfun(void* myParm) {
	TaskClassS *myTask = static_cast<TaskClassS *>(myParm);
#if INCLUDE_vTaskPrioritySet
	myTask->priority(myTask->myPriority);
#endif
	myTask->task();
	// If we get here, task has returned, delete ourselves or block indefinitely.
#if INCLUDE_vTaskDelete
	myTask->handle = 0;
    vTaskDelete(0); // Delete ourselves
#else
    while(1)
      vTaskDelay(portMAX_DELAY);
#endif
  }
#if INCLUDE_vTaskPrioritySet
  TaskPriority myPriority;
#endif
};


#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
typedef TaskClassS<0> TaskClass;
#endif
#endif

