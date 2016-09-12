#ifndef RPROC_MAILBOX_INIT
#define RPROC_MAILBOX_INIT
#include <remoteproc.h>
enum rp_mbox_messages {
	RP_MBOX_READY		= 0xFFFFFF00,
	RP_MBOX_PENDING_MSG	= 0xFFFFFF01,
	RP_MBOX_CRASH		= 0xFFFFFF02,
	RP_MBOX_ECHO_REQUEST	= 0xFFFFFF03,
	RP_MBOX_ECHO_REPLY	= 0xFFFFFF04,
	RP_MBOX_ABORT_REQUEST	= 0xFFFFFF05,
};
extern const struct rproc_ops rprocMailbox_ops;
struct rprocMailbox_opt {
	struct mailbox *rxmbox;
	struct mailbox *txmbox;
};
#endif
