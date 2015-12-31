#ifndef CAPTURE_H_
#define CAPTURE_H_
#include <stdint.h>
#include <stdbool.h>
struct capture;
#ifdef CONFIG_CAPTURE_MULTI
struct capture_ops {
	struct capture *(*capture_init)(uint32_t index, void *settings);
	int32_t (*capture_deinit)(struct capture *capture);

	int32_t (*capture_setCallback)(struct capture *capture, bool (*callback)(struct capture *capture, uint32_t index, uint64_t time, void *data), void *data);

	int32_t (*capture_setPeriod)(struct capture *capture, uint64_t us);
	uint64_t (*capture_getTime)(struct capture *capture);
	uint64_t (*capture_getChannelTime)(struct capture *capture);
};
#endif
struct capture_generic  {
	bool init;
#ifdef CONFIG_CAPTURE_MULTI
	const struct capture_ops *ops;
#endif
};
extern struct capture **captures;
#ifndef CONFIG_CAPTURE_MULTI
struct capture *capture_init(uint32_t index, void *settings);
int32_t capture_deinit(struct capture *capture);

int32_t capture_setCallback(struct capture *capture, bool (*callback)(struct capture *capture, uint32_t index, uint64_t time, void *data), void *data);

int32_t capture_setPeriod(struct capture *capture, uint64_t us);
uint64_t capture_getTime(struct capture *capture);
uint64_t capture_getChannelTime(struct capture *capture);
#else
inline struct capture *capture_init(uint32_t index, void *settings) {
	struct capture_generic *p = (struct capture_generic *) captures[index];
	return p->ops->capture_init(index, settings);
}
inline int32_t capture_deinit(struct capture *capture) {
	struct capture_generic *p = (struct capture_generic *) capture;
	return p->ops->capture_deinit(capture);
}

inline int32_t capture_setCallback(struct capture *capture, bool (*callback)(struct capture *capture, uint32_t index, uint64_t time, void *data), void *data) {
	struct capture_generic *p = (struct capture_generic *) capture;
	return p->ops->capture_setCallback(capture, callback, data);
}

inline int32_t capture_setPeriod(struct capture *capture, uint64_t us) {
	struct capture_generic *p = (struct capture_generic *) capture;
	return p->ops->capture_setPeriod(capture, us);
}

inline uint64_t capture_getTime(struct capture *capture) {
	struct capture_generic *p = (struct capture_generic *) capture;
	return p->ops->capture_getTime(capture);

}
inline uint64_t capture_getChannelTime(struct capture *capture) {
	struct capture_generic *p = (struct capture_generic *) capture;
	return p->ops->capture_getChannelTime(capture);
}
#endif
#endif
