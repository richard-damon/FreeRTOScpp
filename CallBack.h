/**
 * @file Callback.h
 *
 * @brief FreeRTOS CallBack Wrapper
 *
 * This file provides a template to generate wrappers around member function
 * calls with parameters, including one special one for use with the FreeRTOS
 * Pend / pendFromISR functions which includes the C trampoline function
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

#ifndef FREERTOSPP_CALLBACK_H
#define FREERTOSPP_CALLBACK_H

#include <FreeRTOScpp.h>
#include <timers.h>
#include <stdint.h>

extern "C" {
    /**
     * @brief Define a "C" Callback trampline suitable for Timer Callbacks.
     *
     * This is a "C" API function that takes a void* and a uint32_t points,
     * like a FreeRTOS Timer callback that makes that FreeRTOS callback
     * link to our CallBack templates using the "C" Callbacks void* parameter.
     * This allows a CallBack<void, uint32_t> to be used for a timer
     *
     * @param cb The Callback Object to use
     * @param parm The Parameter to send to the Callback
     */
	extern void voidCallbackU32(void* cb, uint32_t parm);
}

#if FREERTOSCPP_USE_NAMESPACE
namespace FreeRTOScpp {
#endif

/**
 * @brief Base callback with two parameters
 *
 * This is an abstract base class, where the derived class should define the callBack function.
 *
 * @tparam Tr The return type for the callback function
 * @tparam Tp1 The first parameter type, if void, there are no parameters
 * @tparam Tp2 The second parameter type, if void, only one parameter (unless Tp1 is void)
*/
template<class Tr = void, class Tp1 = void, class Tp2 = void>
class CallBack {
public:
	CallBack() {}
	virtual ~CallBack() {}

	virtual Tr	callback(Tp1, Tp2) = 0;
};

/**
 * Specialization of CallBack with only one parameter
*/
template<class Tr, class Tp>
class CallBack<Tr, Tp> {
public:
	CallBack() {}
	virtual ~CallBack() {}

	virtual Tr	callback(Tp) = 0;
};

/**
 * Specialization of CallBack with no parameters
*/
template<class Tr>
class CallBack<Tr> {
public:
	CallBack() {}
	virtual ~CallBack() {}

	virtual Tr	callback() = 0;
};

/**
 * Specialization for One parameter of uint32_t, as that can be pended by FreeRTOS
 *
 * Add the pend operation (and pendFromISR)
*/
template<>
class CallBack<void, uint32_t> {
public:
	CallBack() {}
	virtual ~CallBack() {}

	virtual void	callback(uint32_t) = 0;
#if INCLUDE_xTimerPendFunctionCall
	void pend(uint32_t parm, TickType_t ticks = portMAX_DELAY) {
		xTimerPendFunctionCall(&voidCallbackU32, this, parm, ticks);
	}
#if FREERTOSCPP_USE_CHRONO
    void pend(uint32_t parm, Time_ms ms) {
        xTimerPendFunctionCall(&voidCallbackU32, this, parm, ms2ticks(ms));
    }
#endif

	void pendFromISR(uint32_t parm, BaseType_t* wasWoken) {
		xTimerPendFunctionCallFromISR(&voidCallbackU32, this, parm, wasWoken);
	}
#endif
};

/**
 * Derived Callback to call to a function
 * @tparam Tr
 * @tparam Tp1
 * @tparam Tp2
 */
template<class Tr = void, class Tp1 = void, class Tp2 = void>
class FunctionCallback : public CallBack<Tr, Tp1, Tp2> {
public:
    FunctionCallback(Tr (*fun)(Tp1, Tp2)) : m_fun(fun) {}
    virtual ~FunctionCallback() {};

    virtual Tr 		callback(Tp1 parm1, Tp2 parm2) 	{ return (*m_fun)(parm1, parm2); }

    /// @brief Change the Memberfunction for the Callback
    /// @param fun
    void	setFun(Tr (*fun)(Tp1, Tp2)) 	{ m_fun = fun; }
protected:
    Tr (*m_fun)(Tp1, Tp2);
};

template<class Tr, class Tp>
class FunctionCallback<Tr, Tp> : public CallBack<Tr, Tp> {
public:
    FunctionCallback(Tr (*fun)(Tp)) : m_fun(fun) {}
    virtual ~FunctionCallback() {};

    virtual Tr 		callback(Tp parm) 			{ return m_fun(parm); }
    void	setFun(Tr (*fun)(Tp)) 	{ m_fun = fun; }
protected:
    Tr (*m_fun)(Tp);
};

template<class Tr>
class FunctionCallback<Tr> : public CallBack<Tr, void> {
public:
    FunctionCallback(Tr (*fun)()) : m_fun(fun) {}
    virtual ~FunctionCallback() {};

    virtual Tr callback() { return m_fun(); }
    void	setFun(Tr *fun()) { m_fun = fun; }
protected:
    Tr (*m_fun)();

};

template<class Tc, class Tr = void, class Tp1 = void, class Tp2 = void>
class MemberFunctionCallback : public CallBack<Tr, Tp1, Tp2> {
public:
	MemberFunctionCallback(Tc* obj, Tr (Tc::*fun)(Tp1, Tp2)) : m_obj(obj), m_fun(fun) {}
	virtual ~MemberFunctionCallback() {};

	virtual Tr 		callback(Tp1 parm1, Tp2 parm2) 	{ return (m_obj->*m_fun)(parm1, parm2); }

			/// @brief Change the Object for the Callback
			/// @param obj the new object
			void	setObj(Tc* obj) 			{ m_obj = obj;}
			/// @brief Change the Memberfunction for the Callback
			/// @param fun 
			void	setFun(Tr (Tc::*fun)(Tp1, Tp2)) 	{ m_fun = fun; }
protected:
	Tc*	m_obj;
	Tr (Tc::*m_fun)(Tp1, Tp2);
};

template<class Tc, class Tr, class Tp>
class MemberFunctionCallback<Tc, Tr, Tp> : public CallBack<Tr, Tp> {
public:
	MemberFunctionCallback(Tc* obj, Tr (Tc::*fun)(Tp)) : m_obj(obj), m_fun(fun) {}
	virtual ~MemberFunctionCallback() {};

	virtual Tr 		callback(Tp parm) 			{ return (m_obj->*m_fun)(parm); }
			void	setObj(Tc* obj) 			{ m_obj = obj;}
			void	setFun(Tr (Tc::*fun)(Tp)) 	{ m_fun = fun; }
protected:
	Tc*	m_obj;
	Tr (Tc::*m_fun)(Tp);
};

template<class Tc, class Tr>
class MemberFunctionCallback<Tc, Tr> : public CallBack<Tr, void> {
public:
	MemberFunctionCallback(Tc* obj, Tr (Tc::*fun)()) : m_obj(obj), m_fun(fun) {}
	virtual ~MemberFunctionCallback() {};

	virtual Tr callback() { return (m_obj->*m_fun)(); }
	void	setObj(Tc* obj) { m_obj = obj;}
	void	setFun(Tr Tc::*fun()) { m_fun = fun; }
protected:
	Tc*	m_obj;
	Tr (Tc::*m_fun)();

};

#if FREERTOSCPP_USE_NAMESPACE
}   // namespace FreeRTOScpp
#endif


#endif /* FREERTOS_FREERTOSPP_CALLBACK_H_ */
