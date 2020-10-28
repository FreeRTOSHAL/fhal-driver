/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2018
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
	uint32_t id;
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
	 * Timestamp
	 *
	 * May from a free Running Conunter of the Hardware
	 */
	uint32_t ts;
	/**
	 * ID
	 */
	uint32_t id;
	/**
	 * Request Flag
	 */
	bool req;
	/**
	 * Length
	 *
	 * DLC values ranging from 1001 to 1111 are used to specify the data lengths of 12, 16, 20, 24, 32, 48, and 64 bytes.
	 */
	uint8_t length;
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
	struct can *(*can_init)(uint32_t index, uint32_t bitrate, struct gpio_pin *pin, bool pinHigh);
	int32_t (*can_deinit)(struct can *can);
	int32_t (*can_setCallback)(struct can *can, int32_t filterID, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data);
	int32_t (*can_registerFilter)(struct can *can, struct can_filter *filter);
	int32_t (*can_deregisterFilter)(struct can *can, int32_t filterID);
	int32_t (*can_send)(struct can *can, struct can_msg *msg, TickType_t waittime);
	int32_t (*can_recv)(struct can *can, int32_t filterID, struct can_msg *msg, TickType_t waittime);
	int32_t (*can_sendISR)(struct can *can, struct can_msg *msg);
	int32_t (*can_recvISR)(struct can *can, int32_t filterID, struct can_msg *msg);
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
struct can *can_init(uint32_t index, uint32_t bitrate, struct gpio_pin *pin, bool pinHigh);

/**
 * Deinit CAN
 * \param can CAN instance
 * \return -1 on error 0 on ok
 */
int32_t can_deinit(struct can *can);

/**
 * Set CAN Interrupt Callback for a filter
 *
 * \warning This Callback is may executed in the contex of CAN interrupt ISR!
 * \param can CAN instance
 * \param filterID filterID
 * \param callback Callback
 * \param data Data parsed to Callback
 * \return -1 on error 0 on ok
 */
int32_t can_setCallback(struct can *can, int32_t filterID, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data);
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
int32_t can_deregisterFilter(struct can *can, int32_t filterID);

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
int32_t can_recv(struct can *can, int32_t filterID, struct can_msg *msg, TickType_t waittime);
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
int32_t can_recvISR(struct can *can, int32_t filterID, struct can_msg *msg);

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
inline struct can *can_init(uint32_t index, uint32_t bitrate, struct gpio_pin *pin, bool pinHigh) {
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
inline int32_t can_setCallback(struct can *can, int32_t filterID, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_setCallback(can, filterID, callback, data);
}
inline int32_t can_registerFilter(struct can *can, struct can_filter *filter) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_registerFilter(can, filter);
}
inline int32_t can_deregisterFilter(struct can *can, int32_t filterID) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_deregisterFilter(can, filterID);
}
inline int32_t can_send(struct can *can, struct can_msg *msg, TickType_t waittime) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_send(can, msg, waittime);
}
inline int32_t can_recv(struct can *can, int32_t filterID, struct can_msg *msg, TickType_t waittime) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_recv(can, filterID, msg, waittime);
}
inline int32_t can_sendISR(struct can *can, struct can_msg *msg) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_sendISR(can, msg);
}
inline int32_t can_recvISR(struct can *can, int32_t filterID, struct can_msg *msg) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_recvISR(can, filterID, msg);
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
