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
#ifndef TEMP_PRV_
#define TEMP_PRV_
#ifndef TEMP_PRV
# error "Never include this file out of a Accel driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#define TEMP_ALREDY_INITED 1
int32_t temp_generic_init(struct temp *temp);
#ifdef CONFIG_TEMP_THREAD_SAVE
# define temp_lock(u, w, e) HAL_LOCK(u, w, e)
# define temp_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define temp_lock(u, w, e)
# define temp_unlock(u, e)
#endif

#define TEMP_ADDDEV(ns, p) HAL_ADDDEV(temp, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(temp);
#define TEMP_GET_DEV(index) HAL_GET_DEV(temp, index)
#ifndef CONFIG_TEMP_MULTI
# define TEMP_OPS(ns)
# define TEMP_INIT_DEV(ns) 

# define TEMP_INIT(ns, index) struct temp *temp_init(uint32_t index)
# define TEMP_DEINIT(ns, t) int32_t temp_deinit(struct temp *t)

# define TEMP_GET(ns, t, v, waittime) int32_t temp_get(struct temp *t, float *v, TickType_t waittime)
# define TEMP_GET_ISR(ns, t, v) int32_t temp_getISR(struct temp *t, float *v)

#else
# define TEMP_OPS(ns) const struct temp_ops ns##_temp_ops = { \
	.temp_init = &ns##_temp_init, \
	.temp_deinit = &ns##_temp_deinit, \
	.temp_get = &ns##_temp_get, \
	.temp_getISR = &ns##_temp_getISR, \
}
# define TEMP_INIT_DEV(ns) .gen.ops = &ns##_temp_ops,

# define TEMP_INIT(ns, index) static struct temp *ns##_temp_init(uint32_t index)
# define TEMP_DEINIT(ns, t) static int32_t ns##_temp_deinit(struct temp *t)

# define TEMP_GET(ns, t, v, waittime) static int32_t ns##_temp_get(struct temp *t, float *value, TickType_t waittime)
# define TEMP_GET_ISR(ns, t, v) static int32_t ns##_temp_getISR(struct temp *t, float *v)

#endif
#endif


