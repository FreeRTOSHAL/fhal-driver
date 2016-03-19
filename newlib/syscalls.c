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
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>

#include <gnr_uart.h>
#include "FreeRTOS.h"
#include <task.h>
#include <semphr.h>

#ifndef CONFIG_STD_DEBUG_PORT
#define CONFIG_STD_DEBUG_PORT 1
#endif

#ifndef CONFIG_STD_REPLACE_LF
/**
 * Replace '\n' to '\n\r' 
 * We shout use UNIX Lineending in Code
 * 
 * mincom use '\n\r' by Default
 */
#define CONFIG_STD_REPLACE_LF 1
#endif

/**
 * close - close a file descriptor
 * 
 * \param file File No
 * \return -1 not Supported
 */
int _close(int file) {
	(void) file;
	errno = EPERM;
	return -1;
}

/**
 * fstat - get file status
 * 
 * Status of an open file. For consistency with other minimal implementations in these examples, 
 * all files are regarded as character special devices. The sys/stat.h header file required is 
 * distributed in the include subdirectory for this C library.
 * 
 * \param file File No
 * \param st File Stat
 * \return 0 minimal Implementation
 */
int _fstat(int file, struct stat *st) {
	(void) file;
	st->st_mode = S_IFCHR;
	return 0;
}

/**
 * isatty - test whether a file descriptor refers to a terminal
 * 
 * Query whether output stream is a terminal. For consistency with the other minimal implementations, 
 * which only support output to stdout, this minimal implementation is suggested
 * 
 * \param file File No
 * \return 
 */
int _isatty(int file) {
	(void) file;
	return 1;
}

/**
 * lseek - reposition read/write file offset
 * 
 * Set position in a file.
 * 
 * Not implement yet
 *
 * \param file File No
 * \param ptr offset
 * \param dir SEEK_SET, SEEK_CUR, SEEK_END
 * \return new offset
 */
int _lseek(int file, int ptr, int dir) {
	(void) file;
	(void) ptr;
	(void) dir;
	return 0;
}

/**
 * open - open and possibly create a file or device
 * 
 * \param name File to open
 * \param flags Open Flags
 * \param mode Open Mode
 * \return -1 not implement yet
 */
int _open(const char *name, int flags, int mode) {
	(void) name;
	(void) flags;
	(void) mode;
	errno = EPERM;
	return -1;
}

/**
 * read - read from a file descriptor
 *
 * Not implement yet
 * 
 * \param File No
 * \param ptr char pointer
 * \param len count
 * \return On  success, the number of bytes read is returned (zero indicates end of file), and the file 
 *         position is advanced by this number.  It is not an error if this number is smaller than the 
 *         number of bytes requested; this may happen for example because fewer bytes are  actually  
 *         available  right  now  (maybe because we were close to end-of-file, or because we are reading 
 *         from a pipe, or from a terminal), or because read() was interrupted by a signal.  On error, 
 *         -1 is returned, and errno is set appropriately.  In this case it is  left  unspecified  
 *         whether the file position (if any) changes.
 */
int _read(int file, char *ptr, int len) {
	(void) file;
	(void) ptr;
	(void) len;
	errno = EPERM;
	return -1;
}

#ifdef CONFIG_USE_MALLOC_OF_NEWLIB

/**
 * Compled Heap for Programm
 */
static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ];
/**
 * End of Heap
 * 
 * At the beginning no space has bean alloc  
 */
static uint8_t *heap_end = ucHeap;

/**
 * sbrk - change data segment size
 * 
 * Increase program data space. This function only check Heap is growing to high. The freeing Algorithm is located at malloc and free
 * Implementation of Newlib. Newlib use Malloc Algorithm of Doug Lea (http://g.oswego.edu/dl/html/malloc.html). 
 *
 * configTOTAL_HEAP_SIZE of FreeRTOS must been defined
 * 
 * \param incr Size of incrementation is always aligned
 * \return new Pointer address
 */
caddr_t _sbrk(int incr) {
	uint8_t *prev_heap_end;
	/*
	 * Top of Heap is the address of ucHeap + configTOTAL_HEAP_SIZE
	 */
	static uint8_t *heap_top = ucHeap + configTOTAL_HEAP_SIZE;

	prev_heap_end = heap_end;
	if (heap_end + incr > heap_top) {
		goto _sbrk_error_0;
	}
	/*
	 * Check incr is negative or positive  
	 */
	if (incr > 0) {
		prev_heap_end = heap_end;
		heap_end += incr;
	} else {
		heap_end += incr;
		prev_heap_end = heap_end;
	}

	return (caddr_t) prev_heap_end;
_sbrk_error_0:
	return (caddr_t)0;
}

#endif

/**
 * write - write to a file descriptor
 * 
 * \param file File No
 * \param ptr Pointer to data
 * \param len size of data
 * \return On success, the number of bytes written is returned (zero indicates nothing was written).
 * On error, -1 is returned, and errno is set appropriately.
 */
int _write(int file, char *ptr, int len) {
	int i;
	(void) file;
	gnr_uart_dev_t *dev = gnr_uart_get_dev(CONFIG_STD_DEBUG_PORT);
	for (i = 0; i < len; i++) {
		gnr_uart_putc(dev, *ptr);
#ifdef CONFIG_STD_REPLACE_LF
		/*
		 * Send \r after \n
		 */
		if (*ptr == '\n') {
			gnr_uart_putc(dev, '\r');
		}
#endif
		ptr++;
	}
	return len;
}

/**
 * exit - cause normal process termination
 *
 * This function can be redeclar by user space, but think about atexit(2)
 *
 * \param status
 */
void __attribute__((weak)) __attribute__((noreturn)) _exit(int status) {
	(void) status;
	taskDISABLE_INTERRUPTS();
	for (;;) {}
}

/**
 * Application Cleanup function Called before \ref{_exit} over atexit(2)
 * 
 * whithout mainCRT not called
 */
void __attribute__((weak)) _fini() {
	
}


/**
 * FreeRTOS Mutex for malloc
 */
static SemaphoreHandle_t mallockLock = NULL;

/**
 * Override __malloc_lock Method to enable Multi Thread Support
 */
void __wrap___malloc_lock (struct _reent *ptr) {
	BaseType_t semStatus;

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
