#ifndef RPROC_MAILBOX_INIT
#define RPROC_MAILBOX_INIT
#include <remoteproc.h>
/**
 * Spectale Message for Mailbox
 * This Messages are the same for TI OMAP Prosessor
 */
enum rp_mbox_messages {
	/**
	 * informs the Core that we're up and running. this is
	 * part of the init sequence sent that the core expects to see immediately
	 * after it is booted.
	 */
	RP_MBOX_READY		= 0xFFFFFF00,
	/**
	 * informs the receiver that there is an inbound
	 * message waiting in its own receive-side vring. please note that currently
	 * this message is optional: alternatively, one can explicitly send the index
	 * of the triggered virtqueue itself. the preferred approach will be decided
	 * as we progress and experiment with those two different approaches.
	 */
	RP_MBOX_PENDING_MSG	= 0xFFFFFF01,
	/**
	 * This message is sent if Core crashes
	 */
	RP_MBOX_CRASH		= 0xFFFFFF02,
	/**
	 * a mailbox-level "ping" message.
	 */
	RP_MBOX_ECHO_REQUEST	= 0xFFFFFF03,
	/**
	 * a mailbox-level reply to a "ping"
	 */
	RP_MBOX_ECHO_REPLY	= 0xFFFFFF04,
	/**
	 * a "please crash" request, used for testing the
	 * recovery mechanism (to some extent).
	 */
	RP_MBOX_ABORT_REQUEST	= 0xFFFFFF05,
	/**
	 * auto suspend request for the remote processor
	 */
	RP_MBOX_SUSPEND_AUTO	= 0xFFFFFF10,
	/**
	 * system suspend request for the remote processor
	 */
	RP_MBOX_SUSPEND_SYSTEM	= 0xFFFFFF11,
	/**
	 * successful response from remote processor for a
	 * suspend request
	 */
	RP_MBOX_SUSPEND_ACK	= 0xFFFFFF12,
	/**
	 * a cancel suspend response from a remote processor
	 * on a suspend request
	 *
	 * Introduce new message definitions if any here.
	 */
	RP_MBOX_SUSPEND_CANCEL	= 0xFFFFFF13,
	/**
	 * Indicates end of known/defined messages from remote core
	 * This should be the last definition.
	 */
	RP_MBOX_END_MSG		= 0xFFFFFF14,
};
extern const struct rproc_ops rprocMailbox_ops;
struct rprocMailbox_opt {
	struct mailbox *rxmbox;
	struct mailbox *txmbox;
};
#endif
