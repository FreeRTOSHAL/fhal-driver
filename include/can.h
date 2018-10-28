/*
 * Copyright (c) 2018 Andreas Werner <kernel@andy89.org>
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
#ifndef CAN_H_
#define CAN_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal.h>
#include <gpio.h>
/**
 * \defgroup CAN CAN Subsystem
 * \ingroup HAL
 * \code
 * #include <can.h>
 * \endcode
 *
 * This is the CAN Subsystem for controlling CAN of a SOC. 
 * \{
 */
/**
 * Private Structure of CAN driver
 */
struct can;
/**
 * CAN Filter Struct
 *
 * Filter ist true if:
 * <code>
 * recv.id & filter.mask == filter.id & filter.mask
 * </code>
 */
struct can_filter {
	/**
	 * ID
	 */
	uint16_t id;
	/**
	 * Mask
	 */
	uint16_t mask;
};
#ifdef CONFIG_CAN_FD
# define CAN_MAX_LENGTH 64
#else
/**
 * MAX CAN length 8 or 64 if CAN_FD
 */
# define CAN_MAX_LENGTH 8
#endif
/**
 * Frame
 */
struct can_msg {
	/**
	 * ID
	 */
	uint16_t id;
	/**
	 * Request Flag
	 */
	bool req;
	/**
	 * Length
	 */
	uint8_t len;
	/**
	 * Daten
	 */
	uint8_t data[CAN_MAX_LENGTH];
};
#ifdef CONFIG_CAN_MULTI
/**
 * Function of CAN driver in Multi Driver implementation mode 
 */
struct can_ops {
	struct can *(*can_init)(uint32_t index, uint8_t bitrate, struct gpio_pin *pin, bool pinHigh);
	int32_t (*can_deinit)(struct can *can);
	int32_t (*can_setCallback)(struct can *can, uint32_t filterID, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data);
	int32_t (*can_registerFilter)(struct can *can, struct can_filter *filter);
	int32_t (*can_deregisterFilter)(struct can *can, uint32_t filterID);
	int32_t (*can_send)(struct can *can, struct can_msg *msg, TickType_t waittime);
	int32_t (*can_recv)(struct can *can, uint32_t filterID, struct can_msg *msg, TickType_t waittime);
	int32_t (*can_sendISR)(struct can *can, struct can_msg *msg);
	int32_t (*can_recvISR)(struct can *can, uint32_t filterID, struct can_msg *msg);
	int32_t (*can_up)(struct can *can);
	int32_t (*can_down)(struct can *can);
};
#endif
/**
 * Generic CAN Interface  
 */
struct can_generic {
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
#ifdef CONFIG_CAN_THREAD_SAVE
	/**
	 * Mutex
	 */
	OS_DEFINE_MUTEX_RECURSIVE(lock);
#endif
#ifdef CONFIG_CAN_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct can_ops *ops;
#endif
};
#ifndef CONFIG_CAN_MULTI
/**
 * init CAN instance
 * \param index Index of CAN
 * \param bitrate bitrate
 * \param pin enable pin
 * \param pinHigh Enable Pin Driven High or low for enable
 * \return CAN Instance NULL on Error if only get instance on bits == 0 
 */
struct can *can_init(uint32_t index, uint8_t bitrate, struct gpio_pin *pin, bool pinHigh);

/**
 * Deinit CAN
 * \param can CAN instance
 * \return -1 on error 0 on ok
 */
int32_t can_deinit(struct can *can);

/**
 * Set CAN Interrupt Callback for a filter
 * \param can CAN instance
 * \param filterID filterID
 * \param callback Callback
 * \param data Data parsed to Callback
 * \return -1 on error 0 on ok
 */
int32_t can_setCallback(struct can *can, uint32_t filterID, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data);
/**
 * Register CAN Filter
 * \param can CAN instance
 * \param filter Filter
 * \return -1 on error or filterID
 */
int32_t can_registerFilter(struct can *can, struct can_filter *filter);
/**
 * Deregister CAN Filter
 * \param can CAN instance
 * \param filterID FilterID
 * \return -1 on error 0 on ok
 */
int32_t can_deregisterFilter(struct can *can, uint32_t filterID);

/**
 * Send CAN Message
 * \param can CAN instance
 * \param msg CAN Message
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t can_send(struct can *can, struct can_msg *msg, TickType_t waittime);
/**
 * Receive CAN Message
 * \param can CAN instance
 * \param filterID filterID
 * \param msg CAN Message
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t can_recv(struct can *can, uint32_t filterID, struct can_msg *msg, TickType_t waittime);
/**
 * Send CAN Message
 * \param can CAN instance
 * \param msg CAN Message
 * \return -1 on error 0 on ok
 */
int32_t can_sendISR(struct can *can, struct can_msg *msg);
/**
 * Receive CAN Message
 * \param can CAN instance
 * \param filterID filterID
 * \param msg CAN Message
 * \return -1 on error 0 on ok
 */
int32_t can_recvISR(struct can *can, uint32_t filterID, struct can_msg *msg);

/**
 * Start CAN Net
 * \param can CAN instance
 * \return -1 on error 0 on ok
 */
int32_t can_up(struct can *can);

/**
 * Stop CAN Net
 * \param can CAN instance
 * \return -1 on error 0 on ok
 */
int32_t can_down(struct can *can);

#else
inline struct can *can_init(uint32_t index, uint8_t bitrate, struct gpio_pin *pin, bool pinHigh) {
	HAL_DEFINE_GLOBAL_ARRAY(can);
	struct can_generic *a = (struct can_generic *) HAL_GET_DEV(can, index);
	if (a == NULL) {
		return NULL;
	}
	return a->ops->can_init(index, bitrate, pin, pinHigh);
}
inline int32_t can_deinit(struct can *can) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_deinit(can);
}
inline int32_t can_deinit(struct can *can) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_deinit(can);
}
inline int32_t can_setCallback(struct can *can, uint32_t filterID, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_setCallback(can, filter, callback, data);
}
inline int32_t can_registerFilter(struct can *can, struct can_filter *filter) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_registerFilter(can, filter);
}
inline int32_t can_deregisterFilter(struct can *can, uint32_t filterID) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_deregisterFilter(can, filter);
}
inline int32_t can_send(struct can *can, struct can_msg *msg, TickType_t waittime) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_send(can, msg, waittime);
}
inline int32_t can_recv(struct can *can, uint32_t filterID, struct can_msg *msg, TickType_t waittime) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_recv(can, filter, msg, waittime);
}
inline int32_t can_sendISR(struct can *can, struct can_msg *msg) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_sendISR(can, msg);
}
inline int32_t can_recvISR(struct can *can, uint32_t filterID, struct can_msg *msg) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_recvISR(can, filter, msg);
}
inline int32_t can_up(struct can *can) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_up(can);
}
inline int32_t can_down(struct can *can) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_down(can);
}
#endif
/**\}*/
#endif
