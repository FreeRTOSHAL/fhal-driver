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
#ifndef COUNTER_PRV_
#define COUNTER_PRV_
#ifndef COUNTER_PRV
# error "Never include this file out of a COUNTER driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#define COUNTER_ALREDY_INITED 1
int32_t counter_genericInit(struct counter *counter);
#define COUNTER_ADDDEV(ns, p) HAL_ADDDEV(counter, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(counter);
#define COUNTER_GET_DEV(index) HAL_GET_DEV(counter, index)
#ifndef CONFIG_COUNTER_MULTI
# define COUNTER_OPS(ns)
# define COUNTER_INIT_DEV(ns) 
# define COUNTER_INIT(ns, index, mode) struct counter *counter_init(uint32_t index, enum counter_mode mode)
# define COUNTER_DEINIT(ns, p) int32_t counter_deinit(struct counter *p)

# define COUNTER_GET_VALUE(ns, p) int64_t counter_getValue(struct counter *p)
# define COUNTER_RESET(ns, p) int32_t counter_reset(struct counter *p)
#else
# define COUNTER_OPS(ns) const struct counter_ops ns##_counter_ops = { \
	.counter_init = &ns##_counter_init,\
	.counter_deinit = &ns##_counter_deinit,\
	.counter_getValue = &ns##_counter_getValue,\
	.counter_reset = &ns##_counter_reset,\
}
# define COUNTER_INIT_DEV(ns) .gen.ops = &ns##_counter_ops,

# define COUNTER_INIT(ns, index, mode) static struct counter *ns##_counter_init(uint32_t index, enum counter_mode mode)
# define COUNTER_DEINIT(ns, p) static int32_t ns##_counter_deinit(struct counter *p)
# define COUNTER_GET_VALUE(ns, p) static int64_t ns##_counter_getValue(struct counter *p)
# define COUNTER_RESET(ns, p) static int32_t ns##_counter_reset(struct counter *p)
#endif
#endif

