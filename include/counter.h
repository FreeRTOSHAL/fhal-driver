/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef COUNTER_H_
#define COUNTER_H_
#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <hal.h>
/**
 * \defgroup counter_driver Counter Subsystem
 * \ingroup HAL
 * \code
 * #include <counter.h>
 * \endcode
 * \{
 */
enum counter_mode {
	COUNTER_FALLING,
	COUNTER_RISING,
	COUNTER_EITHER,
};
/**
 * Handle of a Instants of Counter
 * 
 * The data of this Handle is private and only the driver has access to the data. 
 */
struct counter;
#ifdef CONFIG_COUNTER_MULTI
/**
 * Function of a driver in Multi Driver implementation mode 
 */
struct counter_ops {
	struct counter *(*counter_init)(uint32_t index, enum counter_mode mode);
	int32_t (*counter_deinit)(struct counter *counter);
	int64_t (*counter_getValue)(struct counter *counter);
	int32_t (*counter_reset)(struct counter *counter);
};
#endif
/**
 * All driver shall implement this Struct
 */
struct counter_generic {
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
#ifdef CONFIG_COUNTER_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct counter_ops *ops;
#endif
};
#ifndef CONFIG_COUNTER_MULTI
/**
 * Init Function
 * \param index in counters Array
 * \param mode @see{counter_mode}
 * \return Counter Instants or NULL
 */
struct counter *counter_init(uint32_t index, enum counter_mode mode);
/**
 * Deinit Driver Instants 
 * \param counter Instant
 * \return 0 on ok -1 on failure
 */
int32_t counter_deinit(struct counter *counter);
/**
 * Get Counter Value
 * \param counter Instant
 * \return >= 0 value -1 on error
 */
int64_t counter_getValue(struct counter *counter);
/**
 * \param counter Instant
 * \return 0 on ok -1 on error
 */
int32_t counter_reset(struct counter *counter);
#else
inline struct counter *counter_init(uint32_t index, enum counter_mode mode) {
	HAL_DEFINE_GLOBAL_ARRAY(counter);
	struct counter_generic *p = (struct counter_generic *) HAL_GET_DEV(counter, index);
	if (p == NULL) {
		return NULL;
	}
	return p->ops->counter_init(index, mode);
}
inline int32_t counter_deinit(struct counter *counter) {
	struct counter_generic *p = (struct counter_generic *) counter;
	return p->ops->counter_deinit(counter);
}
inline int64_t counter_getValue(struct counter *counter) {
	struct counter_generic *p = (struct counter_generic *) counter;
	return p->ops->counter_getValue(counter);
}
inline int32_t counter_reset(struct counter *counter) {
	struct counter_generic *p = (struct counter_generic *) counter;
	return p->ops->counter_reset(counter);
}
#endif
/**\}*/
#endif

