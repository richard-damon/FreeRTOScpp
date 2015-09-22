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
 */
#ifndef TaskCPP_H
#define TaskCPP_H

#include "FreeRTOS.h"
#include "task.h"

/**
 * Names for Base set of Priorities.
 *
 * Needs configMAX_PRIORITIES at least 3 for value to make sense, preferably at least 6
 */
enum TaskPriority {
	TaskPrio_Idle = 0,                        ///< Non-Real Time operatons
	TaskPrio_Low = 1,                         ///< Non-Critical operations
	TaskPrio_HMI = 2,						  ///< Normal User Interface Level
	TaskPrio_Mid = (configMAX_PRIORITIES-1)/2,///< Semi-Critical, have deadlines, not a lot of processing
	TaskPrio_High = configMAX_PRIORITIES-2,   ///< Urgent tasks, short deadlines, not much processing
	TaskPRio_Highest = configMAX_PRIORITIES-1 ///< Critical Tasks, Do NOW, must be quick (Used by FreeRTOS)
};

/**
 * @brief Lowest Level Wrapper.
 * Create the specified task with a provided task function.
 *
 * If the Task object is destroyed, the class will be deleted (if deletion has been enabled)
 */

class Task {
public:
	/**
	 * @brief Constructor
	 *
	 * @param name The name of the task.
	 * @param taskfun The function implementing the task, should have type void (*taskfun)(void *)
	 * @param priority The priority of the task. Use the TaskPriority enum values or a related value converted to a TaskPriority
	 * @param stackDepth Size of the stack to give to the task
	 * @param parm the parameter passed to taskFun. Defaults to NULL.
	 *
	 * Upon construction the task will be created.
	 */
  Task(char const*name, void (*taskfun)(void *), TaskPriority priority,
       unsigned portSHORT stackDepth, void * parm = 0) {
    xTaskCreate(taskfun, name, stackDepth, parm, priority, &handle);
  }
  virtual ~Task() {
#if INCLUDE_vTaskDelete
	if(handle){
	    vTaskDelete(handle);
	}
#endif
    return;
  }

  TaskHandle_t handle;

};

/**
 * @brief Make a class based task.
 * Derive from TaskClass and the 'task()' member function will get called as the task based on the class.
 *
 * If task() returns the task will be deleted if deletion has been enabled.
 */
class TaskClass : public Task {
public:
	/**
	 * @brief Constructor
	 *
	 * @param name The name of the task.
	 * @param taskfun The function implementing the task, should have type void (*taskfun)(void *)
	 * @param priority The priority of the task. Use the TaskPriority enum values or a related value converted to a TaskPriority
	 * @param stackDepth Size of the stack to give to the task
	 *
	 * Note: At construction time the task will be created, so if the the scheduler has been started, the created task needs to
	 * have a priority less than the creating task so it can't start until after the class deriving from TaskClass has finished
	 * it constructor (or other measures need to have been taken to make sure this happens, like stopping the scheduler).
	 */
  TaskClass(char const*name,TaskPriority priority,
           unsigned portSHORT stackDepth) :
    Task(name, &taskfun, priority, stackDepth, this)
  {
  }
  /**
   * @brief task function.
   * The member function task needs to
   */
  virtual void task() = 0;
  static void taskfun(void* parm) {
	static_cast<TaskClass *>(parm) -> task();
#if INCLUDE_vTaskDelete
	handle = 0;
    vTaskDelete(0); // Delete ourselves
#else
    while(1)
      vTaskDelay(portMAX_DELAY);
#endif
  }
};

#endif
