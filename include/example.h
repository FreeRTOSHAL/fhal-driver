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
#ifndef EXAMPLE_H_
#define EXAMPLE_H_
#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <semphr.h>
#include <hal.h>
/**
 * \defgroup example_driver Example driver 
 * \ingroup HAL
 * \code
 * #include <example.h>
 * \endcode
 * 
 * This is a implementation of a sample driver. This Driver interface has no function and is only for demonstration how to write a new driver 
 * interface and implement a driver. 
 * 
 * Use a Instance of the driver:
 * \code
 * #include <devs.h>
 * #include <example.h>
 * struct example *ex1;
 * struct example *ex2;
 * int32_t ret;
 * ex1 = example_init(EXAMPLE1); // EXAMPLE1 is defined by Hartware Interface
 * CONFIG_ASSERT(ex1 != NULL);
 * ex2 = example_init(EXAMPLE2);
 * CONFIG_ASSERT(ex1 != NULL);
 * //Call Funcname on device EXAMPLE1
 * ret = example_funcname(ex1, 0x42);
 * CONFIG_ASSERT(ret >= 0);
 * //Call Funcname on device EXAMPLE2
 * ret = example_funcname(ex2, 0x42);
 * CONFIG_ASSERT(ret >= 0);
 * \endcode
 * \{
 */
/**
 * Handle of a Instants of Example
 * 
 * The data of this Handle is private and only the driver has access to the data. 
 */
struct example;
#ifdef CONFIG_EXAMPLE_MULTI
/**
 * Function of a driver in Multi Driver implementation mode 
 */
struct example_ops {
	/**
	 * Init Function
	 * \param index in examples Array
	 * \return Example Instants or NULL
	 */
	struct example *(*example_init)(uint32_t index);
	/**
	 * Deinit Driver Instants 
	 * \param example Instant
	 * \return 0 on ok -1 on failure
	 */
	int32_t (*example_deinit)(struct example *example);
	/**
	 * Sample Function for a driver
	 * \param example Instant
	 * \param params sample param
	 * \return 0 on ok -1 on error
	 */
	int32_t (*example_funcname)(struct example *example, uint32_t params);
};
#endif
/**
 * All driver shall implement this Struct
 * 
 * For Example: 
 * \code{.c}
 * struct example {
 * 	struct example_generic gen;
 * }
 * \endcode
 */
struct example_generic {
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
#ifdef CONFIG_EXAMPLE_THREAD_SAVE
	/**
	 * Mutex
	 */
	SemaphoreHandle_t lock;	
#endif
#ifdef CONFIG_EXAMPLE_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct example_ops *ops;
#endif
};
#ifndef CONFIG_EXAMPLE_MULTI
/**
 * Init Function
 * \param index in examples Array
 * \return Example Instants or NULL
 */
struct example *example_init(uint32_t index);
/**
 * Deinit Driver Instants 
 * \param example Instant
 * \return 0 on ok -1 on failure
 */
int32_t example_deinit(struct example *example);
/**
 * Sample Function for a driver
 * \param example Instant
 * \param params sample param
 * \return 0 on ok -1 on error
 */
int32_t example_funcname(struct example *example, uint32_t params);
#else
/**
 * Init Function
 * \param index in examples Array
 * \return Example Instants or NULL
 */
inline struct example *example_init(uint32_t index) {
	/*
	 * Define Global Array in local space
	 * Userspace din't need it
	 */
	HAL_DEFINE_GLOBAL_ARRAY(example);
	/*
	 * Get Dev from array if NULL dev not exists 
	 */
	struct example_generic *p = (struct example_generic *) HAL_GET_DEV(example, index);
	if (p == NULL) {
		return NULL;
	}
	return p->ops->example_init(index);
}
/**
 * Deinit Driver Instants 
 * \param example Instant
 * \return 0 on ok -1 on failure
 */
inline int32_t example_deinit(struct example *example) {
	struct example_generic *p = (struct example_generic *) example;
	return p->ops->example_deinit(example);
}
/**
 * Sample Function for a driver
 * \param example Instant
 * \param params sample param
 * \return 0 on ok -1 on error
 */
inline int32_t example_funcname(struct example *example, uint32_t params) {
	struct example_generic *p = (struct example_generic *) example;
	return p->ops->example_funcname(example, params);
}
#endif
/**\}*/
#endif
