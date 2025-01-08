/**
 * @file FreeRTOScpp.h
 * @brief FreeRTOS Wrapper
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
 */

#ifndef FREERTOSPP_FREERTOSCPP_H_
#define FREERTOSPP_FREERTOSCPP_H_

#include "FreeRTOS.h"
#include "task.h"		// For Version Numbers

/**
 * @def FREERTOSCPP_USE_CHRONO
 * Enable the use of C++ chrono time values.
 * 
 * @ingroup FreeRTOSCpp 
 * 
 * @def FREERTOS_USE_NAMESPACE
 * If non-zero, put FreeRTOScpp library into namespace FreeRTOScpp.
 * If 2, adds a using namespace FreeRTOScpp, so code doesn't need to use that namespace unless conflicts arise.
 * @ingroup FreeRTOSCpp
 */

#if DOXYGEN
#define FREERTOSCPP_USE_CHRONO 1
#define FREERTOS_USE_NAMESPACE 2
#endif

#define FREERTOS_VERSION_ALL    (tskKERNEL_VERSION_MAJOR * 1'000'000 + tskKERNEL_VERSION_MINOR * 1000 + tskKERNEL_VERSION_BUILD)

#ifndef FREERTOSCPP_USE_CHRONO
#define FREERTOSCPP_USE_CHRONO 1        // Define to 1 to add C++ chrono time versions
#endif

#ifndef FREERTOSCPP_USE_NAMESPACE
#define FREERTOSCPP_USE_NAMESPACE 2		// 0 = No Namespace, 1 = In namespace FreeRTOScpp, 2 = In namespace FreeRTOScpp and then use the namespace
#endif

#if FREERTOSCPP_USE_CHRONO
#include <chrono>
#endif

namespace FreeRTOScpp {

#if FREERTOSCPP_USE_CHRONO
// Code to use std::chrono::durations optionally for times

typedef std::chrono::milliseconds Time_ms;

inline constexpr TickType_t ms2ticks(Time_ms ms) {
    return pdMS_TO_TICKS(ms.count());
}
#endif

}   // namespace FreeRTOScpp
#if FREERTOSCPP_USE_NAMESPACE == 2
using namespace FreeRTOScpp;
#endif

#endif /* FREERTOSPP_FREERTOSCPP_H_ */
