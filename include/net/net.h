#ifndef NET_H_
#define NET_H_
#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <semphr.h>
#include <hal.h>

#define NET_NO_TIMESTAMP UINT64_MAX

struct net;
struct netbuff;
#ifdef CONFIG_NET_MULTI
struct net_ops {
};
#endif
struct net_generic {
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
#ifdef CONFIG_NET_THREAD_SAVE
	/**
	 * Mutex
	 */
	SemaphoreHandle_t lock;	
#endif
#ifdef CONFIG_NET_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct net_ops *ops;
#endif
	struct net *parent;
	struct net *child;
	struct mac *mac;
};
inline int32_t net_setParent(struct net *net, struct net *parent) {
	struct net_generic *gen = (struct net_generic *) net;
	gen->parent = parent;
	return 0;
}
inline int32_t net_setChild(struct net *net, struct net *child) {
	struct net_generic *gen = (struct net_generic *) net;
	gen->child = child;
	return 0;
}
inline int32_t net_setMac(struct net *net, struct mac *mac) {
	struct net_generic *gen = (struct net_generic *) net;
	gen->mac = mac;
	return 0;
}
inline struct net *net_getParent(struct net *net) {
	struct net_generic *gen = (struct net_generic *) net;
	return gen->parent;
}
inline struct net *net_getChild(struct net *net) {
	struct net_generic *gen = (struct net_generic *) net;
	return gen->child;
}
inline struct mac *net_getMac(struct net *net) {
	struct net_generic *gen = (struct net_generic *) net;
	return gen->mac;
}
#ifndef CONFIG_NET_MULTI
struct net *net_init(uint32_t index);
int32_t net_deinit(struct net *net);
struct netbuff *net_allocNetbuff(struct net *net, size_t size);
int32_t net_reserve(struct net *net, struct netbuff *buff, size_t size);
int32_t net_setAlignment(struct net *net, size_t size);
void * net_getPayload(struct net *net, struct netbuff *buff);
size_t net_getSize(struct net *net, struct netbuff *buff);
int32_t net_freeNetbuff(struct net *net, struct netbuff *buff);
int32_t net_setTimestamp(struct net *net, struct netbuff *buff, uint64_t timestamp);
uint64_t net_getTimestamp(struct net *net, struct netbuff *buff);
int32_t net_recv(struct net *net, struct netbuff *buff);
int32_t net_send(struct net *net, struct netbuff *buff);
int32_t net_getMTU(struct net *net);
#else
# error TODO not defined
#endif
#endif
