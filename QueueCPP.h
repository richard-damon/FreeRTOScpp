/**
 * @file QueueCPP.h
 * @brief FreeRTOS Queue Wrapper
 *
 * This file contains a set of lightweight wrappers for queues using FreeRTOS
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
#ifndef QUEUECPP_H
#define QUEUECPP_H

#include "FreeRTOS.h"
#include "queue.h"

//#include <type_traits>

template<class T> class Queue {
//	static_assert(::std::is_pod<T>, "Queues only work with PODs"); ///@todo need to get later version of C++ compile working
public:
  xQueueHandle handle;

  Queue(unsigned portBASE_TYPE length, char const* name){
    handle = xQueueCreate(length, sizeof(T));
#if configQUEUE_REGISTRY_SIZE > 0
    vQueueAddToRegistry(handle, name);
#endif

  };

  ~Queue() {
    vQueueDelete(handle);
  }

  unsigned portBASE_TYPE waiting() const {
    return uxQueueMessagesWaiting(handle);
  }

  bool push(T const& item, TickType_t time = portMAX_DELAY){
    return xQueueSendToFront(handle, &item, time);
  }

  bool add(T const& item, TickType_t time = portMAX_DELAY){
    return xQueueSendToBack(handle, &item, time);
  }

  bool pop(T& var, TickType_t time = portMAX_DELAY) {
    return xQueueReceive(handle, &var, time);
  }

  bool peek(T& var, TickType_t time = portMAX_DELAY) {
    return xQueuePeek(handle, &var, time);
  }

  bool full() {
    return 0 < uxQueueSpacesAvailable(handle);
  }

  bool empty() {
    return uxQueueMessagesWaiting(handle) == 0;
  }

  bool push_ISR(T const& item){
    return xQueueSendToFrontFromISR(handle, &item, waswoken);
  }

  bool add_ISR(T const& item){
    return xQueueSendToBackFromISR(handle, &item, waswoken);
  }

  bool pop_ISR(T& var) {
    return xQueueReceiveFromISR(handle, &var, waswoken);
  }

  bool peek_ISR(T& var) {
    return xQueuePeekFromISR(handle, &var, waswoken);
  }

  bool full_ISR() {
    return xQueueIsQueueFullFromISR(handle);
  }

  bool empty_ISR() {
    return xQueueIsQueueEmptyFromISR(handle);
  }

  unsigned waiting_ISR() {
    return uxQueueMessagesWaitingFromISR(handle);
  }

  void start_ISR(portBASE_TYPE& flag) { waswoken = &flag; }

private:
  portBASE_TYPE* waswoken;
};

#endif
