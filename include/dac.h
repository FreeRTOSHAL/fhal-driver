/* SPDX-License-Identifier: MIT */
/*
 * Author: David Brandt <david@wakka.de>
 * Date: 2021
 */
#ifndef DAC_H_
#define DAC_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal.h>
/**
 * \defgroup DAC DAC Subsystem
 * \ingroup HAL
 * \code
 * #include <dac.h>
 * \endcode
 *
 * This is the DAC Subsystem for controlling DAC of a SOC. 
 * \{
 */
/**
 * Private Structure of DAC driver
 */
struct dac;
#ifdef CONFIG_DAC_MULTI
/**
 * Function of DAC driver in Multi Driver implementation mode 
 */
struct dac_ops {
	struct dac *(*dac_init)(uint32_t index, uint8_t bits);
	int32_t (*dac_deinit)(struct dac *dac);
	int32_t (*dac_set)(struct dac *dac, uint32_t data, TickType_t waittime);
	int32_t (*dac_setISR)(struct dac *dac, uint32_t data);
};
#endif
/**
 * Generic DAC Interface  
 */
struct dac_generic {
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
#ifdef CONFIG_DAC_THREAD_SAVE
	/**
	 * Mutex
	 */
	OS_DEFINE_MUTEX_RECURSIVE(lock);
#endif
#ifdef CONFIG_DAC_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct dac_ops *ops;
#endif
};
#ifndef CONFIG_DAC_MULTI
/**
 * init DAC instance
 * \param index Index of PWM
 * \param bits bit width of DAC common: 4, 8, 12. 0 on only set instance
 * \return DAC Instance NULL on Error if only set instance on bits == 0 
 */
struct dac *dac_init(uint32_t index, uint8_t bits);
/**
 * Deinit DAC
 * \param dac DAC instance
 * \return -1 on error 0 on ok
 */
int32_t dac_deinit(struct dac *dac);
/**
 * Get DAC Value
 * \param dac DAC instance
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error >= 0 Value
 */
int32_t dac_set(struct dac *dac, uint32_t data, TickType_t waittime);
/**
 * Get DAC Value in Interrupt Service Routine
 * \param dac DAC instance
 * \return -1 on error >= 0 Value
 */
int32_t dac_setISR(struct dac *dac, uint32_t data);
#else
inline struct dac *dac_init(uint32_t index, uint8_t bits) {
	HAL_DEFINE_GLOBAL_ARRAY(dac);
	struct dac_generic *a = (struct dac_generic *) HAL_GET_DEV(dac, index);
	if (a == NULL) {
		return NULL;
	}
	return a->ops->dac_init(index, bits);
}
inline int32_t dac_deinit(struct dac *dac) {
	struct dac_generic *a = (struct dac_generic *) dac;
	return a->ops->dac_deinit(dac);
}
inline int32_t dac_set(struct dac *dac, uint32_t data, TickType_t waittime) {
	struct dac_generic *a = (struct dac_generic *) dac;
	return a->ops->dac_set(dac, data, waittime);
}
inline int32_t dac_setISR(struct dac *dac, uint32_t data) {
	struct dac_generic *a = (struct dac_generic *) dac;
	return a->ops->dac_setISR(dac, data);
}
#endif
/**\}*/
#endif
