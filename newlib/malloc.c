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
#include <errno.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include <task.h>
#include <semphr.h>
/**
 * FreeRTOS Mutex for malloc
 */
static SemaphoreHandle_t mallockLock = NULL;

/**
 * Override __malloc_lock Method to enable Multi Thread Support
 */
void __wrap___malloc_lock (struct _reent *ptr) {
	BaseType_t semStatus;
	(void) ptr;

	/*
	 * Protect against recursion
	 * the Semaphore Create with heap space called pvPortMalloc 
	 * malloc -> mallock_lock -> xSemaphoreCreateMutex -> pvPortMalloc -> malloc -> ...
	 */
	if (mallockLock == (SemaphoreHandle_t) -1) {
		goto malloc_lock_ignore;
	} else if (mallockLock == NULL) {
		/**
		 * Set to -1 Protect against recursion
		 */
		mallockLock = (SemaphoreHandle_t) -1;
		/* 
		 * Create Mutex 
		 */
		mallockLock = xSemaphoreCreateRecursiveMutex();
	}
	/**
	 * Wait for get Semaphore
	 */
	do {
#ifdef INCLUDE_vTaskSuspend
		/**
		 * Take or Suspend 
		 */
		semStatus = xSemaphoreTakeRecursive(mallockLock, portMAX_DELAY);
#else
		/**
		 * Take or Poll every 100 Ticks
		 */
		semStatus = xSemaphoreTakeRecursive(mallockLock, 100);
#endif
	} while(!semStatus);
malloc_lock_ignore:
	return;
}
/**
 * Override __malloc_unlock Method to enable Multi Thread Support
 */
void __wrap___malloc_unlock (struct _reent *ptr) {
	if (mallockLock == (SemaphoreHandle_t) -1) {
		goto malloc_unlock_ignore;
	}
	xSemaphoreGiveRecursive(mallockLock);
malloc_unlock_ignore:
	return;
}
