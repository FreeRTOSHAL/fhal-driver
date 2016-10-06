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
#ifndef CAPTURE_PRV_
#define CAPTURE_PRV_
#ifndef CAPTURE_PRV
# error "Never include this file out of a Capture driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#define CAPTURE_ALREDY_INITED 1
int32_t capture_generic_init(struct capture *capture);
#define CAPTURE_ADDDEV(ns, p) HAL_ADDDEV(capture, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(capture);
#define CAPTURE_GET_DEV(index) HAL_GET_DEV(capture, index)
#ifndef CONFIG_CAPTURE_MULTI
# define CAPTURE_OPS(ns)
# define CAPTURE_INIT_DEV(ns) 

# define CAPTURE_INIT(ns, index) struct capture *capture_init(uint32_t index)
# define CAPTURE_DEINIT(ns, c) int32_t capture_deinit(struct capture *c)

# define CAPTURE_SET_CALLBACK(ns, c, callback, data) int32_t capture_setCallback(struct capture *c, bool (*callback)(struct capture *capture, uint32_t index, uint64_t time, void *data), void *data)

# define CAPTURE_SET_PERIOD(ns, c, us) int32_t capture_setPeriod(struct capture *c, uint64_t us)
# define CAPTURE_GET_TIME(ns, c) uint64_t capture_getTime(struct capture *c)
# define CAPTURE_GET_CHANNEL_TIME(ns, c) uint64_t capture_getChannelTime(struct capture *c)
#else
# define CAPTURE_OPS(ns) static const struct capture_ops ns##_capture_ops = { \
	.capture_init = &ns##_capture_init,\
	.capture_deinit = &ns##_capture_deinit,\
	.capture_setCallback = &ns##_capture_setCallback, \
	.capture_setPeriod= &ns##_capture_setPeriod, \
	.capture_getTime = &ns##_capture_getTime, \
	.capture_getChannelTime = &ns##_capture_getChannelTime\
};
# define CAPTURE_INIT_DEV(ns) .gen.ops = &ns##_capture_ops,

# define CAPTURE_INIT(ns, index) static struct capture *ns##_capture_init(uint32_t index)
# define CAPTURE_DEINIT(ns, c) static int32_t ns##_capture_deinit(struct capture *c)

# define CAPTURE_SET_CALLBACK(ns, c, callback, data) static int32_t ns##_capture_setCallback(struct capture *c, bool (*callback)(struct capture *capture, uint32_t index, uint64_t time, void *data), void *data)

# define CAPTURE_SET_PERIOD(ns, c, us) static int32_t ns##_capture_setPeriod(struct capture *c, uint64_t us)
# define CAPTURE_GET_TIME(ns, c) static uint64_t ns##_capture_getTime(struct capture *c)
# define CAPTURE_GET_CHANNEL_TIME(ns, c) static uint64_t ns##_capture_getChannelTime(struct capture *c)

#endif
#endif
