/**
 * @file Lock.h
 * @brief FreeRTOS Lock wrapper
 *
 * A Generic Locking interface for Lockable objects.
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

#ifndef FREERTOSPP_LOCK_H_
#define FREERTOSPP_LOCK_H_

#include "FreeRTOScpp.h"

#if FREERTOSCPP_USE_NAMESPACE
namespace FreeRTOScpp {
#endif

/**
 * A Base class to provide block based locking capability.
 *
 * Any object that supports "Locking" should be derived from Lockable (like a Semaphore or Mutex)
 *
 * Such objects need to provide an interface consisting of
 * + virtual bool take(TickType_t wait)
 * + virtual bool give();
 *
 * @ingroup FreeRTOSCpp
 */

class Lockable {
public:
	Lockable() {}
	virtual ~Lockable() {}

	virtual bool take(TickType_t wait = portMAX_DELAY) = 0;
#if FREERTOSCPP_USE_CHRONO
	        bool take(Time_ms ms) { return take(ms2ticks(ms)); }
#endif
	virtual bool give() = 0;
private:
#if __cplusplus < 201101L
    Lockable(Lockable const&);      ///< We are not copyable.
    void operator =(Lockable const&);  ///< We are not assignable.
#else
    Lockable(Lockable const&) = delete;      ///< We are not copyable.
    void operator =(Lockable const&) = delete;  ///< We are not assignable.
#endif // __cplusplus
};

/**
 * Class to hold a block based lock. (auto unlocks on in its destructor)
 *
 * Typical calling sequences:
 *
 * @code
 *  // Somewhere global
 *  Mutex mutex
 * @endcode
 * then, in a block
 * @code
 *  {
 *      Lock lock(mutex); // Mutex is taken here
 *  ...
 *  } // and released here
 * @endcode
 *  or
 *  @code
 *
 *  {
 *      Lock lokc(mutex, false);     // don't take it yet
 * ...
 *      if (lock.lock(5)) {          // but take it here, with a maximum timeout
 *         ...
 *         lock.unlock();           // and possible release it
 *      }
 *
 *  } // will be released here if taken and not released
 * @endcode
 * or
 * @code
 *  {
 *      Lock lock(mute, 5);         // Try to take the semaphore with a timeout
 *      if (lock.locked()) {
 *                                  // Semaphore was locked, so we could use it
 *      } else {
 *                                  // Error handling because we couldn't take the semaphore.
 *      }
 *  } // Mutex will be released here if not otherwise release
 * @endcode
 *
 * @ingroup FreeRTOSCpp
 */
class Lock {
public:
	Lock(Lockable& mylockable, bool mylocked = true, TickType_t wait = portMAX_DELAY);
    /**
     * Constructor with assumed locking by specifying lock time
     * @param mylockable The Lockabe object to use
     * @param wait The time it Ticks to wait to get the lock.
     */
	Lock(Lockable& mylockable, TickType_t wait) : Lock(mylockable, true, wait) {}
#if FREERTOSCPP_USE_CHRONO
    Lock(Lockable& mylockable, Time_ms wait);
#endif
	virtual ~Lock();

	bool lock(TickType_t wait = portMAX_DELAY);
#if FREERTOSCPP_USE_CHRONO
	bool lock(Time_ms ms) { return lock(ms2ticks(ms)); }
#endif
	void unlock();
    /**
     * Do we have the lock?
     * @return True if we have the lock.
     */
	bool locked() const { return lockCnt > 0; }
private:
	Lockable& lockable; ///< The Lockage object we are connected to
	int		lockCnt;    ///< The number of locks we hold on lockable.

#if __cplusplus < 201101L
    Lock(Lock const&);                      ///< We are not copyable.
    void operator =(Lock const&);           ///< We are not assignable.
#else
    Lock(Lock const&) = delete;             ///< We are not copyable.
    void operator =(Lock const&) = delete;  ///< We are not assignable.
#endif // __cplusplus
};

#if FREERTOSCPP_USE_NAMESPACE
}   // namespace FreeRTOScpp
#endif
#endif /* FREERTOSPP_LOCK_H_ */
