/**
 * @file MessageBufferCPP.h
 * @brief FreeRTOS MessageBuffer wrapper
 *
 * Wrapper for FreeRTOS MessageBuffers
 *
 * @copyright (c) 2024 Richard Damon
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

#ifndef MESSAGEBUFFER_CPP_H
#define MESSAGEBUFFER_CPP_H

#include "FreeRTOScpp.h"

#include "message_buffer.h"

#if FREERTOSCPP_USE_NAMESPACE
namespace FreeRTOScpp {
#endif

/**
  * Base Wrapper Class for MessageBuffer
  *
  * Base Class does all the operations, there is then a derived class to build the buffer,
  * or the base class can be a wrapper around an elsewhere created handle.
 */
class MessageBufferBase {
public:
    MessageBufferBase(MessageBufferHandle_t mbHandle) : msgHandle(mbHandle) {}
    virtual ~MessageBufferBase() { }

    size_t send(const void* data, size_t len, TickType_t delay = portMAX_DELAY) 
        {return xMessageBufferSend(msgHandle, data, len, delay);} 
#if FREERTOSCPP_USE_CHRONO
    size_t send(const void* data, size_t len, Time_ms delay) 
        {return xMessageBufferSend(msgHandle, data, len, ms2ticks(delay));} 
#endif        
    size_t send_ISR(const void* data, size_t len, BaseType_t &wasWoken) 
        {return xMessageBufferSendFromISR(msgHandle, data, len, &wasWoken);} 

    size_t read(void* data, size_t len, TickType_t delay = portMAX_DELAY) 
        {return xMessageBufferReceive(msgHandle, data, len, delay);} 
#if FREERTOSCPP_USE_CHRONO
    size_t read(void* data, size_t len, Time_ms delay) 
        {return xMessageBufferReceive(msgHandle, data, len, ms2ticks(delay));} 
#endif        
    size_t read_ISR(void* data, size_t len, BaseType_t &wasWoken) 
        {return xMessageBufferReceiveFromISR(msgHandle, data, len, &wasWoken);} 

    // Message Buffers do not provide "Bytes Available"

    /// @brief Get the amount of available space open in the MessageBuffer
    /// @return The number of bytes that can be sent before the buffer is full 
    size_t available() const { return xMessageBufferSpacesAvailable(msgHandle);}

    bool isEmpty() const { return xMessageBufferIsEmpty(msgHandle);}

    bool isFull() const { return xMessageBufferIsFull(msgHandle);}

    /// @brief Resets the buffer to empty
    /// @return True if done, stream can not be reset if a task is waiting on the MessageBuffer.
    bool reset() { return xMessageBufferReset(msgHandle);}

    MessageBufferHandle_t msgHandle;
};

/**
 * Template to implement a Message Buffer of a given size.
 *
 * MessageBuffer will be created statically if possible.
 *
 * @tparam size The number of bytes to store in the buffer, 0 = dynamically created
 */

template <size_t size
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	= 0
#endif
> 
class MessageBuffer : public MessageBufferBase {
public: 
    MessageBuffer() :
        MessageBufferBase(
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
            xMessageBufferCreateStatic(size, storage, msgBuff)
#else
            xMessageBufferCreate(size)
#endif
        ) {}

    virtual ~MessageBuffer() { vMessageBufferDelete(msgHandle);}

#if configUSE_SB_COMPLETED_CALLBACK
    MessageBuffer(StreamBufferCallbackFunction_t sendCallback, StreamBufferCallbackFunction_t recvCallback) :
        MessageBufferBase(
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
            xMessageBufferCreateStaticWithCallback(size, storage, msgBuff, sendCallback, recvCallback)
#else
            xMessageBufferCreateWithCallback(size, sendCallback. recvCallBack)
#endif
        ) 
    {} 
#endif //configUSE_SB_COMPLETED_CALLBACK

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    uint8_t storage[size+1];
    StaticMessageBuffer_t msgBuff;
#endif
};

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

template <> 
class MessageBuffer<0> : public MessageBufferBase {
public:
    MessageBuffer(size_t size) :
        MessageBufferBase(xMessageBufferCreate(size)) 
        {}
    virtual ~MessageBuffer() { vMessageBufferDelete(msgHandle);}

#if configUSE_SB_COMPLETED_CALLBACK
    MessageBuffer(size_t size, StreamBufferCallbackFunction_t sendCallback, StreamBufferCallbackFunction_t recvCallback) :
        MessageBufferBase(xMessageBufferCreateWithCallback(size, sendCallback, recvCallback))
    {} 
#endif // configUSE_SB_COMPLETED_CALLBACK   
};
#endif

#if FREERTOSCPP_USE_NAMESPACE
}
#endif

#endif