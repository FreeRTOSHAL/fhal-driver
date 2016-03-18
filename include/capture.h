#ifndef CAPTURE_H_
#define CAPTURE_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <hal.h>
/**
 * \defgroup CAPTURE Capture Subsystem
 * \ingroup HAL
 * \code
 * #include <capture.h>
 * \endcode
 *
 * This is the Capture Subsystem for controlling Capture of a SOC. 
 * 
 * The most Capture Driver need a Timer! See {@link TIMER} Interface
 * \{
 */
/**
 * Private Structure of Capture driver
 */
struct capture;
#ifdef CONFIG_CAPTURE_MULTI
struct capture_ops {
	struct capture *(*capture_init)(uint32_t index);
	int32_t (*capture_deinit)(struct capture *capture);

	int32_t (*capture_setCallback)(struct capture *capture, bool (*callback)(struct capture *capture, uint32_t index, uint64_t time, void *data), void *data);

	int32_t (*capture_setPeriod)(struct capture *capture, uint64_t us);
	uint64_t (*capture_getTime)(struct capture *capture);
	uint64_t (*capture_getChannelTime)(struct capture *capture);
};
#endif
/**
 * Generic Capture Interface  
 */
struct capture_generic  {
	/**
	 * true = is init
	 * false = is not init
	 */
	bool init;
#ifdef CONFIG_CAPTURE_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct capture_ops *ops;
#endif
};
#ifndef CONFIG_CAPTURE_MULTI
/**
 * Init capture instances
 * \param index Index of PWM
 * \return Capture Instance NULL on Error
 */
struct capture *capture_init(uint32_t index);
/**
 * Deinit Capture
 * \param capture Capture instance
 * \return -1 on error 0 on ok
 */
int32_t capture_deinit(struct capture *capture);
/**
 * Set Capture Callback
 * \param capture Capture instance
 * \param callback Callback
 * \param data Data passed to Callback
 * \return -1 on error 0 on ok
 */
int32_t capture_setCallback(struct capture *capture, bool (*callback)(struct capture *capture, uint32_t index, uint64_t time, void *data), void *data);
/**
 * Set sampling period 
 * \param capture Capture instance
 * \param us Time
 * \return -1 on error 0 on ok
 */
int32_t capture_setPeriod(struct capture *capture, uint64_t us);
/**
 * Get Time see {@link timer_getTime}
 * \param capture Capture instance
 * \return Get Time 
 */
uint64_t capture_getTime(struct capture *capture);
/**
 * Get Channel Time
 * \param capture Capture instance
 * \return Get Time
 */
uint64_t capture_getChannelTime(struct capture *capture);
#else
inline struct capture *capture_init(uint32_t index) {
	HAL_DEFINE_GLOBAL_ARRAY(capture);
	struct capture_generic *p = (struct capture_generic *) HAL_GET_DEV(capture, index);
	return p->ops->capture_init(index);
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
/**\}*/
#endif
