/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <example.h>
#define EXAMPLE_PRV
#include <example_prv.h>

int32_t example_genericInit(struct example *t) {
	struct example_generic *example = (struct example_generic *) t;
	if (hal_isInit(example)) {
		return EXAMPLE_ALREDY_INITED;
	}
#ifdef CONFIG_EXAMPLE_THREAD_SAVE
	{
		int32_t ret = hal_init(example);
		if (ret < 0) {
			goto example_generic_init_error0;
		}
	}
#endif
	example->init = true;
	return 0;
#ifdef CONFIG_EXAMPLE_THREAD_SAVE
example_generic_init_error0:
	return -1;
#endif

}
#ifdef CONFIG_EXAMPLE_MULTI
struct example *example_init(uint32_t index);
int32_t example_deinit(struct example *example);
int32_t example_funcname(struct example *example, uint32_t params);
#endif
