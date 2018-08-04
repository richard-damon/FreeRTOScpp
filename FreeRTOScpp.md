@page FreeRTOSpp FreeRTOS C++ Wrappers

FreeRTOScpp is a package that provides a light C++ wrapper for FreeRTOS (http://freertos.org).

It provides wrappers for the Task, Queue, Semaphore, and Mutex (including RecursiveMutex).

I find the big advantage is that these allow me to declare these items as (part of) 
global objects, and then the items are automatically created and configured, without 
needing to change the main() function.

They also provide member function style access for most of the basic operations
on the items.

See @ref FreeRTOSCpp

@defgroup FreeRTOSCpp FreeRTOC C++ Wrapper
See @ref FreeRTOSpp