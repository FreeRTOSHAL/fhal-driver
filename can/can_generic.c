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
#include <can.h>
#define CAN_PRV
#include <can_prv.h>

int32_t can_generic_init(struct can *t) {
	struct can_generic *can = (struct can_generic *) t;
	if (hal_isInit(can)) {
		return CAN_ALREDY_INITED;
	}
	can->init = true;
	return 0;
}
#ifdef CONFIG_CAPTURE_MULTI
struct can *can_init(uint32_t index, uint8_t bitrate, struct gpio_pin *pin, bool pinHigh);
int32_t can_deinit(struct can *can);
int32_t can_setCallback(struct can *can, struct can_filter *filter, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data);
int32_t can_registerFilter(struct can *can, struct can_filter *filter);
int32_t can_deregisterFilter(struct can *can, struct can_filter *filter);
int32_t can_send(struct can *can, struct can_msg *msg, TickType_t waittime);
int32_t can_recv(struct can *can, struct can_filter *filter, struct can_msg *msg, TickType_t waittime);
int32_t can_sendISR(struct can *can, struct can_msg *msg);
int32_t can_recvISR(struct can *can, struct can_filter *filter, struct can_msg *msg);
int32_t can_up(struct can *can);
int32_t can_down(struct can *can);
#endif
