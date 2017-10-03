#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <remoteproc.h>
#include <remoteproc_mailbox.h>
struct mproc {
	bool init;
	struct rproc *rproc;
	struct mailbox *rxmbox;
	struct mailbox *txmbox;
	OS_DEFINE_TASK(task, 300);
};
static void rprocMailbox_task(void * data) {
	struct mproc *mproc = data;
	uint32_t vqid;
	int32_t ret;
	for(;;) {
		printf("wait for message\n");
		ret = mailbox_recv(mproc->rxmbox, &vqid, portMAX_DELAY);
		CONFIG_ASSERT(ret >= 0);
		switch (vqid) {
			case RP_MBOX_ECHO_REQUEST:
				printf("Recv Echo Request\n");
				/* TODO Timeout config */
				ret = mailbox_send(mproc->txmbox, RP_MBOX_ECHO_REPLY, 1000 / portTICK_PERIOD_MS);
				CONFIG_ASSERT(ret >= 0);
				break;
			case RP_MBOX_ABORT_REQUEST:
				printf("Recv Mbox Abort Request\n");
				/* TODO */
				break;
			default:
				printf("Recv vqid: %lu\n", vqid);
				/* notify rproc subsystem */
				ret = rproc_isr(mproc->rproc, vqid);
				CONFIG_ASSERT(ret >= 0);
				break;
		}
	}
}
static struct mproc mprocDev = {.init = false };
static void *rprocMailbox_init(struct rproc *rproc, void *initData, uint32_t cpuID) {
	BaseType_t ret;
	struct mproc *mproc = &mprocDev; 
	struct rprocMailbox_opt *opt = initData;
	if (mproc->init) {
		return NULL;
	}
	mproc->init = false;
	mproc->rxmbox = opt->rxmbox;
	mproc->txmbox = opt->txmbox;
	mproc->rproc = rproc;
	ret = OS_CREATE_TASK(rprocMailbox_task, "Mainbox task", 300, mproc, CONFIG_RPROC_MAILBOX_PRIO, mproc->task);
	if (ret != pdPASS) {
		return NULL;
	}
	return mproc;
}
static int32_t rprocMailbox_deinit(struct rproc *rproc, void *data) {
	struct mproc *mproc = data;;
	int32_t ret = mailbox_sendISR(mproc->txmbox, RP_MBOX_CRASH);
	if (ret < 0) {
		return ret;
	}
	mproc->init = false;
	return 0;
}
static int32_t rprocMailbox_notify(struct rproc *rproc, void *data, uint32_t virtID) {
	struct mproc *mproc = data;
	/* TODO Timeout config */
	int32_t ret = mailbox_send(mproc->txmbox, virtID, 100 / portTICK_PERIOD_MS);
	return ret;
}

const struct rproc_ops rprocMailbox_ops = {
	.init = rprocMailbox_init,
	.deinit = rprocMailbox_deinit,
	.notify = rprocMailbox_notify,
};
