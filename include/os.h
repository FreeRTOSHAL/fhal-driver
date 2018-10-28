/*
 * Copyright (c) 2016 Andreas Werner <kernel@andy89.org>
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 */
#ifndef OS_H_
#define OS_H_
#include <FreeRTOS.h>
#include <semphr.h>
#include <system.h>
#include <stdbool.h>
/**
 * \defgroup Operation System Layer for FreeRTOS
 * \ingroup HAL
 * 
 * \code
 * #include <os.h>
 * \endcode
 */
#ifdef CONFIG_USE_STATIC
# define OS_DEFINE_TASK(name, stackSize) StaticTask_t name##_buffer; StackType_t name##_stack[stackSize]; TaskHandle_t name; 
# define OS_DEFINE_QUEUE(name, length, size) StaticQueue_t name##_buffer; uint8_t name##_storage[length * size]; QueueHandle_t name 
# define OS_DEFINE_TIMER(name) StaticTimer_t name##_buffer; TimerHandle_t name
# define OS_DEFINE_EVENT_GROUP(name) StaticEventGroup_t name##_buffer; EventGroupHandle_t name
# define OS_DEFINE_SEMARPHORE(name) StaticSemaphore_t name##_buffer; SemaphoreHandle_t name
# define OS_DEFINE_SEMARPHORE_BINARAY(name) OS_DEFINE_SEMARPHORE(name)
# define OS_DEFINE_MUTEX(name) OS_DEFINE_SEMARPHORE(name)
# define OS_DEFINE_MUTEX_RECURSIVE(name) OS_DEFINE_SEMARPHORE(name)

# define OS_CREATE_TASK(function, name, stackSize, parameter, prio, handleName) (\
		{\
			BaseType_t ret; \
			handleName = xTaskCreateStatic(function, name, stackSize, parameter, prio, handleName##_stack, &handleName##_buffer); \
			ret = (handleName == NULL) ? errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY : pdPASS; \
			ret;\
		}\
	)
# define OS_CREATE_QUEUE(length, size, handleName) xQueueCreateStatic(length, size, handleName##_storage, &handleName##_buffer)
# define OS_CREATE_TIMER(name, period, autoReload, timerID, function, handleName) xTimerCreateStatic(name,period, autoReload, timerID, function, &handleName##_buffer)
# define OS_CREATE_EVENT_GROUP(handleName) xEventGroupCreateStatic(&handleName##_buffer)
# define OS_CREATE_SEMARPHORE(maxCount, initialCount, handleName) xSemaphoreCreateCountingStatic(maxCount, initialCount, &handleName##_buffer)
# define OS_CREATE_SEMARPHORE_BINARAY(handleName) xSemaphoreCreateBinaryStatic(&handleName##_buffer)
# define OS_CREATE_MUTEX(handleName) xSemaphoreCreateMutexStatic(&handleName##_buffer)
# define OS_CREATE_MUTEX_RECURSIVE(handleName) xSemaphoreCreateRecursiveMutexStatic(&handleName##_buffer)

#else

# define OS_DEFINE_TASK(name, stackSize) TaskHandle_t name
# define OS_DEFINE_QUEUE(name, length, size) QueueHandle_t name
# define OS_DEFINE_TIMER(name) TimerHandle_t name
# define OS_DEFINE_EVENT_GROUP(name) EventGroupHandle_t name
# define OS_DEFINE_SEMARPHORE(name) SemaphoreHandle_t name
# define OS_DEFINE_SEMARPHORE_BINARAY(name) OS_DEFINE_SEMARPHORE(name)
# define OS_DEFINE_MUTEX(name) OS_DEFINE_SEMARPHORE(name)
# define OS_DEFINE_MUTEX_RECURSIVE(name) OS_DEFINE_SEMARPHORE(name)

# define OS_CREATE_TASK(function, name, stackSize, parameter, prio, handleName) xTaskCreate(function, name, stackSize, parameter, prio, &handleName)
# define OS_CREATE_QUEUE(length, size, handleName) xQueueCreate(length, size)
# define OS_CREATE_TIMER(name, period, autoReload, timerID, function, handleName) xTimerCreate(name,period, autoReload, timerID, function)
# define OS_CREATE_EVENT_GROUP(handleName) xEventGroupCreate()
# define OS_CREATE_SEMARPHORE(maxCount, initialCount, handleName) xSemaphoreCreateCounting(maxCount, initialCount)
# define OS_CREATE_SEMARPHORE_BINARAY(handleName) xSemaphoreCreateBinary()
# define OS_CREATE_MUTEX(handleName) xSemaphoreCreateMutex()
# define OS_CREATE_MUTEX_RECURSIVE(handleName) xSemaphoreCreateRecursiveMutex()
#endif
/**\}*/
#endif
