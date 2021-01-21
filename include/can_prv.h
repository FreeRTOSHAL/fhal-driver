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

/**
 * CAN bit-timing parameters
 *
 * For further information, please read chapter "8 BIT TIMING
 * REQUIREMENTS" of the "Bosch CAN Specification version 2.0".
 *
 * based on Linux CAN API
 */
struct can_bittiming {
	uint32_t bitrate;	/* Bit-rate in bits/second */
	uint32_t sample_point;	/* Sample point in one-tenth of a percent */
	uint32_t tq;		/* Time quanta (TQ) in nanoseconds */
	uint32_t prop_seg;	/* Propagation segment in TQs */
	uint32_t phase_seg1;	/* Phase buffer segment 1 in TQs */
	uint32_t phase_seg2;	/* Phase buffer segment 2 in TQs */
	uint32_t sjw;		/* Synchronisation jump width in TQs */
	uint32_t brp;		/* Bit-rate prescaler */
};
/**
 *  CAN hardware-dependent bit-timing constant
 *
 *  Used for calculating and checking bit-timing parameters
 *
 *  based on Linux CAN API
 */
struct can_bittiming_const {
	uint32_t tseg1_min;	/* Time segment 1 = prop_seg + phase_seg1 */
	uint32_t tseg1_max;
	uint32_t tseg2_min;	/* Time segment 2 = phase_seg2 */
	uint32_t tseg2_max;
	uint32_t sjw_max;		/* Synchronisation jump width */
	uint32_t brp_min;		/* Bit-rate prescaler */
	uint32_t brp_max;
	uint32_t brp_inc;
};
int32_t can_calcBittiming(struct can_bittiming *bt, struct can_bittiming_const const *btc, int64_t clkFreq);

#ifndef CONFIG_CAN_MULTI
# define CAN_OPS(ns)
# define CAN_INIT_DEV(ns) 
# define CAN_INIT(ns, index, bitrate, pin, pinHigh, callback, data) struct can * can_init(uint32_t index, uint32_t bitrate, struct gpio_pin *pin, bool pinHigh, bool (*callback)(struct can *can, can_error_t error, can_errorData_t d, void *userData), void *data)
# define CAN_DEINIT(ns, c) int32_t  can_deinit(struct can *c)
# define CAN_SET_CALLBACK(ns, c, filterID, callback, data) int32_t  can_setCallback(struct can *c, int32_t filterID, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data)
# define CAN_REGISTER_FILTER(ns, c, filter) int32_t  can_registerFilter(struct can *c, struct can_filter *filter)
# define CAN_DEREGISTER_FILTER(ns, c, filterID) int32_t  can_deregisterFilter(struct can *c, int32_t filterID)
# define CAN_SEND(ns, c, msg, waittime) int32_t  can_send(struct can *c, struct can_msg *msg, TickType_t waittime)
# define CAN_RECV(ns, c, filterID, msg, waittime) int32_t  can_recv(struct can *c, int32_t filterID, struct can_msg *msg, TickType_t waittime)
# define CAN_SEND_ISR(ns, c, msg) int32_t  can_sendISR(struct can *c, struct can_msg *msg)
# define CAN_RECV_ISR(ns, c, filterID, msg) int32_t  can_recvISR(struct can *c, int32_t filterID, struct can_msg *msg)
# define CAN_UP(ns, c) int32_t  can_up(struct can *c)
# define CAN_DOWN(ns, c) int32_t  can_down(struct can *c)
#else
# define CAN_OPS(ns) const struct can_ops ns##_can_ops = { \
	.can_init = &ns##_can_init,\
	.can_deinit = &ns##_can_deinit,\
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
# define CAN_INIT(ns, index, bitrate, pin, pinHigh, callback, data) struct can * ns##_can_init(uint32_t index, uint32_t bitrate, struct gpio_pin *pin, bool pinHigh, bool (*callback)(struct can *can, can_error_t error, can_errorData_t d, void *userData), void *data)
# define CAN_DEINIT(ns, c) int32_t  ns##_can_deinit(struct can *c)
# define CAN_SET_CALLBACK(ns, c, filterID, callback, data) int32_t  ns##_can_setCallback(struct can *c, int32_t filterID, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data)
# define CAN_REGISTER_FILTER(ns, c, filter) int32_t  ns##_can_registerFilter(struct can *c, struct can_filter *filter)
# define CAN_DEREGISTER_FILTER(ns, c, filterID) int32_t  ns##_can_deregisterFilter(struct can *c, int32_t filterID)
# define CAN_SEND(ns, c, msg, waittime) int32_t  ns##_can_send(struct can *c, struct can_msg *msg, TickType_t waittime)
# define CAN_RECV(ns, c, filterID, msg, waittime) int32_t  ns##_can_recv(struct can *c, int32_t filterID, struct can_msg *msg, TickType_t waittime)
# define CAN_SEND_ISR(ns, c, msg) int32_t  ns##_can_sendISR(struct can *c, struct can_msg *msg)
# define CAN_RECV_ISR(ns, c, filterID, msg) int32_t  ns##_can_recvISR(struct can *c, int32_t filterID, struct can_msg *msg)
# define CAN_UP(ns, c) int32_t  ns##_can_up(struct can *c)
# define CAN_DOWN(ns, c) int32_t  ns##_can_down(struct can *c)
#endif
#endif
