/**
 * @file Lock.cpp
 * @brief FreeRTOS Lock wrapper
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

#include <Lock.h>

using namespace FreeRTOScpp;

/**
 * Constructor
 *
 * This is the most generic constructor, and the one that will be used if no parameters beyond the
 * lockable object are given. The alternate constructor only take the time to wait, and assumes the
 * second parameter is true
 *
 * If wait is specified (or some code might abort the wait) then the code should check with
 * a call to locked() to confirm the lock has been taken.
 *
 * @param mylockable The Lockable object we will be using
 * @param mylocked Should we start by taking the lock
 * @param wait How long to wait to take the lock
 */
Lock::Lock(Lockable& mylockable, bool mylocked, TickType_t wait) :
lockable(mylockable),
lockCnt(0)
{
	if(mylocked) lock(wait);
}

#if FREERTOSCPP_USE_CHRONO
/**
 * Constructor
 *
 * Version to specifiy the time to wait to get the lock as a Chrono in milliseconds
 *
 * Code will need to check that the lock was gotten with a call to locked()
 *
 * @param mylockable The Lockable object we will be using
 * @param wait_ms How long to wait to take the lock in millisecons
 */
Lock::Lock(Lockable& myLockable, Time_ms wait_ms) :
lockable(myLockable),
lockCnt(0)
{
    lock(wait_ms);
}
#endif

/**
 * Destructor
 */
Lock::~Lock() {
	// on destruct, remove all locks.
	if(lockCnt > 0) {
		lockable.give();
	}
	lockCnt = 0;
}

/**
 * Try to take to lock
 *
 * Note, lock allows for nested calls, which need to be unlocked as many times
 * as taken, even if the lockable isn't recursive.
 *
 * @param wait How long to wait in Ticks
 * @return true if lock has been taken
 */
bool Lock::lock(TickType_t wait) {
	if(lockCnt > 0) {
		lockCnt++;
		return true;
	}
	if(lockable.take(wait)) {
		lockCnt++;
		return true;
	}
	return false;
}

/**
 * Release a lock
 */
void Lock::unlock() {
	if(lockCnt > 0) {	// ignore extra unlocks.
		lockCnt--;
		if(lockCnt == 0) {
			lockable.give();
		}
	}
}
