/**
 * @file SemaphoreCPP.h
 * @brief FreeRTOS Semaphore Wrapper
 *
 * This file contains a set of lightweight wrappers for semaphores using FreeRTOS
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
#ifndef SEMAPHORE_CPP_H
#define SEMAPHORE_CPP_H

#include "FreeRTOS.h"
#include "semphr.h"
class Semaphore {
public:
  Semaphore(char const* name) {
    vSemaphoreCreateBinary(sema);
#if configQUEUE_REGISTRY_SIZE > 0
    vQueueAddToRegistry(sema, name);
#endif
  }

  ~Semaphore() {
    vQueueDelete(sema);
  }

  signed portBASE_TYPE give() {
    return xSemaphoreGive(sema);
  }
  signed portBASE_TYPE  take(TickType_t delay = portMAX_DELAY){
    return xSemaphoreTake(sema, delay);
  }

  void start_ISR(portBASE_TYPE& flag) {
    waswoken = &flag;
  }
  signed portBASE_TYPE  give_ISR() {
    return xSemaphoreGiveFromISR(sema, waswoken);
  }
private:
  xSemaphoreHandle sema;
  portBASE_TYPE* waswoken;

};
#endif
