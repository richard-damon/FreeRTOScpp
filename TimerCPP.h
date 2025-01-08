/**
 * @file TimerCPP.h
 * @brief FreeRTOS Timer Wrapper
 *
 * This file contains a set of lightweight wrappers for tasks using FreeRTOS
 *
 * @copyright (c) 2016-2024 Richard Damon
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
 *
 */

#ifndef FREERTOS_FREERTOSPP_TIMERCPP_H
#define FREERTOS_FREERTOSPP_TIMERCPP_H

#include "FreeRTOScpp.h"
#include "FreeRTOS.h"
#include "timers.h"

#if FREERTOSCPP_USE_NAMESPACE
namespace FreeRTOScpp {
#endif

/**
 * @ingroup FreeRTOSCpp
 */
class Timer {
public:
	Timer(char const* name_, void(*func)(TimerHandle_t handle), TickType_t period_, bool reload, bool start_) :
	timerHandle(
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
		xTimerCreateStatic(name_, period_, reload, this, func, &timerBuffer)
#else
		xTimerCreate(name_, period_, reload, this, func)
#endif
		)
	{
		if(start_) start();
	}

#if FREERTOSCPP_USE_CHRONO
    Timer(char const* name_, void(*func)(TimerHandle_t handle), Time_ms period_, bool reload, bool start_) :
    timerHandle(
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
        xTimerCreateStatic(name_, ms2ticks(period_), reload, this, func, &timerBuffer)
#else
        xTimerCreate(name_, ms2ticks(period_), reload, this, func)
#endif
        )
    {
        if(start_) start();
    }
#endif // FREERTOSCPP_USE_CHRONO

	virtual ~Timer() {xTimerDelete(timerHandle, portMAX_DELAY); }

	bool 		active() { return xTimerIsTimerActive(timerHandle); }
	TickType_t	expiryTime() { return xTimerGetExpiryTime(timerHandle); }   // TODO Time_ms versions?
	const char* name() { return pcTimerGetName(timerHandle); }
	TickType_t  period() { return xTimerGetPeriod(timerHandle); }
	bool		period(TickType_t period_, TickType_t wait = portMAX_DELAY) { configASSERT(period_ > 0); return xTimerChangePeriod(timerHandle, period_, wait);}
#if FREERTOSCPP_USE_CHRONO
    bool        period(Time_ms period_, TickType_t wait = portMAX_DELAY) { configASSERT(ms2ticks(period_) > 0); return xTimerChangePeriod(timerHandle, ms2ticks(period_), wait);}
    bool        period(Time_ms period_, Time_ms wait) { configASSERT(ms2ticks(period_) > 0); return xTimerChangePeriod(timerHandle, ms2ticks(period_), ms2ticks(wait));}
#endif
	bool		periodISR(TickType_t period_, portBASE_TYPE& waswoken) { configASSERT(period_ > 0); return xTimerChangePeriodFromISR(timerHandle, period_, &waswoken); }
#if FREERTOSCPP_USE_CHRONO
    bool        periodISR(Time_ms period_, portBASE_TYPE& waswoken) { configASSERT(ms2ticks(period_) > 0); return xTimerChangePeriodFromISR(timerHandle, ms2ticks(period_), &waswoken); }
#endif
	bool		reset(TickType_t wait = portMAX_DELAY) { return xTimerReset(timerHandle, wait); }
#if FREERTOSCPP_USE_CHRONO
    bool        reset(Time_ms wait) { return xTimerReset(timerHandle, ms2ticks(wait)); }
#endif
	bool		resetISR(portBASE_TYPE& waswoken) { return xTimerResetFromISR(timerHandle, &waswoken); }

	bool		start(TickType_t wait = portMAX_DELAY) { return xTimerStart(timerHandle, wait); }
#if FREERTOSCPP_USE_CHRONO
    bool        start(Time_ms wait) { return xTimerStart(timerHandle, ms2ticks(wait)); }
#endif
	bool		startISR(portBASE_TYPE& waswoken) { return xTimerStartFromISR(timerHandle, &waswoken); }

	bool		stop(TickType_t wait = portMAX_DELAY) { return xTimerStop(timerHandle, wait); }
#if FREERTOSCPP_USE_CHRONO
    bool        stop(Time_ms wait) { return xTimerStop(timerHandle, ms2ticks(wait)); }
#endif
	bool		stopISR(portBASE_TYPE& waswoken) { return xTimerStopFromISR(timerHandle, &waswoken); }

#if FREERTOS_VERSION >= 10'002'000
	void 		reload(bool reload) { vTimerSetReloadMode( timerHandle, reload); }
#endif

protected:
	TimerHandle_t timerHandle;

private:
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    StaticTimer_t timerBuffer;
#endif
};

class TimerClass : public Timer {
public:
	TimerClass(char const* name_, TickType_t period_, bool reload) :
		Timer(name_, &timerClassCallback, period_, reload, false) // Do not start in Timer, our parents constructor needs to do it.
	{}

	virtual void timer() = 0;	// Function called on timer activation
private:
	static void timerClassCallback(TimerHandle_t timerHandle) {
		TimerClass* me = static_cast<TimerClass*>(pvTimerGetTimerID(timerHandle));
		me->timer();
	}
};

/**
 * @ingroup FreeRTOSCpp
 */
template <class T> class TimerMember : public TimerClass {
public:
	TimerMember(char const* name_, T* obj_, void (T::*func_)(), TickType_t period_, UBaseType_t reload) :
		TimerClass(name_, period_, reload),
		obj(obj_),
		func(func_)
	{}
	virtual void timer() { (obj->*func)();}
private:
	T* obj;
	void (T::*func)();
};

#if FREERTOSCPP_USE_NAMESPACE
}   // namespace FreeRTOScpp
#endif


#endif /* FREERTOS_FREERTOSPP_TIMERCPP_H_ */
