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
#ifndef ACCEL_PRV_
#define ACCEL_PRV_
#ifndef ACCEL_PRV
# error "Never include this file out of a Accel driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#define ACCEL_ALREDY_INITED 1
int32_t accel_generic_init(struct accel *accel);
#ifdef CONFIG_ACCEL_THREAD_SAVE
# define accel_lock(u, w, e) HAL_LOCK(u, w, e)
# define accel_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define accel_lock(u, w, e)
# define accel_unlock(u, e)
#endif

#define ACCEL_ADDDEV(ns, p) HAL_ADDDEV(accel, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(accel);
#define ACCEL_GET_DEV(index) HAL_GET_DEV(accel, index)
#ifndef CONFIG_ACCEL_MULTI
# define ACCEL_OPS(ns)
# define ACCEL_INIT_DEV(ns) 

# define ACCEL_INIT(ns, index) struct accel *accel_init(uint32_t index)
# define ACCEL_DEINIT(ns, a) int32_t accel_deinit(struct accel *a)

# define ACCEL_GET(ns, a, v, waittime) int32_t accel_get(struct accel *a, struct vector *v, TickType_t waittime)
# define ACCEL_GET_ISR(ns, a, v) int32_t accel_getISR(struct accel *a, struct vector *v)

#else
# define ACCEL_OPS(ns) const struct accel_ops ns##_accel_ops = { \
	.accel_init = &ns##_accel_init, \
	.accel_deinit = &ns##_accel_deinit, \
	.accel_get = &ns##_accel_get, \
	.accel_getISR = &ns##_accel_getISR, \
}
# define ACCEL_INIT_DEV(ns) .gen.ops = &ns##_accel_ops,

# define ACCEL_INIT(ns, index) static struct accel *ns##_accel_init(uint32_t index)
# define ACCEL_DEINIT(ns, a) static int32_t ns##_accel_deinit(struct accel *a)

# define ACCEL_GET(ns, a, v, waittime) static int32_t ns##_accel_get(struct accel *a, struct vector *v, TickType_t waittime)
# define ACCEL_GET_ISR(ns, a, v) static int32_t ns##_accel_getISR(struct accel *a, struct vector *v)

#endif
#endif

