/**
 * @file TaskCpp.cpp
 * @brief FreeRTOS Task Wrapper
 *
 * @copyright (c) 2018-2024 Richard Damon
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


#include "TaskCpp.h"

#if FREERTOSCPP_USE_NAMESPACE
using namespace FreeRTOScpp;
#endif

extern "C" {

/**
 * Thunk for FreeRTOS to C++ Task Wrapper
*/
void taskcpp_task_thunk(void* parm) {
    TaskClassBase *myClass = static_cast<TaskClassBase*>(parm);
    if (myClass->wait_at_start) TaskBase::take();
    myClass->task();
#if INCLUDE_vTaskDelete
    vTaskDelete(nullptr);
#else
    while(1) {
        vTaskDelay(portMAX_DELAY);
    }
#endif
}    
} // extern "C"
