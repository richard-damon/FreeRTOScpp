/**
 * @file MutexCPP.h
 * @brief FreeRTOS Mutex Wrapper
 *
 * This file contains a set of lightweight wrappers for mutexes using FreeRTOS
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
 */

#ifndef MUTEXCPP_H
#define MUTEXCPP_H

#include "Lock.h"
#include "semphr.h"

/**
 * @brief Mutex Wrapper.
 *
 * A Mutex is a basic synchronization primitive allowing mutual exclusion to be
 * handled which also implements priority inheritance. Note, the basic mutex is 
 * NOT recursive, ie if a Task has taken the Mutex, it must not try to take it 
 * again before giving it. If you need this, use a RecurviseMutex. 
 *
 * The usage of a Mutex is similar to a semaphore, but the task that takes the
 * Mutex is also supposed to be the Task that eventually gives it back. It also
 * doesn't normally make sense for an ISR to use a Mutex, so no _ISR routines 
 * have been made available.
 *
 * Example Usage:
 * @code
 * RecursiveMutex mutex("MyMutex");
 *
 * // In some other task
 * mutex.take();
 * ... // some code that needs mutual exclusion
 * mutex.give();
 * @endcode
 *
 * @ingroup FreeRTOSCpp
 */

class Mutex : public Lockable {
public:
	/**
	 * @brief Constructor.
	 * @param name Name to give mutex, used for Debug Registry if setup
	 */
	Mutex(char const* name) {
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
		handle = xSemaphoreCreateMutexStatic(&mutexBuffer);
#else
		handle = xSemaphoreCreateMutex();
#endif
#if configQUEUE_REGISTRY_SIZE > 0
		vQueueAddToRegistry(handle, name);
#endif
	}
	/**
	 * @brief Destructor.
	 *
	 * Deletes the semaphore.
	 */
	~Mutex() {
		vSemaphoreDelete(handle);
	}

	bool take(TickType_t wait = portMAX_DELAY) {
		return xSemaphoreTake(handle, wait);
	}

	bool give() {
		return xSemaphoreGive(handle);
	}
private:
	SemaphoreHandle_t handle;
#if __cplusplus < 201101L
    Mutex(Mutex const&);      ///< We are not copyable.
    void operator =(Mutex const&);  ///< We are not assignable.
#else
    Mutex(Mutex const&) = delete;      ///< We are not copyable.
    void operator =(Mutex const&) = delete;  ///< We are not assignable.
#endif // __cplusplus
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    StaticSemaphore_t mutexBuffer;
#endif

};

#if configUSE_RECURSIVE_MUTEXES > 0
/**
 * @brief Recursive Mutex Wrapper.
 *
 * A RecursiveMutex adds the ability to nest takes, so that if you have taken the 
 * RecursiveMutex and take it again, this works and requires you to give the 
 * RecursiveMutex back as many times as it was taken before it is released.
 *
 * One very common application for this is for messages to the user on a console.
 * Generally, you don't want pieces of a message interrupted by pieces of other 
 * messages, so message output routines use a mutex on the console port.
 * These routines often use lower level routines that also want to make sure their
 * output isn't interspersed, so each level takes the RecursiveMutex at their start
 * and releases it at the end. Being a RecursiveMutex this works.
 *
 * Example Usage:
 * @code
 * RecursiveMutex mutex("MyMutex");
 *
 * // In some other task
 * mutex.take();
 * ...
 *  // possible in a recursive call or call to lower level routine.
 *  mutex.take();
 *  ...
 *  mutex.give();
 * ... 
 * mutex.give();
 *
 * @endcode
 * @ingroup FreeRTOSCpp
 */

class RecursiveMutex : public Lockable{
public:
	RecursiveMutex(char const* name) {
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
		handle = xSemaphoreCreateRecursiveMutexStatic(&mutexBuffer);
#else
		handle = xSemaphoreCreateRecursiveMutex();
#endif
#if configQUEUE_REGISTRY_SIZE > 0
		vQueueAddToRegistry(handle, name);
#endif
	}
	~RecursiveMutex() {
		vSemaphoreDelete(handle);
	}

	bool take(TickType_t wait = portMAX_DELAY) {
		return xSemaphoreTakeRecursive(handle, wait);

	}

	bool give() {
		return xSemaphoreGiveRecursive(handle);
	}

private:
	SemaphoreHandle_t handle;
#if __cplusplus < 201101L
    RecursiveMutex(RecursiveMutex const&);      ///< We are not copyable.
    void operator =(RecursiveMutex const&);  ///< We are not assignable.
#else
    RecursiveMutex(RecursiveMutex const&) = delete;      ///< We are not copyable.
    void operator =(RecursiveMutex const&) = delete;  ///< We are not assignable.
#endif // __cplusplus
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    StaticSemaphore_t mutexBuffer;
#endif
};
#endif // configUSE_RECURSIVE_MUTEXES

#endif // MUTEXCPP_H
