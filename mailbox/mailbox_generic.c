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
#include <mailbox.h>
#define MAILBOX_PRV
#include <mailbox_prv.h>

int32_t mailbox_genericInit(struct mailbox *t) {
	struct mailbox_generic *mailbox = (struct mailbox_generic *) t;
	if (hal_isInit(mailbox)) {
		return MAILBOX_ALREDY_INITED;
	}
#ifdef CONFIG_MAILBOX_THREAD_SAVE
	{
		int32_t ret = hal_init(mailbox);
		if (ret < 0) {
			goto mailbox_generic_init_error0;
		}
	}
#endif
	mailbox->init = true;
	return 0;
#ifdef CONFIG_MAILBOX_THREAD_SAVE
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
