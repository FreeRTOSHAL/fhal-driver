/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <errno.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include <task.h>
#include <semphr.h>
#include <os.h>
/**
 * FreeRTOS Mutex for malloc
 */
OS_DEFINE_MUTEX_RECURSIVE(mallockLock) = NULL;

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
		mallockLock = OS_CREATE_MUTEX_RECURSIVE(mallockLock);
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
