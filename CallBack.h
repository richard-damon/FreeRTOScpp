/**
 * @file CallBack.h
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
 * @brief FreeRTOS CallBack Wrapper
 *
 * This file provides a template to generate wrappers around member function
 * calls with parameters, including one special one for use with the FreeRTOS
 * Pend / pendFromISR functions which includes the C trampoline function
 *
 * @ingroup FreeRTOSCpp
 */


#ifndef FREERTOSPP_CALLBACK_H
#define FREERTOSPP_CALLBACK_H

#include <FreeRTOS.h>
#include <Timers.h>

template<class Tr = void, class Tp = void>
class CallBack {
public:
	CallBack() {}
	virtual ~CallBack() {}

	virtual Tr	callback(Tp) = 0;
};

template<class Tr>
class CallBack<Tr, void> {
public:
	CallBack() {}
	virtual ~CallBack() {}

	virtual Tr	callback() = 0;
};

extern "C" {
	extern void voidCallbackU32(void* cb, uint32_t parm);
}

template<>
class CallBack<void, uint32_t> {
public:
	CallBack() {}
	virtual ~CallBack() {}

	virtual void	callback(uint32_t) = 0;

	void pend(uint32_t parm, TickType_t ticks = portMAX_DELAY) {
		xTimerPendFunctionCall(&voidCallbackU32, this, parm, ticks);
	}

	void pendFromISR(uint32_t parm, BaseType_t* wasWoken) {
		xTimerPendFunctionCallFromISR(&voidCallbackU32, this, parm, wasWoken);
	}
};

template<class Tc, class Tr = void, class Tp = void>
class MemberFunctionCallback : public CallBack<Tr, Tp> {
public:
	MemberFunctionCallback(Tc* obj_, Tr (Tc::*fun_)(Tp)) : obj(obj_), fun(fun_) {}
	virtual ~MemberFunctionCallback() {};

	virtual Tr 		callback(Tp parm) 			{ return (obj->*fun)(parm); }
			void	setObj(Tc* obj_) 			{ obj = obj_;}
			void	setFun(Tr (Tc::*fun_)(Tp)) 	{ fun = fun_; }
protected:
	Tc*	obj;
	Tr (Tc::*fun)(Tp);
};

template<class Tc, class Tr>
class MemberFunctionCallback<Tc, Tr, void> : public CallBack<Tr, void> {
public:
	MemberFunctionCallback(Tc* obj_, Tr (Tc::*fun_)()) : obj(obj_), fun(fun_) {}
	virtual ~MemberFunctionCallback() {};

	virtual Tr callback() { return (obj->*fun)(); }
	void	setObj(Tc* obj_) { obj = obj_;}
	void	setFun(Tr Tc::*fun_()) { fun = fun_; }
protected:
	Tc*	obj;
	Tr (Tc::*fun)();

};

#endif /* FREERTOS_FREERTOSPP_CALLBACK_H_ */
