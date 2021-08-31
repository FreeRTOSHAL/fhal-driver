#ifndef NET_H_
#define NET_H_
#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <semphr.h>
#include <hal.h>
#include <rtc.h>

#define NET_NO_TIMESTAMP UINT64_MAX

struct net;
struct netbuff;
#ifdef CONFIG_NET_MULTI
struct net_ops {
	struct net *(*net_init)(uint32_t index);
	int32_t (*net_deinit)(struct net *net);
	struct netbuff *(*net_allocNetbuff)(struct net *net, size_t size);
	int32_t (*net_reserve)(struct net *net, struct netbuff *buff, size_t size);
	int32_t (*net_setAlignment)(struct net *net, size_t size);
	void *(*net_getPayload)(struct net *net, struct netbuff *buff);
	int32_t (*net_setSize)(struct net *net, struct netbuff *buff, size_t size);
	size_t (*net_getSize)(struct net *net, struct netbuff *buff);
	int32_t (*net_freeNetbuff)(struct net *net, struct netbuff *buff);
	int32_t (*net_setTimestamp)(struct net *net, struct netbuff *buff, struct timespec *timestamp);
	int32_t (*net_getTimestamp)(struct net *net, struct netbuff *buff, struct timespec *timestamp);
	int32_t (*net_recv)(struct net *net, struct netbuff *buff);
	int32_t (*net_send)(struct net *net, struct netbuff *buff);
	int32_t (*net_getMTU)(struct net *net);
	int32_t (*net_setUp)(struct net *net);
	int32_t (*net_setDown)(struct net *net);
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
#ifdef CONFIG_NET_THREAD_SAFE
	/**
	 * Mutex
	 */
	OS_DEFINE_MUTEX_RECURSIVE(lock);
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
#else
inline struct net *net_init(uint32_t index) {
	HAL_DEFINE_GLOBAL_ARRAY(net);
	struct net_generic *a = (struct net_generic *) HAL_GET_DEV(net, index);
	if (a == NULL) {
		return NULL;
	}
	return a->ops->net_init(index);
}
inline int32_t net_deinit(struct net *net) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_deinit(net);
}
inline struct netbuff *net_allocNetbuff(struct net *net, size_t size) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_allocNetbuff(net, size);
}
inline int32_t net_reserve(struct net *net, struct netbuff *buff, size_t size) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_reserve(net, buff, size);
}
inline int32_t net_setAlignment(struct net *net, size_t size) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_setAlignment(net, size);
}
inline void *net_getPayload(struct net *net, struct netbuff *buff) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_getPayload(net, buff);
}
inline int32_t net_setSize(struct net *net, struct netbuff *buff, size_t size) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_setSize(net, buff, size);
}
inline size_t net_getSize(struct net *net, struct netbuff *buff) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_getSize(net, buff);
}
inline int32_t net_freeNetbuff(struct net *net, struct netbuff *buff) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_freeNetbuff(net, buff);
}
inline int32_t net_setTimestamp(struct net *net, struct netbuff *buff, struct timespec *timestamp) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_setTimestamp(net, buff, timestamp);
}
inline int32_t net_getTimestamp(struct net *net, struct netbuff *buff, struct timespec *timestamp) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_getTimestamp(net, buff, timestamp);
}
inline int32_t net_recv(struct net *net, struct netbuff *buff) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_recv(net, buff);
}
inline int32_t net_send(struct net *net, struct netbuff *buff) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_send(net, buff);
}
inline int32_t net_getMTU(struct net *net) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_getMTU(net);
}
inline int32_t net_setUp(struct net *net) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_setUp(net);
}
inline int32_t net_setDown(struct net *net) {
	struct net_generic *n = (struct net_generic *) net;
	return n->ops->net_setDown(net);
}
#endif
#endif
