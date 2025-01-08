/**
 * @file ReadWrite.cpp
 * @brief FreeRTOS Read/Write Lock Wrapper
 *
 * This file contains a set of lightweight wrappers for mutexes using FreeRTOS
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

#include <ReadWrite.h>

#if FREERTOSCPP_USE_NAMESPACE
namespace FreeRTOScpp {
#endif

/* 
Different events we can be waiting for.
Right now minimal, might be able to expand bits so less tasks get woken prematurely
*/
constexpr unsigned read_bit  = 1 << 0;
constexpr unsigned write_bit = 1 << 1;

bool Reader::take(TickType_t wait) {
    return static_cast<ReadWriteLock*>(this)->readLock(wait);
}

bool Reader::give() {
    return static_cast<ReadWriteLock*>(this)->readUnlock();
}

bool Writer::take(TickType_t wait) {
    return static_cast<ReadWriteLock*>(this)->writeLock(wait);
}

bool Writer::give() {
    return static_cast<ReadWriteLock*>(this)->writeUnlock();
}

ReadWriteLock::ReadWriteLock() {
}

ReadWriteLock::~ReadWriteLock() {
}


bool ReadWriteLock::readLock(TickType_t wait) {
    TickType_t start = xTaskGetTickCount();
    while(1) {
        taskENTER_CRITICAL();
        if(readCount >= 0 && (int)uxTaskPriorityGet(nullptr) > writeReq) {
            // Lock is granted, record that.
            readCount++;
            taskEXIT_CRITICAL();
            return true;
        }
        taskEXIT_CRITICAL();
        TickType_t now = xTaskGetTickCount();
        if(now-start >= wait) {
            // Timed out. 
            return false;
        }
        event.wait(read_bit, true, true, 1);
    }
}


bool ReadWriteLock::reservedLock(TickType_t wait) {
    TickType_t start = xTaskGetTickCount();
    TaskHandle_t task = xTaskGetCurrentTaskHandle();
    while(1) {
        taskENTER_CRITICAL();
        if(readCount >= 0 && reserved == nullptr && (int)uxTaskPriorityGet(nullptr) > writeReq) {
            // Lock is granted, record that, reserve our ability to promote
            readCount++;
            reserved = task;
            taskEXIT_CRITICAL();
            return true;
        }
        taskEXIT_CRITICAL();
        TickType_t now = xTaskGetTickCount();
        if(now-start >= wait) {
            // Timed out. 
            return false;
        }
        event.wait(read_bit, true, true, 1);
    }
}

bool ReadWriteLock::requestReserved() {
    TaskHandle_t task = xTaskGetCurrentTaskHandle();
    bool flag = false;
    taskENTER_CRITICAL();
    if( readCount > 0 && reserved == nullptr) {
        reserved = task;
        flag = true;
    }
    taskEXIT_CRITICAL();
    return flag;
}

bool ReadWriteLock::releaseReserved() {
    // Don't need a critical, as no one with change it to us
    if(xTaskGetCurrentTaskHandle() == reserved) {
        reserved = nullptr;
        // Signal read event as another task may be waiting on it. This might be able to be a different bit.
        event.set(read_bit);
        return true;
    }
    return false;
}

bool ReadWriteLock::readUnlock() {
    bool ret = true;
    // IF we had the reservation, clear it.
    if(xTaskGetCurrentTaskHandle() == reserved) {
        reserved = nullptr;
        // Signal read event as another task may be waiting on it. This might be able to be a different bit.
        event.set(read_bit);
    }
    taskENTER_CRITICAL();
    if (readCount > 0) {
        readCount--;
    } else {
        // something is wrong with the unlock, as we aren't locked.
        ret = false;   
    }
    if (readCount == 0) {
        reserved = nullptr;     // just for safety.
    }
    taskEXIT_CRITICAL();
    // Can be outside critical as false positive doesn't really hurt anything here.
    if (readCount == 0) {
        event.set(write_bit);
    }
    return ret;
}

bool ReadWriteLock::writeLock(TickType_t wait) {
    TaskHandle_t task = xTaskGetCurrentTaskHandle();
    TickType_t start = xTaskGetTickCount();
    int priority = uxTaskPriorityGet(nullptr);
    while(1) {
        taskENTER_CRITICAL();
        if (0 <= readCount && readCount <= (reserved == task)) {
            readCount = -1;
            writeReq = -1;
            taskEXIT_CRITICAL();
            return true;
        }
        taskEXIT_CRITICAL();

        TickType_t now = xTaskGetTickCount();
        if(now-start >= wait) {
            // clear writeReq if it might have been us.

            taskENTER_CRITICAL();
            if (writeReq == priority) {
                writeReq = -1;
                taskEXIT_CRITICAL();
                // We may have been blocking a reader, or removed another writers request.
                event.set(read_bit|write_bit);
            } else {
                taskEXIT_CRITICAL();
            }
            return false;
        }

        taskENTER_CRITICAL();
        // Update request priority if we are higher
        if (writeReq < priority) {
            writeReq = priority;
        }
        taskEXIT_CRITICAL();

        event.wait(write_bit, true, true, 1);
    }
}

bool ReadWriteLock::writeUnlock() {
    // Assume we had the write lock until proven otherwise
    // no one else touches things if readCount is -1
    if (readCount >= 0) return false;   // bad call
    if (reserved == xTaskGetCurrentTaskHandle()) {
        // We were reserved, convert the write lock to our previous reserved lock
        readCount = 1;
    } else {
        readCount = 0;
    }
    // Let other readers in, and notify writers so they can add their requests
    event.set(read_bit|write_bit);
    return true;
}

#if FREERTOSCPP_USE_NAMESPACE
}
#endif
