/**
 * @file ReadWrite.h
 * @brief FreeRTOS Read/Write Lock Wrapper
 *
 * This file contains a set of lightweight wrappers for mutexes using FreeRTOS
 *
 * @warning This is a fairly new module, and may not be fully tested
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

#ifndef READWRITE_H
#define READWRITE_H

#include "EventCpp.h"
#include "TaskCPP.h"
#include "Lock.h"

#if FREERTOSCPP_USE_NAMESPACE
namespace FreeRTOScpp {
#endif

class ReadWriteLock;

/**
 * Read-Write Lock Read Side Lockability Base
 *
 * Base class to provide Read side Lockability
*/
class Reader : public Lockable {
    friend class ReadWriteLock;
private:
    Reader() {}
public:
    bool take(TickType_t wait) override;
    bool give() override;
};

/**
 * Read-Write Write Side Lockability Base
 */
class Writer : public Lockable {
    friend class ReadWriteLock;
private:
    Writer() {}
public:
    bool take(TickType_t wait) override;
    bool give() override;

};


/**
 * Read/Write Lock control
 *
 * States:
 * + Free: readCount == 0
 * + Read: readCount > 0, reserved == 0
 * + Reserved: readCount > 0, reserved != 0
 * + Write: readCount == -1, reserved == 0
 * + Upgraded: readCount == -1, reserved != 0
 * 
 * @dot
 digraph states {
    compound=true;
    rankdir="LR";
    subgraph cluster_0 {
        Free;
    }

    subgraph cluster_1 {
        Read;
        Reserved;
        label="Read";

    }

    subgraph cluster_2 {
        Write;
        Upgraded;
        label="Write";
    }

    Free:w -> Read [color=green, label="readLock"];
    Read -> Free:sw [color=red, label="readUnlock"];
    Free:ne -> Reserved:se [color=green, label="reserveLock"];
    Read:e -> Reserved:w [color=green, label="reserveRequest"];
    Reserved:nw -> Read:ne [color=red, label="reserveRelease"];
    Reserved:sw -> Free:nw [color=red, label="readUnlock"];
    Free:e -> Write [color=green, label="writeLock"];
    Reserved:e -> Upgraded:w [color=green, label="writeLock"];
    Write -> Free:se [color=red, label="writeUnlock"];
    Upgraded:nw -> Reserved:ne [color=red, label="writeUnlock"];
 }
 * @enddot
 * @warning This is a fairly new module, and may not be fully tested
 */
class ReadWriteLock : public Reader, public Writer {
public:
    ReadWriteLock();
    ~ReadWriteLock();

    /**
     * Get Read Lockable
     * 
     * @returns a lockable object for using with Read Locks.
     */
    Reader& rlock() { return *this; }
    /**
     * Get Write Lockable
     * 
     * @returns a lockable object for using with Write Locks
     */
    Writer& wlock() { return *this; }

    /**
     * ReadLock
     * 
     * Get a read lock.
     * @param wait The maximum number of ticks to wait to get the lock
     * @returns true if the read lock has been granted
     * 
     * Algorithm:
     * 
     * + Loop
     *   + If readCount >=0 and our priority > writeReq
     *     + Increment readCount
     *     + return true
     *   + if Time has expired: 
     *     + return false
     *   + else wait a tick for read_bit event. 
     */
    bool readLock(TickType_t wait = portMAX_DELAY);
    /**
     * Get an upgradable Read Lock
     * 
     * like readLock, but add reserved == nullptr to the conditions.
     * On success will set reserved to our task handle.
     * 
     * Only one task can reserve this, as if two are in this state they will deadlock when 
     * they both try to upgrade
     */
    bool reservedLock(TickType_t wait = portMAX_DELAY);
    /**
     * Task with a Read Lock request upgrade to a reserved lock.
     * If no reservation current, will be granted, if reservation present, will be rejected.
     * 
     * Does not check if this task has a read lock, but that is assumed.
     */
    bool requestReserved();
    /**
     * If we have a reserved lock, down grade to just a read lock
     */
    bool releaseReserved();
    /**
     * Remove our lock. If we reserved the upgrade, release that.
     */
    bool readUnlock();

    /**
     * Take the write lock, requires readCount to be 0, or 1 if we reserved the upgrade.
     */
    bool writeLock(TickType_t wait = portMAX_DELAY);
    /**
     * Release the write lock. If we upgraded go back to a reserved lock which will need to be unlocked.
     */
    bool writeUnlock();

#if FREERTOSCPP_USE_CHRONO
    bool readLock(Time_ms delay_ms)     { return readLock(ms2ticks(delay_ms)); }
    bool reservedLock(Time_ms delay_ms)  { return reservedLock(ms2ticks(delay_ms)); }
    bool writeLock(Time_ms delay_ms)    { return writeLock(ms2ticks(delay_ms)); }
#endif
protected:
    EventGroup      event;
    /**
     * Count of Read Locks
     * 
     * If 0, then lock is free
     * if >0, lock is in read mode and is the count of the number of read locks granted
     * if <0, loci is in write mode.
     */
    int             readCount = 0;
    /**
     * Reserved Lock indicator
     * 
     * If nullptr, then no reservedLock are currently in existence, so a reservedLock can be granted
     * 
     * Else, TaskHandle of the task that has reserved the right to upgrade to a write lock.
     */
    TaskHandle_t    reserved = nullptr;
    /**
     * Write Request Pending Priority.
     * 
     * If in Read mode, and a task with priority below writeReq 
     */
    int    writeReq = -1;
};

#if FREERTOSCPP_USE_NAMESPACE
}
#endif

#endif
