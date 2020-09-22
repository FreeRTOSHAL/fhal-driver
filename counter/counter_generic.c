/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <counter.h>
#define COUNTER_PRV
#include <counter_prv.h>

int32_t counter_genericInit(struct counter *t) {
	struct counter_generic *counter = (struct counter_generic *) t;
	if (hal_isInit(counter)) {
		return COUNTER_ALREDY_INITED;
	}
	counter->init = true;
	return 0;
}
#ifdef CONFIG_COUNTER_MULTI
struct counter *counter_init(uint32_t index, enum counter_mode mode);
int32_t counter_deinit(struct counter *counter);
int64_t counter_getValue(struct counter *counter);
int32_t counter_reset(struct counter *counter);
#endif
