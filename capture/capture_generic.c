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
#include <capture.h>
#define CAPTURE_PRV
#include <capture_prv.h>

int32_t capture_generic_init(struct capture *t) {
	struct capture_generic *capture = (struct capture_generic *) t;
	if (hal_isInit(capture)) {
		return -CAPTURE_ALREDY_INITED;
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
