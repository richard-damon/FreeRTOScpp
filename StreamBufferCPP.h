/**
 * @file StreamBufferCPP.h
 * @brief FreeRTOS Streambuffer wrapper
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

#ifndef STREAMBUFFER_CPP_H
#define STREAMBUFFER_CPP_H

#include "FreeRTOScpp.h"

#include "stream_buffer.h"

#if FREERTOSCPP_USE_NAMESPACE
namespace FreeRTOScpp {
#endif

    /**
     * @brief Base class for the Various Stream Buffers
     *
     * This class provides all the generic operations.
     *
     * The derived class will create the buffer, or can be created as a wrapper from an existing streambuffer.
     */
class StreamBufferBase {
public:
    StreamBufferBase(StreamBufferHandle_t sbHandle) : streamHandle(sbHandle) {}
    virtual ~StreamBufferBase() { }

    size_t send(const void* data, size_t len, TickType_t delay = portMAX_DELAY) 
        {return xStreamBufferSend(streamHandle, data, len, delay);} 
#if FREERTOSCPP_USE_CHRONO
    size_t send(const void* data, size_t len, Time_ms delay) 
        {return xStreamBufferSend(streamHandle, data, len, ms2ticks(delay));} 
#endif        
    size_t send_ISR(const void* data, size_t len, BaseType_t &wasWoken) 
        {return xStreamBufferSendFromISR(streamHandle, data, len, &wasWoken);} 

    size_t read(void* data, size_t len, TickType_t delay = portMAX_DELAY) 
        {return xStreamBufferReceive(streamHandle, data, len, delay);} 
#if FREERTOSCPP_USE_CHRONO
    size_t read(void* data, size_t len, Time_ms delay) 
        {return xStreamBufferReceive(streamHandle, data, len, ms2ticks(delay));} 
#endif        
    size_t read_ISR(void* data, size_t len, BaseType_t &wasWoken) 
        {return xStreamBufferReceiveFromISR(streamHandle, data, len, &wasWoken);} 

    /// @brief  Get number of bytes of data available in the StreamBuffer
    /// @return The number of bytes that can be read
    size_t waiting() const { return xStreamBufferBytesAvailable(streamHandle);}

    /// @brief Get the amount of available space open in the StreamBuffer
    /// @return The number of bytes that can be sent before the buffer is full 
    size_t available() const { return xStreamBufferSpacesAvailable(streamHandle);}

    bool isEmpty() const { return xStreamBufferIsEmpty(streamHandle);}

    bool isFull() const { return xStreamBufferIsFull(streamHandle);}

    /// @brief Resets the buffer to empty
    /// @return True if done, stream can not be reset if a task is waiting on the StreamBuffer.
    bool reset() { return xStreamBufferReset(streamHandle);}

    /// @brief Sets the Trigger Level for the StreamBuffer
    /// @param trigger the Trigger Level
    /// @return If trigger level was set (false means trigger bigger than the buffer size)
    bool trigger(size_t trigger) { return xStreamBufferSetTriggerLevel(streamHandle, trigger);}

    StreamBufferHandle_t streamHandle;
};

/**
 * StreamBuffer wrapper
 *
 * @tparam size The size of the stream buffer, 0 for dynamically created
 */
template <size_t size
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	= 0
#endif
> 
class StreamBuffer : public StreamBufferBase {
public: 
    StreamBuffer(size_t trigger = 1) :
        StreamBufferBase(
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
            xStreamBufferCreateStatic(size, trigger, storage, streamBuff)
#else
            xStreamBufferCreate(size, trigger)
#endif
        ) {}
    virtual ~StreamBuffer() { vStreamBufferDelete(streamHandle);}

#if configUSE_SB_COMPLETED_CALLBACK
    StreamBuffer(size_t trigger, StreamBufferCallbackFunction_t sendCallback, StreamBufferCallbackFunction_t recvCallback) :
        StreamBufferBase(
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
            xStreamBufferCreateStaticWithCallback(size, trigger, storage, streamBuff, sendCallback, recvCallback)
#else
            xStreamBufferCreateWithCallback(size, trigger, sendCallback. recvCallBack)
#endif
        ) 
    {} 
#endif //configUSE_SB_COMPLETED_CALLBACK

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    uint8_t storage[size+1];
    StaticStreamBuffer_t streamBuff;
#endif
};

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )

/**
 * Dynamically created StreamBuffer
 */
template <> 
class StreamBuffer<0> : public StreamBufferBase {
public:
    StreamBuffer(size_t size, size_t trigger=1) :
        StreamBufferBase(xStreamBufferCreate(size, trigger)) 
        {}  
#if configUSE_SB_COMPLETED_CALLBACK
    StreamBuffer(size_t size, size_t trigger, StreamBufferCallbackFunction_t sendCallback, StreamBufferCallbackFunction_t recvCallback) :
        StreamBufferBase(xStreamBufferCreateWithCallback(size, trigger, sendCallback, recvCallback))
    {} 
#endif // configUSE_SB_COMPLETED_CALLBACK   
};
#endif

#if FREERTOS_VERSION_ALL >= 11'001'000
// Version 11.1.0 added StraamBatchBuffer Varient

/**
 * BatchingBuffer variant wrapper
 *
 * Batching buffers are like normal streambuffers but don't return partial buffer
 * until the timeout period has expired even if some data is available.
 */
template <size_t size
#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
	= 0
#endif
> 
class BatchingBuffer : public StreamBufferBase {
public: 
    BatchingBuffer(size_t trigger = 1) :
        StreamBufferBase(
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
            xStreamBatchingBufferCreateStatic(size, trigger, storage, streamBuff)
#else
            xStreamBatchingBufferCreate(size, trigger)
#endif
        ) {}
    virtual ~BatchBuffer() { vStreamBufferDelete(streamHandle);}
#if configUSE_SB_COMPLETED_CALLBACK
    BatchingBuffer(size_t trigger, StreamBufferCallbackFunction_t sendCallback, StreamBufferCallbackFunction_t recvCallback) :
        StreamBufferBase(
#if( configSUPPORT_STATIC_ALLOCATION == 1 )
            xStreamBatchingBufferCreateStaticWithCallback(size, trigger, storage, streamBuff, sendCallback, recvCallback)
#else
            xStreamBatchingBufferCreateWithCallback(size, trigger, sendCallback. recvCallBack)
#endif
        ) 
    {} 
#endif //configUSE_SB_COMPLETED_CALLBACK

#if( configSUPPORT_STATIC_ALLOCATION == 1 )
    uint8_t storage[size+1];
    StaticStreamBuffer_t streamBuff;
#endif
};

#if( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
/**
 * Dynamically Created BatchingBuffer wrapper.
 */

template <> 
class BatchingBuffer<0> : public StreamBufferBase {
public:
    BatchingBuffer(size_t size, size_t trigger=1) :
        StreamBufferBase(xStreamBatchingBufferCreate(size, trigger)) 
        {}  
    virtual ~BatchingBuffer() { vStreamBufferDelete(streamHandle);}
#if configUSE_SB_COMPLETED_CALLBACK
    BatchingBuffer(size_t size, size_t trigger, StreamBufferCallbackFunction_t sendCallback, StreamBufferCallbackFunction_t recvCallback) :
        StreamBatchingBufferBase(xStreamBufferCreateWithCallback(size, trigger, sendCallback, recvCallback))
    {} 
#endif // configUSE_SB_COMPLETED_CALLBACK   
};
#endif
#endif

#if FREERTOSCPP_USE_NAMESPACE
}
#endif

#endif