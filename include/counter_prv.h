/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
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

