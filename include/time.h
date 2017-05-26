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
#ifndef TIME_H_
#define TIME_H_
#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <semphr.h>
#include <hal.h>
/**
 * \defgroup time_driver Example driver 
 * \ingroup HAL
 * \code
 * #include <time.h>
 * \endcode
 * 
 * This is a implementation of a sample driver. This Driver interface has no function and is only for demonstration how to write a new driver 
 * interface and implement a driver. 
 * 
 * Use a Instance of the driver:
 * \code
 * #include <devs.h>
 * #include <time.h>
 * struct time *ex1;
 * struct time *ex2;
 * int32_t ret;
 * ex1 = time_init(TIME1); // TIME1 is defined by Hartware Interface
 * CONFIG_ASSERT(ex1 != NULL);
 * ex2 = time_init(TIME2);
 * CONFIG_ASSERT(ex1 != NULL);
 * //Call Funcname on device TIME1
 * ret = time_funcname(ex1, 0x42);
 * CONFIG_ASSERT(ret >= 0);
 * //Call Funcname on device TIME2
 * ret = time_funcname(ex2, 0x42);
 * CONFIG_ASSERT(ret >= 0);
 * \endcode
 * \{
 */
/**
 * Handle of a Instants of Example
 * 
 * The data of this Handle is private and only the driver has access to the data. 
 */
struct time;
#ifdef CONFIG_TIME_MULTI
/**
 * Function of a driver in Multi Driver implementation mode 
 */
struct time_ops {
	/**
	 * Init Function
	 * \param index in times Array
	 * \return Example Instants or NULL
	 */
	struct time *(*time_init)(uint32_t index);
	/**
	 * Deinit Driver Instants 
	 * \param time Instant
	 * \return 0 on ok -1 on failure
	 */
	int32_t (*time_deinit)(struct time *time);
	/**
	 * Sample Function for a driver
	 * \param time Instant
	 * \param params sample param
	 * \return 0 on ok -1 on error
	 */
	int32_t (*time_funcname)(struct time *time, uint32_t params);
};
#endif
/**
 * All driver shall implement this Struct
 * 
 * For Example: 
 * \code{.c}
 * struct time {
 * 	struct time_generic gen;
 * }
 * \endcode
 */
struct time_generic {
	/**
	 * true = is init
	 * false = is not init
	 */
	bool init;
#ifdef CONFIG_INSTANCE_NAME
	/**
	 * Name of Driver Instance for Debugging 
	 */
	const char *name;
#endif
#ifdef CONFIG_TIME_THREAD_SAVE
	/**
	 * Mutex
	 */
	SemaphoreHandle_t lock;	
#endif
#ifdef CONFIG_TIME_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct time_ops *ops;
#endif
};
#ifndef CONFIG_TIME_MULTI
/**
 * Init Function
 * \param index in times Array
 * \return Example Instants or NULL
 */
struct time *time_init(uint32_t index);
/**
 * Deinit Driver Instants 
 * \param time Instant
 * \return 0 on ok -1 on failure
 */
int32_t time_deinit(struct time *time);
/**
 * Sample Function for a driver
 * \param time Instant
 * \param params sample param
 * \return 0 on ok -1 on error
 */
int32_t time_funcname(struct time *time, uint32_t params);
#else
/**
 * Init Function
 * \param index in times Array
 * \return Example Instants or NULL
 */
inline struct time *time_init(uint32_t index) {
	/*
	 * Define Global Array in local space
	 * Userspace din't need it
	 */
	HAL_DEFINE_GLOBAL_ARRAY(time);
	/*
	 * Get Dev from array if NULL dev not exists 
	 */
	struct time_generic *p = (struct time_generic *) HAL_GET_DEV(time, index);
	if (p == NULL) {
		return NULL;
	}
	return p->ops->time_init(index);
}
/**
 * Deinit Driver Instants 
 * \param time Instant
 * \return 0 on ok -1 on failure
 */
inline int32_t time_deinit(struct time *time) {
	struct time_generic *p = (struct time_generic *) time;
	return p->ops->time_deinit(time);
}
/**
 * Sample Function for a driver
 * \param time Instant
 * \param params sample param
 * \return 0 on ok -1 on error
 */
inline int32_t time_funcname(struct time *time, uint32_t params) {
	struct time_generic *p = (struct time_generic *) time;
	return p->ops->time_funcname(time, params);
}
#endif
/**\}*/
#endif
