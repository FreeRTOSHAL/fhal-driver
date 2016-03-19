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
#ifndef ACCEL_H_
#define ACCEL_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal.h>
#include <vec.h>
/**
 * \defgroup ACCEL Accelerator Subsystem
 * \ingroup HAL
 * \code
 * #include <accel.h>
 * \endcode
 * 
 * This is the Accelerator Subsystem.
 * \{
 */
/**
 * Private Structure of Accelerator driver
 */
struct accel;
#ifdef CONFIG_ACCEL_MULTI
/**
 * Function of Accelerator driver in Multi Driver implementation mode 
 */
struct accel_ops {
	struct accel *(*accel_init)(uint32_t index);
	int32_t (*accel_deinit)(struct accel *accel);

	int32_t (*accel_get)(struct accel *accel, struct vector *vector, TickType_t waittime);
	int32_t (*accel_getISR)(struct accel *accel, struct vector *vector);
};
#endif
/**
 * Generic Accelerator Interface  
 */
struct accel_generic {
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
#ifdef CONFIG_ACCEL_THREAD_SAVE
	/**
	 * Mutex
	 */
	SemaphoreHandle_t lock;	
#endif
#ifdef CONFIG_ACCEL_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct accel_ops *ops;
#endif
};

#ifndef CONFIG_ACCEL_MULTI
/**
 * Init Accelerator
 * \param index Index of Accelerator
 * \return Accelerator Instance or NULL on error
 */
struct accel *accel_init(uint32_t index);
/**
 * Deinit Accelerator
 * \param accel Accelerator Instance
 * \return -1 on error 0 on ok
 */
int32_t accel_deinit(struct accel *accel);
/**
 * Get Accelerator Value
 * \param accel Accelerator Instance
 * \param vector Values
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t accel_get(struct accel *accel, struct vector *vector, TickType_t waittime);
/**
 * Get Accelerator Value in Interrupt Service Routine
 * \param accel Accelerator Instance
 * \param vector Values
 * \return -1 on error 0 on ok
 */
int32_t accel_getISR(struct accel *accel, struct vector *vector);
#else
inline struct accel *accel_init(uint32_t index) {
	HAL_DEFINE_GLOBAL_ARRAY(accel);
	struct accel_generic *a = (struct accel_generic *) HAL_GET_DEV(accel, index);
	if (a == NULL) {
		return NULL;
	}
	return a->ops->accel_init(index);
}
inline int32_t accel_deinit(struct accel *accel) {
	struct accel_generic *a = (struct accel_generic *) accel;
	return a->ops->accel_deinit(accel);
}

inline int32_t accel_get(struct accel *accel, struct vector *vector, TickType_t waittime) {
	struct accel_generic *a = (struct accel_generic *) accel;
	return a->ops->accel_get(accel, vector, waittime);
}
inline int32_t accel_getISR(struct accel *accel, struct vector *vector) {
	struct accel_generic *a = (struct accel_generic *) accel;
	return a->ops->accel_getISR(accel, vector);
}
#endif
/**\}*/
#endif
