/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <net/net.h>
#define NET_PRV
#include <net/net_prv.h>

int32_t net_genericInit(struct net *t) {
	struct net_generic *net = (struct net_generic *) t;
	if (hal_isInit(net)) {
		return NET_ALREDY_INITED;
	}
#ifdef CONFIG_NET_THREAD_SAFE
	{
		int32_t ret = hal_init(net);
		if (ret < 0) {
			goto net_generic_init_error0;
		}
	}
#endif
	net->init = true;
	return 0;
#ifdef CONFIG_NET_THREAD_SAFE
net_generic_init_error0:
	return -1;
#endif
}
int32_t net_setParent(struct net *net, struct net *parent);
int32_t net_setChild(struct net *net, struct net *child);
int32_t net_setMac(struct net *net, struct mac *mac);
struct net *net_getParent(struct net *net);
struct net *net_getChild(struct net *net);
struct mac *net_getMac(struct net *net);
#ifdef CONFIG_NET_MULTI
struct net *net_init(uint32_t index);
int32_t net_deinit(struct net *net);
struct netbuff *net_allocNetbuff(struct net *net, size_t size);
int32_t net_reserve(struct net *net, struct netbuff *buff, size_t size);
int32_t net_setAlignment(struct net *net, size_t size);
void * net_getPayload(struct net *net, struct netbuff *buff);
int32_t net_setSize(struct net *net, struct netbuff *buff, size_t size);
size_t net_getSize(struct net *net, struct netbuff *buff);
int32_t net_freeNetbuff(struct net *net, struct netbuff *buff);
int32_t net_setTimestamp(struct net *net, struct netbuff *buff, struct timespec *timestamp);
int32_t net_getTimestamp(struct net *net, struct netbuff *buff, struct timespec *timestamp);
int32_t net_recv(struct net *net, struct netbuff *buff);
int32_t net_send(struct net *net, struct netbuff *buff);
int32_t net_getMTU(struct net *net);
int32_t net_setUp(struct net *net);
int32_t net_setDown(struct net *net);
#endif
