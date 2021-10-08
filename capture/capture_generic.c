/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <capture.h>
#define CAPTURE_PRV
#include <capture_prv.h>
int32_t capture_generic_init(struct capture *t) {
	struct capture_generic *capture = (struct capture_generic *) t;
	if (hal_isInit(capture)) {
		return CAPTURE_ALREDY_INITED;
	}
	capture->init = true;
	return 0;
}
#ifdef CONFIG_CAPTURE_MULTI
struct capture *capture_init(uint32_t index);
int32_t capture_deinit(struct capture *capture);

int32_t capture_setCallback(struct capture *capture, bool (*callback)(struct capture *capture, uint32_t index, uint64_t time, void *data), void *data);

int32_t capture_setPeriod(struct capture *capture, uint64_t us);
uint64_t capture_getTime(struct capture *capture);
uint64_t capture_getChannelTime(struct capture *capture);
#endif
