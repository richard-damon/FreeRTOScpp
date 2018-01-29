/**
 * @file TimerCPP.h
 * @brief FreeRTOS Timer Wrapper
 *
 * This file contains a set of lightweight wrappers for tasks using FreeRTOS
 *
 * @copyright (c) 2016 Richard Damon
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

#include "FreeRTOS.h"
#include "timers.h"

class Timer {
public:
	Timer(char const* name_, void(*func)(TimerHandle_t handle), TickType_t period_, bool reload, bool start_) :
	handle(
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
		xTimerCreateStatic(name_, period_, reload, this, func, &timerBuffer)
#else
		xTimerCreate(name_, period_, reload, this, func)
#endif
		)
	{
		if(start_) start();
	}
	virtual ~Timer() {xTimerDelete(handle, portMAX_DELAY); }

	bool 		active() { return xTimerIsTimerActive(handle); }
	TickType_t	expiryTime() { return xTimerGetExpiryTime(handle); }
	const char* name() { return pcTimerGetName(handle); }
	TickType_t  period() { return xTimerGetPeriod(handle); }
	bool		period(TickType_t period_, TickType_t wait = portMAX_DELAY) { return xTimerChangePeriod(handle, period_, wait);}
	bool		periodISR(TickType_t period_, portBASE_TYPE& waswoken) {return xTimerChangePeriodFromISR(handle, period_, &waswoken); }
	bool		reset(TickType_t wait = portMAX_DELAY) { return xTimerReset(handle, wait); }
	bool		resetISR(portBASE_TYPE& waswoken) { return xTimerResetFromISR(handle, &waswoken); }
	bool		start(TickType_t wait = portMAX_DELAY) { return xTimerStart(handle, wait); }
	bool		startISR(portBASE_TYPE& waswoken) { return xTimerStartFromISR(handle, &waswoken); }
	bool		stop(TickType_t wait = portMAX_DELAY) { return xTimerStop(handle, wait); }
	bool		stopISR(portBASE_TYPE& waswoken) { return xTimerStopFromISR(handle, &waswoken); }

protected:
	TimerHandle_t handle;

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
	static void timerClassCallback(TimerHandle_t handle) {
		TimerClass* me = static_cast<TimerClass*>(pvTimerGetTimerID(handle));
		me->timer();
	}
};

template <class T> class TimerMember : public TimerClass {
public:
	TimerMember(char const* name_, T* obj_, void (T::*func_)(), TickType_t period_, UBaseType_t reload) :
		TimerClass(name_, period_, reload),
		obj(obj_),
		func(func_)
	{}
	virtual void timer() { obj->func();}
private:
	T* obj;
	void (T::*func)();
};


#endif /* FREERTOS_FREERTOSPP_TIMERCPP_H_ */
