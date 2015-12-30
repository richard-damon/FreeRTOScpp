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

#include "FreeRTOS.h"
#include "semphr.h"

/**
 * @brief Mutex Wrapper.
 *
 * @ingroup FreeRTOSCpp
 */

class Mutex {
public:
	/**
	 * @brief Constructor.
	 * @param name Name to give mutex, used for Debug Registry if setup
	 */
	Mutex(char const* name) {
		handle = xSemaphoreCreateMutex();
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

};

#if configUSE_RECURSIVE_MUTEXES > 0
/**
 * @brief Recursive Mutex Wrapper.
 *
 * @ingroup FreeRTOSCpp
 */

class RecursiveMutex {
public:
	RecursiveMutex(char const* name) {
		handle = xSemaphoreCreateRecursiveMutex();
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
};
#endif // configUSE_RECURSIVE_MUTEXES

#endif // MUTEXCPP_H
