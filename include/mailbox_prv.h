/*
 * Copyright (c) 2016 Andreas Werner <kernel@andy89.org>
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
/**
 * \defgroup mailbox_driver_prv Driver View form Example driver 
 * \ingroup mailbox_driver
 * 
 * This is the Driver View form Example Driver. 
 * 
 * This is a implementation of a driver 
 * \include driver/mailbox/mailbox_dev0.c
 * 
 * Driver with board specific like the MPU9247 has special macros to create new Instances. (for mailbox: MPU9250_ADDDEV())
 * \{
 */
/**
 * Error Code Returned if Already inited
 */
#define MAILBOX_ALREDY_INITED 1
/**
 * Generic Init Function
 * 
 * Check Driver Already inited and init mutex if active
 * \param mailbox Instants of Driver
 * \return MAILBOX_ALREDY_INITED on alredy init 0 on not init < 0 error
 */
int32_t mailbox_genericInit(struct mailbox *mailbox);
#ifdef CONFIG_MAILBOX_THREAD_SAVE
/**
 * Lock Driver
 * if THREAD_SAVE is not defined this instruction has no function
 */
# define mailbox_lock(u, w, e) HAL_LOCK(u, w, e)
/**
 * Unlock Driver
 * if THREAD_SAVE is not defined this instruction has no function
 */
# define mailbox_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define mailbox_lock(u, w, e)
# define mailbox_unlock(u, e)
#endif
/**
 * Add Driver Instance
 * \param ns Driver namespace
 * \param p Pointer to driver instance
 */
#define MAILBOX_ADDDEV(ns, p) HAL_ADDDEV(mailbox, ns, p)
/*
 * Define Global Array for Driver Access
 */
HAL_DEFINE_GLOBAL_ARRAY(mailbox);
/**
 * Simple function for access a dev from driver
 * \param index index
 * \return see HAL_GET_DEV
 */
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
/**
 * Define Operation
 * \param ns Driver Namespace
 */
# define MAILBOX_OPS(ns) static const struct mailbox_ops ns##_mailbox_ops = { \
	.mailbox_init = &ns##_mailbox_init,\
	.mailbox_deinit = &ns##_mailbox_deinit,\
	.mailbox_send = &ns##_mailbox_send,\
	.mailbox_recv = &ns##_mailbox_recv,\
	.mailbox_sendISR = &ns##_mailbox_sendISR,\
	.mailbox_recvISR = &ns##_mailbox_recvISR,\
}
/**
 * Init Driver Struct 
 * \param ns Driver Namespace
 */
# define MAILBOX_INIT_DEV(ns) .gen.ops = &ns##_mailbox_ops,

/**
 * Define mailbox_init() Implementation 
 * \param ns Driver namespace Variablenname
 * \param index Index Variablenname
 */
# define MAILBOX_INIT(ns, index) static struct mailbox *ns##_mailbox_init(uint32_t index)
/**
 * Define mailbox_deinit() Implementation
 * \param ns Driver namespace Variablenname
 * \param p Instance Variablenname
 */
# define MAILBOX_DEINIT(ns, p) static int32_t ns##_mailbox_deinit(struct mailbox *p)
/**
 * Define mailbox_funcname() Implementation
 * \param ns Driver namespace Variablenname
 * \param p Instance Variablenname
 * \param param Param Variablenname
 */
# define MAILBOX_SEND(ns, p, data, w) int32_t ns##_mailbox_send(struct mailbox *p, uint32_t data, TickType_t w)
# define MAILBOX_RECV(ns, p, data, w) int32_t ns##_mailbox_recv(struct mailbox *p, uint32_t *data, TickType_t w)
# define MAILBOX_SEND_ISR(ns, p, data) int32_t ns##_mailbox_sendISR(struct mailbox *p, uint32_t data)
# define MAILBOX_RECV_ISR(ns, p, data) int32_t ns##_mailbox_recvISR(struct mailbox *p, uint32_t *data)
#endif
/**\}*/
#endif
