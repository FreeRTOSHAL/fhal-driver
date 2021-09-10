/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef NET_PRV_H_
#define NET_PRV_H_
#ifndef NET_PRV
# error "Never include this file out of a NET driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#include <net/net.h>
/**
 * Error Code Returned if Already inited
 */
#define NET_ALREDY_INITED 1
/**
 * Generic Init Function
 * 
 * Check Driver Already inited and init mutex if active
 * \param net Instants of Driver
 * \return NET_ALREDY_INITED on alredy init 0 on not init < 0 error
 */
int32_t net_genericInit(struct net *net);
#ifdef CONFIG_NET_THREAD_SAFE
# define net_lock(u, w, e) HAL_LOCK(u, w, e)
# define net_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define net_lock(u, w, e)
# define net_unlock(u, e)
#endif
#define NET_ADDDEV(ns, p) HAL_ADDDEV(net, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(net);
#define NET_GET_DEV(index) HAL_GET_DEV(net, index)
#ifndef CONFIG_NET_MULTI
# define NET_OPS(ns)
# define NET_INIT_DEV(ns) 

# define NET_INIT(ns, index) struct net *net_init(uint32_t index)
# define NET_DEINIT(ns, p) int32_t net_deinit(struct net *p)
# define NET_ALLOC_NETBUFF(ns, p, size) struct netbuff *net_allocNetbuff(struct net *p, size_t size)
# define NET_RESERVE(ns, p, buff, size) int32_t net_reserve(struct net *p, struct netbuff *buff, size_t size)
# define NET_SET_ALIGNMENT(ns, p, align) int32_t net_setAlignment(struct net *p, size_t align)
# define NET_GET_PAYLOAD(ns, p, buff) void * net_getPayload(struct net *p, struct netbuff *buff)
# define NET_SET_SIZE(ns, p, buff, size) int32_t net_setSize(struct net *p, struct netbuff *buff, size_t size)
# define NET_GET_SIZE(ns, p, buff) size_t net_getSize(struct net *p, struct netbuff *buff)
# define NET_FREE_BUFF(ns, p, buff) int32_t net_freeNetbuff(struct net *p, struct netbuff *buff)
# define NET_SET_TIMESTAMP(ns, p, buff, timestamp) int32_t net_setTimestamp(struct net *p, struct netbuff *buff, struct timespec *timestamp)
# define NET_GET_TIMESTAMP(ns, p, buff, timestamp) int32_t net_getTimestamp(struct net *p, struct netbuff *buff, struct timespec *timestamp)
# define NET_RECV(ns, p, buff) int32_t net_recv(struct net *p, struct netbuff *buff)
# define NET_SEND(ns, p, buff) int32_t net_send(struct net *p, struct netbuff *buff)
# define NET_GET_MTU(ns, p) int32_t net_getMTU(struct net *p)
# define NET_SET_UP(ns, p) int32_t net_setUp(struct net *p)
# define NET_SET_DOWN(ns, p) int32_t net_setDown(struct net *p)
#else
# define NET_OPS(ns) const struct net_ops ns##_net_ops = { \
	.net_init = &ns##_net_init,\
	.net_deinit = &ns##_net_deinit,\
	.net_init = &ns##_net_init, \
	.net_deinit = &ns##_net_deinit, \
	.net_allocNetbuff = &ns##_net_allocNetbuff, \
	.net_reserve = &ns##_net_reserve, \
	.net_setAlignment = &ns##_net_setAlignment, \
	.net_getPayload = &ns##_net_getPayload, \
	.net_setSize = &ns##_net_setSize, \
	.net_getSize = &ns##_net_getSize, \
	.net_freeNetbuff = &ns##_net_freeNetbuff, \
	.net_setTimestamp = &ns##_net_setTimestamp, \
	.net_getTimestamp = &ns##_net_getTimestamp, \
	.net_recv = &ns##_net_recv, \
	.net_send = &ns##_net_send, \
	.net_getMTU = &ns##_net_getMTU, \
	.net_setUp = &ns##_net_setUp, \
	.net_setDown = &ns##_net_setDown, \
}
# define NET_INIT_DEV(ns) .gen.ops = &ns##_net_ops,

# define NET_INIT(ns, index) static struct net *ns##_net_init(uint32_t index)
# define NET_DEINIT(ns, p) static int32_t ns##_net_deinit(struct net *p)
# define NET_ALLOC_NETBUFF(ns, p, size) static struct netbuff *ns##_net_allocNetbuff(struct net *p, size_t size)
# define NET_RESERVE(ns, p, buff, size) static int32_t ns##_net_reserve(struct net *p, struct netbuff *buff, size_t size)
# define NET_SET_ALIGNMENT(ns, p, align) static int32_t ns##_net_setAlignment(struct net *p, size_t align)
# define NET_GET_PAYLOAD(ns, p, buff) static void * ns##_net_getPayload(struct net *p, struct netbuff *buff)
# define NET_SET_SIZE(ns, p, buff, size) static int32_t ns##_net_setSize(struct net *p, struct netbuff *buff, size_t size)
# define NET_GET_SIZE(ns, p, buff) static size_t ns##_net_getSize(struct net *p, struct netbuff *buff)
# define NET_FREE_BUFF(ns, p, buff) static int32_t ns##_net_freeNetbuff(struct net *p, struct netbuff *buff)
# define NET_SET_TIMESTAMP(ns, p, buff, timestamp) static int32_t ns##_net_setTimestamp(struct net *p, struct netbuff *buff, struct timespec *timestamp)
# define NET_GET_TIMESTAMP(ns, p, buff, timestamp) static int32_t ns##_net_getTimestamp(struct net *p, struct netbuff *buff, struct timespec *timestamp)
# define NET_RECV(ns, p, buff) static int32_t ns##_net_recv(struct net *p, struct netbuff *buff)
# define NET_SEND(ns, p, buff) static int32_t ns##_net_send(struct net *p, struct netbuff *buff)
# define NET_GET_MTU(ns, p) static int32_t ns##_net_getMTU(struct net *p)
# define NET_SET_UP(ns, p) static int32_t ns##_net_setUp(struct net *p)
# define NET_SET_DOWN(ns, p) static int32_t ns##_net_setDown(struct net *p)
#endif
/**\}*/
#endif
