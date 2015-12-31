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
#ifndef CONFIG_CAPTURE_MULTI
# define CAPTURE_OPS(ns)
# define CAPTURE_INIT_DEV(ns) 

# define CAPTURE_INIT(ns, index, settings) struct capture *capture_init(uint32_t index, void *settings)
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

# define CAPTURE_INIT(ns, index, settings) static struct capture *ns##_capture_init(uint32_t index, void *settings)
# define CAPTURE_DEINIT(ns, c) static int32_t ns##_capture_deinit(struct capture *c)

# define CAPTURE_SET_CALLBACK(ns, c, callback, data) static int32_t ns##_capture_setCallback(struct capture *c, bool (*callback)(struct capture *capture, uint32_t index, uint64_t time, void *data), void *data)

# define CAPTURE_SET_PERIOD(ns, c, us) static int32_t ns##_capture_setPeriod(struct capture *c, uint64_t us)
# define CAPTURE_GET_TIME(ns, c) static uint64_t ns##_capture_getTime(struct capture *c)
# define CAPTURE_GET_CHANNEL_TIME(ns, c) static uint64_t ns##_capture_getChannelTime(struct capture *c)

#endif
#endif
