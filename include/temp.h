/*
* Copyright (c) 2019 Andreas Werner <kernel@andy89.org>
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
#ifndef TEMP_H_
#define TEMP_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal.h>
#include <vec.h>
/**
 * \defgroup TEMP Temperature Subsystem
 * \ingroup HAL
 * \code
 * #include <accel.h>
 * \endcode
 * 
 * This is the Temperature Subsystem.
 * \{
 */
/**
 * Private Structure of Temperature driver
 */
struct temp;
#ifdef CONFIG_TEMP_MULTI
/**
 * Function of Gyro driver in Multi Driver implementation mode 
 */
struct temp_ops {
	struct temp *(*temp_init)(uint32_t index);
	int32_t (*temp_deinit)(struct temp *temp);

	int32_t (*temp_get)(struct temp *temp, float *value, TickType_t waittime);
	int32_t (*temp_getISR)(struct temp *temp, float *value);
};
#endif
/**
 * Generic Temperature Interface  
 */
struct temp_generic {
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
#ifdef CONFIG_TEMP_THREAD_SAVE
	/**
	 * Mutex
	 */
	OS_DEFINE_MUTEX_RECURSIVE(lock);
#endif
#ifdef CONFIG_TEMP_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct temp_ops *ops;
#endif
};

#ifndef CONFIG_TEMP_MULTI
/**
 * Init Temperature
 * \param index Index of Temperature
 * \return Temperature Instance or NULL on error
 */
struct temp *temp_init(uint32_t index);
/**
 * Deinit Temperature
 * \param temp Temperature Instance
 * \return -1 on error 0 on ok
 */
int32_t temp_deinit(struct temp *temp);
/**
 * Get Temperature Value
 * \param temp Temperature Instance
 * \param value Value
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 *
 * \return -1 on error 0 on ok
 */
int32_t temp_get(struct temp *temp, float *value, TickType_t waittime);
/**
 * Get Temperature Value
 * \param temp Temperature Instance
 * \param value Value
 * \return -1 on error 0 on ok
 */
int32_t temp_getISR(struct temp *temp, float *value);
#else
inline struct temp *temp_init(uint32_t index) {
	HAL_DEFINE_GLOBAL_ARRAY(temp);
	struct temp_generic *a = (struct temp_generic *) HAL_GET_DEV(temp, index);
	if (a == NULL) {
		return NULL;
	}
	return a->ops->temp_init(index);
}
inline int32_t temp_deinit(struct temp *temp) {
	struct temp_generic *a = (struct temp_generic *) temp;
	return a->ops->temp_deinit(temp);
}

inline int32_t temp_get(struct temp *temp, float *value, TickType_t waittime) {
	struct temp_generic *a = (struct temp_generic *) temp;
	return a->ops->temp_get(temp, value, waittime);
}
inline int32_t temp_getISR(struct temp *temp, float *value) {
	struct temp_generic *a = (struct temp_generic *) temp;
	return a->ops->temp_getISR(temp, value);
}
#endif
/**\}*/
#endif

