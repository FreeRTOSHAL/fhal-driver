#include <FreeRTOS.h>
#include <net/net.h>
#define NET_PRV
#include <net/net_prv.h>
#include <net/net_dummy.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
struct netbuff {
	size_t size;
	void *payload;
	struct timespec timestamp;
	ssize_t reserved;
};
NET_INIT(dummy, index) {
	int32_t ret;
	struct net *net = NET_GET_DEV(index);
	if  (net == NULL) {
		return NULL;
	}
	ret = net_genericInit(net);
	if (ret < 0) {
		return NULL;
	}
	if (ret == NET_ALREDY_INITED) {
		return net;
	}
	return net;
}
NET_DEINIT(dummy, net) {
	return -1;
}
NET_ALLOC_NETBUFF(dummy, net, size) {
	size_t off;
	struct netbuff *buff = pvPortMalloc(sizeof(struct netbuff));
	if (buff == NULL) {
		goto dummy_net_allocNetbuff_error0;
	}
	buff->payload = pvPortMalloc(size + net->align + 1);
	if (buff->payload == NULL) {
		goto dummy_net_allocNetbuff_error1;
	}
	off = ((uintptr_t) buff->payload) & net->align;
	if (off) {
		/* Auto Alignment */
		buff->reserved = (net->align + 1) - off;
	} else {
		buff->reserved = 0;
	}
	buff->size = size;
	/*buff->timestamp = NET_NO_TIMESTAMP;*/
	return buff;
dummy_net_allocNetbuff_error1:
	vPortFree(buff);
dummy_net_allocNetbuff_error0:
	return NULL;
}
NET_SET_ALIGNMENT(dummy, net, align) {
	net->align = align;
	return 0;
}
NET_RESERVE(dummy, net, buff, size) {
	buff->reserved = size;
	return 0;
}
NET_GET_PAYLOAD(dummy, net, buff) {
	/* Align payload */
	return (uint32_t *) (((uint8_t *) buff->payload) + buff->reserved);
}
NET_GET_SIZE(dummy, net, buff) {
	return buff->size;
}
NET_FREE_BUFF(dummy, net, buff) {
	vPortFree(buff->payload);
	vPortFree(buff);
	return 0;
}
NET_SET_TIMESTAMP(dummy, net, buff, timestamp) {
	buff->timestamp = *timestamp;
	return 0;
}
NET_GET_TIMESTAMP(dummy, net, buff, timestamp) {
	*timestamp = buff->timestamp;
	return 0;
}
NET_RECV(dummy, net, buff) {
	return -1;
}
NET_SEND(dummy, net, buff) {
	return -1;
}
NET_GET_MTU(dummy, net) {
	return 1500;
}
NET_SET_SIZE(dummy, net, buff, size) {
	return 0;	
}
NET_SET_UP(dummy, net) {
	return 0;	
}
NET_SET_DOWN(dummy, net) {
	return 0;	
}
NET_OPS(dummy);
