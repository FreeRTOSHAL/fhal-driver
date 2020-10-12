/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef MAILBOX_PRV_
#define MAILBOX_PRV_
#ifndef MAILBOX_PRV
# error "Never include this file out of a MAILBOX driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#define MAILBOX_ALREDY_INITED 1
int32_t mailbox_genericInit(struct mailbox *mailbox);
#ifdef CONFIG_MAILBOX_THREAD_SAVE
# define mailbox_lock(u, w, e) HAL_LOCK(u, w, e)
# define mailbox_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define mailbox_lock(u, w, e)
# define mailbox_unlock(u, e)
#endif
#define MAILBOX_ADDDEV(ns, p) HAL_ADDDEV(mailbox, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(mailbox);
#define MAILBOX_GET_DEV(index) HAL_GET_DEV(mailbox, index)
#ifndef CONFIG_MAILBOX_MULTI
# define MAILBOX_OPS(ns)
# define MAILBOX_INIT_DEV(ns) 

# define MAILBOX_INIT(ns, index) struct mailbox *mailbox_init(uint32_t index)
# define MAILBOX_DEINIT(ns, p) int32_t mailbox_deinit(struct mailbox *p)

# define MAILBOX_SEND(ns, p, data, w) int32_t mailbox_send(struct mailbox *p, uint32_t data, TickType_t w)
# define MAILBOX_RECV(ns, p, data, w) int32_t mailbox_recv(struct mailbox *p, uint32_t *data, TickType_t w)
# define MAILBOX_SEND_ISR(ns, p, data) int32_t mailbox_sendISR(struct mailbox *p, uint32_t data)
# define MAILBOX_RECV_ISR(ns, p, data) int32_t mailbox_recvISR(struct mailbox *p, uint32_t *data)
#else
# define MAILBOX_OPS(ns) static const struct mailbox_ops ns##_mailbox_ops = { \
	.mailbox_init = &ns##_mailbox_init,\
	.mailbox_deinit = &ns##_mailbox_deinit,\
	.mailbox_send = &ns##_mailbox_send,\
	.mailbox_recv = &ns##_mailbox_recv,\
	.mailbox_sendISR = &ns##_mailbox_sendISR,\
	.mailbox_recvISR = &ns##_mailbox_recvISR,\
}
# define MAILBOX_INIT_DEV(ns) .gen.ops = &ns##_mailbox_ops,
# define MAILBOX_INIT(ns, index) static struct mailbox *ns##_mailbox_init(uint32_t index)
# define MAILBOX_DEINIT(ns, p) static int32_t ns##_mailbox_deinit(struct mailbox *p)
# define MAILBOX_SEND(ns, p, data, w) int32_t ns##_mailbox_send(struct mailbox *p, uint32_t data, TickType_t w)
# define MAILBOX_RECV(ns, p, data, w) int32_t ns##_mailbox_recv(struct mailbox *p, uint32_t *data, TickType_t w)
# define MAILBOX_SEND_ISR(ns, p, data) int32_t ns##_mailbox_sendISR(struct mailbox *p, uint32_t data)
# define MAILBOX_RECV_ISR(ns, p, data) int32_t ns##_mailbox_recvISR(struct mailbox *p, uint32_t *data)
#endif
#endif
