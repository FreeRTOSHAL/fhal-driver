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
#ifndef MAILBOX_H_
#define MAILBOX_H_
#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <semphr.h>
#include <hal.h>
/**
 * \defgroup mailbox Mailbox
 * \ingroup HAL
 * \code
 * #include <mailbox.h>
 * \endcode
 * 
 * This is a Maibox Implementiion like the linux mailbox subystem
 * 
 * \{
 */
/**
 * Handle of a Instants of Mailbox
 */
struct mailbox;
#ifdef CONFIG_MAILBOX_MULTI
/**
 * Function of a driver in Multi Driver implementation mode 
 */
struct mailbox_ops {
	struct mailbox *(*mailbox_init)(uint32_t index);
	int32_t (*mailbox_deinit)(struct mailbox *mailbox);
	int32_t (*mailbox_send)(struct mailbox *mailbox, uint32_t data, TickType_t waittime);
	int32_t (*mailbox_recv)(struct mailbox *mailbox, uint32_t *data, TickType_t waittime);
	int32_t (*mailbox_sendISR)(struct mailbox *mailbox, uint32_t data);
	int32_t (*mailbox_recvISR)(struct mailbox *mailbox, uint32_t *data);
};
#endif
/**
 * All driver shall implement this Struct
 * 
 * For Example: 
 * \code{.c}
 * struct mailbox {
 * 	struct mailbox_generic gen;
 * }
 * \endcode
 */
struct mailbox_generic {
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
#ifdef CONFIG_MAILBOX_THREAD_SAVE
	/**
	 * Mutex
	 */
	SemaphoreHandle_t lock;	
#endif
#ifdef CONFIG_MAILBOX_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct mailbox_ops *ops;
#endif
};
#ifndef CONFIG_MAILBOX_MULTI
/**
 * Init Function
 * \param index in mailbox Array
 * \return Example Instants or NULL
 */
struct mailbox *mailbox_init(uint32_t index);
/**
 * Deinit Driver Instants 
 * \param mailbox Instant
 * \return 0 on ok -1 on failure
 */
int32_t mailbox_deinit(struct mailbox *mailbox);
/**
 * Send a Messsage to Mailbox
 * \param mailbox Instant
 * \param data Data
 * \param waittime waittime
 * \return 0 on ok -1 on error
 */
int32_t mailbox_send(struct mailbox *mailbox, uint32_t data, TickType_t waittime);
/**
 * recv a Messsage from Mailbox
 * \param mailbox Instant
 * \param data pointer to Data
 * \param waittime waittime
 * \return 0 on ok -1 on error
 */
int32_t mailbox_recv(struct mailbox *mailbox, uint32_t *data, TickType_t waittime);
/**
 * Send a Messsage to Mailbox
 * \param mailbox Instant
 * \param data Data
 * \return 0 on ok -1 on error
 */
int32_t mailbox_sendISR(struct mailbox *mailbox, uint32_t data);
/**
 * recv a Messsage from Mailbox
 * \param mailbox Instant
 * \param data pointer to Data
 * \return 0 on ok -1 on error
 */
int32_t mailbox_recvISR(struct mailbox *mailbox, uint32_t *data);
#else
inline struct mailbox *mailbox_init(uint32_t index) {
	HAL_DEFINE_GLOBAL_ARRAY(mailbox);
	struct mailbox_generic *p = (struct mailbox_generic *) HAL_GET_DEV(mailbox, index);
	if (p == NULL) {
		return NULL;
	}
	return p->ops->mailbox_init(index);
}
inline int32_t mailbox_deinit(struct mailbox *mailbox) {
	struct mailbox_generic *p = (struct mailbox_generic *) mailbox;
	return p->ops->mailbox_deinit(mailbox);
}
inline int32_t mailbox_send(struct mailbox *mailbox, uint32_t data, TickType_t waittime) {
	struct mailbox_generic *p = (struct mailbox_generic *) mailbox;
	return p->ops->mailbox_send(mailbox, data, waittime);
}
inline int32_t mailbox_recv(struct mailbox *mailbox, uint32_t *data, TickType_t waittime) {
	struct mailbox_generic *p = (struct mailbox_generic *) mailbox;
	return p->ops->mailbox_recv(mailbox, data, waittime);
}
inline int32_t mailbox_sendISR(struct mailbox *mailbox, uint32_t data) {
	struct mailbox_generic *p = (struct mailbox_generic *) mailbox;
	return p->ops->mailbox_sendISR(mailbox, data);
}
inline int32_t mailbox_recvISR(struct mailbox *mailbox, uint32_t *data) {
	struct mailbox_generic *p = (struct mailbox_generic *) mailbox;
	return p->ops->mailbox_recvISR(mailbox, data);
}
#endif
#endif
