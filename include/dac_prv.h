/* SPDX-License-Identifier: MIT */
/*
 * Author: David Brandt <david@wakka.de>
 * Date: 2021
 */
#ifndef DAC_PRV_H_
#define DAC_PRV_H_
#ifndef DAC_PRV
# error "Never include this file outside of a DAC driver"
#endif
#include <stdbool.h>
#include <stdint.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdbool.h>
#include <stdint.h>
#include <hal.h>
#define DAC_ALREDY_INITED 1
int32_t dac_generic_init(struct dac *dac);
#ifdef CONFIG_DAC_THREAD_SAVE
# define dac_lock(u, w, r) HAL_LOCK(u, w, r)
# define dac_unlock(u, r) HAL_UNLOCK(u, r)
#else
# define dac_lock(u, w, r)
# define dac_unlock(u, r)
#endif
#define DAC_ADDDEV(ns, p) HAL_ADDDEV(dac, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(dac);
#define DAC_GET_DEV(index) HAL_GET_DEV(dac, index)
#ifndef CONFIG_DAC_MULTI
# define DAC_OPS(ns)
# define DAC_INIT_DEV(ns) 
#define DAC_INIT(ns, index, bits) struct dac *dac_init(uint32_t index, uint8_t bits)
#define DAC_DEINIT(ns, a) int32_t dac_deinit(struct dac *a)
#define DAC_SET(ns, a, d, waittime) int32_t dac_set(struct dac *a, uint32_t d, TickType_t waittime)
#define DAC_SET_ISR(ns, a, d) int32_t dac_setISR(struct dac *a, uint32_t d)
#else
# define DAC_OPS(ns) const struct dac_ops ns##_dac_ops = { \
	.dac_init = &ns##_dac_init, \
	.dac_deinit = &ns##_dac_deinit, \
	.dac_set = &ns##_dac_set, \
	.dac_setISR = &ns##_dac_setISR, \
}
# define DAC_INIT_DEV(ns) .gen.ops = &ns##_dac_ops,

#define DAC_INIT(ns, index, bits) static struct dac *ns##_dac_init(uint32_t index, uint8_t bits)
#define DAC_DEINIT(ns, a) static int32_t ns##_dac_deinit(struct dac *a)
#define DAC_SET(ns, a, d, waittime) static int32_t ns##_dac_set(struct dac *a, uint32_t d, TickType_t waittime)
#define DAC_SET_ISR(ns, a, d) static int32_t ns##_dac_setISR(struct dac *a, uint32_t d)
#endif
#endif
