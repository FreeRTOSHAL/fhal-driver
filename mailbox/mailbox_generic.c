/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <mailbox.h>
#define MAILBOX_PRV
#include <mailbox_prv.h>

int32_t mailbox_genericInit(struct mailbox *t) {
	struct mailbox_generic *mailbox = (struct mailbox_generic *) t;
	if (hal_isInit(mailbox)) {
		return MAILBOX_ALREDY_INITED;
	}
#ifdef CONFIG_MAILBOX_THREAD_SAFE
	{
		int32_t ret = hal_init(mailbox);
		if (ret < 0) {
			goto mailbox_generic_init_error0;
		}
	}
#endif
	mailbox->init = true;
	return 0;
#ifdef CONFIG_MAILBOX_THREAD_SAFE
mailbox_generic_init_error0:
	return -1;
#endif

}
#ifdef CONFIG_MAILBOX_MULTI
struct mailbox *mailbox_init(uint32_t index);
int32_t mailbox_deinit(struct mailbox *mailbox);
int32_t mailbox_send(struct mailbox *mailbox, uint32_t data, TickType_t waittime);
int32_t mailbox_recv(struct mailbox *mailbox, uint32_t *data, TickType_t waittime);
int32_t mailbox_sendISR(struct mailbox *mailbox, uint32_t data);
int32_t mailbox_recvISR(struct mailbox *mailbox, uint32_t *data);
#endif
