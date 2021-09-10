/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2018
 */
#include <can.h>
#define CAN_PRV
#include <can_prv.h>

int32_t can_genericInit(struct can *t) {
	struct can_generic *can = (struct can_generic *) t;
	if (hal_isInit(can)) {
		return CAN_ALREDY_INITED;
	}
#ifdef CONFIG_CAN_THREAD_SAFE
	{
		int32_t ret = hal_init(t);
		if (ret < 0) {
			goto can_genericInit_error0;
		}
	}
#endif
	can->init = true;
	return 0;
can_genericInit_error0:
	return -1;
}

#ifdef CONFIG_CAN_MULTI
struct can *can_init(uint32_t index, uint32_t bitrate, struct gpio_pin *pin, bool pinHigh, bool (*callback)(struct can *can, can_error_t error, can_errorData_t data, void *userData), void *data);
int32_t can_deinit(struct can *can);
int32_t can_setCallback(struct can *can, int32_t filterID, bool callback(struct can *can, struct can_msg *msg, void *data), void *data);
int32_t can_registerFilter(struct can *can, struct can_filter *filter);
int32_t can_deregisterFilter(struct can *can, int32_t filterID);
int32_t can_send(struct can *can, struct can_msg *msg, TickType_t waittime);
int32_t can_recv(struct can *can, int32_t filterID, struct can_msg *msg, TickType_t waittime);
int32_t can_sendISR(struct can *can, struct can_msg *msg);
int32_t can_recvISR(struct can *can, int32_t filterID, struct can_msg *msg);
int32_t can_up(struct can *can);
int32_t can_down(struct can *can);
#endif
