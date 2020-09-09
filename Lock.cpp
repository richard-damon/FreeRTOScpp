/** ********************************************************************
 * @file   Lock.cpp
 * @copyright (c) 2017-2018 Richard Damon
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
 * @date Jun 6, 2017 Created
 *
 *  Description:
 * @brief FreeRTOS Lock Wrapper
 *
 *  Wrapper classes for Basic Lock Functionality for FreeRTOS.
 *
 * @ingroup FreeRTOSCpp
 **********************************************************************/
#include <Lock.h>


Lock::Lock(Lockable& myLockable, bool mylocked, TickType_t wait) :
lockable(myLockable),
lockCnt(0)
{
	if(mylocked) lock(wait);
}

Lock::~Lock() {
	// on destruct, remove all locks.
	if(lockCnt > 0) {
		lockable.give();
	}
	lockCnt = 0;
}

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

void Lock::unlock() {
	if(lockCnt > 0) {	// ignore extra unlocks.
		lockCnt--;
		if(lockCnt == 0) {
			lockable.give();
		}
	}
}
