/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef GYRO_PRV_
#define GYRO_PRV_
#ifndef GYRO_PRV
# error "Never include this file out of a Accel driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#define GYRO_ALREDY_INITED 1
int32_t gyro_generic_init(struct gyro *gyro);
#ifdef CONFIG_GYRO_THREAD_SAFE
# define gyro_lock(u, w, e) HAL_LOCK(u, w, e)
# define gyro_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define gyro_lock(u, w, e)
# define gyro_unlock(u, e)
#endif

#define GYRO_ADDDEV(ns, p) HAL_ADDDEV(gyro, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(gyro);
#define GYRO_GET_DEV(index) HAL_GET_DEV(gyro, index)
#ifndef CONFIG_GYRO_MULTI
# define GYRO_OPS(ns)
# define GYRO_INIT_DEV(ns) 

# define GYRO_INIT(ns, index) struct gyro *gyro_init(uint32_t index)
# define GYRO_DEINIT(ns, a) int32_t gyro_deinit(struct gyro *a)

# define GYRO_GET(ns, a, v, waittime) int32_t gyro_get(struct gyro *a, struct vector *v, TickType_t waittime)
# define GYRO_GET_ISR(ns, a, v) int32_t gyro_getISR(struct gyro *a, struct vector *v)

#else
# define GYRO_OPS(ns) const struct gyro_ops ns##_gyro_ops = { \
	.gyro_init = &ns##_gyro_init, \
	.gyro_deinit = &ns##_gyro_deinit, \
	.gyro_get = &ns##_gyro_get, \
	.gyro_getISR = &ns##_gyro_getISR, \
}
# define GYRO_INIT_DEV(ns) .gen.ops = &ns##_gyro_ops,

# define GYRO_INIT(ns, index) static struct gyro *ns##_gyro_init(uint32_t index)
# define GYRO_DEINIT(ns, a) static int32_t ns##_gyro_deinit(struct gyro *a)

# define GYRO_GET(ns, a, v, waittime) static int32_t ns##_gyro_get(struct gyro *a, struct vector *v, TickType_t waittime)
# define GYRO_GET_ISR(ns, a, v) static int32_t ns##_gyro_getISR(struct gyro *a, struct vector *v)

#endif
#endif

