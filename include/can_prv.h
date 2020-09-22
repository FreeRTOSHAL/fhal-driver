/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2018
 */
#ifndef CAN_PRV_
#define CAN_PRV_
#ifndef CAN_PRV
# error "Never include this file out of a CAN driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#define CAN_ALREDY_INITED 1
int32_t can_genericInit(struct can *can);
#ifdef CONFIG_CAN_THREAD_SAVE
# define can_lock(u, w, e) HAL_LOCK(u, w, e)
# define can_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define can_lock(u, w, e)
# define can_unlock(u, e)
#endif
#define CAN_ADDDEV(ns, p) HAL_ADDDEV(can, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(can);
#define CAN_GET_DEV(index) HAL_GET_DEV(can, index)
#ifndef CONFIG_CAN_MULTI
# define CAN_OPS(ns)
# define CAN_INIT_DEV(ns) 
# define CAN_INIT(ns, index, bitrate, pin, pinHigh) struct can * can_init(uint32_t index, uint8_t bitrate, struct gpio_pin *pin, bool pinHigh)
# define CAN_DEINIT(ns, c) int32_t  can_deinit(struct can *c)
# define CAN_SET_CALLBACK(ns, c, filterID, callback, data) int32_t  can_setCallback(struct can *c, uint32_t filterID, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data)
# define CAN_REGISTER_FILTER(ns, c, filter) int32_t  can_registerFilter(struct can *c, struct can_filter *filter)
# define CAN_DEREGISTER_FILTER(ns, c, filterID) int32_t  can_deregisterFilter(struct can *c, uint32_t filterID)
# define CAN_SEND(ns, c, msg, waittime) int32_t  can_send(struct can *c, struct can_msg *msg, TickType_t waittime)
# define CAN_RECV(ns, c, filterID, msg, waittime) int32_t  can_recv(struct can *c, uint32_t filterID, struct can_msg *msg, TickType_t waittime)
# define CAN_SEND_ISR(ns, c, msg) int32_t  can_sendISR(struct can *c, struct can_msg *msg)
# define CAN_RECV_ISR(ns, c, filterID, msg) int32_t  can_recvISR(struct can *c, uint32_t filterID, struct can_msg *msg)
# define CAN_UP(ns, c) int32_t  can_up(struct can *c)
# define CAN_DOWN(ns, c) int32_t  can_down(struct can *c)
#else
# define CAN_OPS(ns) static const struct can_ops ns##_can_ops = { \
	.can_init = &ns##_can_init,\
	.can_deinit = &ns##_can_deinit,\
	.can_funcname = &ns##_can_funcname, \
	.can_init = &ns##_can_init, \
	.can_deinit = &ns##_can_deinit, \
	.can_setCallback = &ns##_can_setCallback, \
	.can_registerFilter = &ns##_can_registerFilter, \
	.can_deregisterFilter = &ns##_can_deregisterFilter, \
	.can_send = &ns##_can_send, \
	.can_recv = &ns##_can_recv, \
	.can_sendISR = &ns##_can_sendISR, \
	.can_recvISR = &ns##_can_recvISR, \
	.can_up = &ns##_can_up, \
	.can_down = &ns##_can_down, \
}
# define CAN_INIT_DEV(ns) .gen.ops = &ns##_can_ops,
# define CAN_INIT(ns, index, bitrate, pin, pinHigh) struct can * ns##_can_init(uint32_t index, uint8_t bitrate, struct gpio_pin *pin, bool pinHigh)
# define CAN_DEINIT(ns, c) int32_t  ns##_can_deinit(struct can *c)
# define CAN_SET_CALLBACK(ns, c, filterID, callback, data) int32_t  ns##_can_setCallback(struct can *c, uint32_t filterID, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data)
# define CAN_REGISTER_FILTER(ns, c, filter) int32_t  ns##_can_registerFilter(struct can *c, struct can_filter *filter)
# define CAN_DEREGISTER_FILTER(ns, c, filterID) int32_t  ns##_can_deregisterFilter(struct can *c, uint32_t filterID)
# define CAN_SEND(ns, c, msg, waittime) int32_t  ns##_can_send(struct can *c, struct can_msg *msg, TickType_t waittime)
# define CAN_RECV(ns, c, filterID, struct can_msg *msg, TickType_t waittime) int32_t  ns##_can_recv(struct can *c, uint32_t filterID, struct can_msg *msg, TickType_t waittime)
# define CAN_SEND_ISR(ns, c, msg) int32_t  ns##_can_sendISR(struct can *c, struct can_msg *msg)
# define CAN_RECV_ISR(ns, c, filterID, msg) int32_t  ns##_can_recvISR(struct can *c, uint32_t filterID, struct can_msg *msg)
# define CAN_UP(ns, c) int32_t  ns##_can_up(struct can *c)
# define CAN_DOWN(ns, c) int32_t  ns##_can_down(struct can *c)
#endif
/**\}*/
#endif
