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
#ifdef CONFIG_NET_THREAD_SAVE
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
# define NET_SET_ALIGNMENT(ns, p, size) int32_t net_setAlignment(struct net *p, size_t align)
# define NET_GET_PAYLOAD(ns, p, buff) void * net_getPayload(struct net *p, struct netbuff *buff)
# define NET_GET_SIZE(ns, p, buff) size_t net_getSize(struct net *p, struct netbuff *buff)
# define NET_FREE_BUFF(ns, p, buff) int32_t net_freeNetbuff(struct net *p, struct netbuff *buff)
# define NET_SET_TIMESTAMP(ns, p, buff, timestamp) int32_t net_setTimestamp(struct net *p, struct netbuff *buff, uint64_t timestamp)
# define NET_GET_TIMESTAMP(ns, p, buff) uint64_t net_getTimestamp(struct net *p, struct netbuff *buff)
# define NET_RECV(ns, p, buff) int32_t net_recv(struct net *p, struct netbuff *buff)
# define NET_SEND(ns, p, buff) int32_t net_send(struct net *net, struct netbuff *buff)
# define NET_GET_MTU(ns, p) int32_t net_getMTU(struct net *p)
#else
# error TODO not defined
# define NET_OPS(ns) static const struct net_ops ns##_net_ops = { \
	.net_init = &ns##_net_init,\
	.net_deinit = &ns##_net_deinit,\
	.net_funcname = &ns##_net_funcname, \
}
# define NET_INIT_DEV(ns) .gen.ops = &ns##_net_ops,

# define NET_INIT(ns, index) static struct net *ns##_net_init(uint32_t index)
# define NET_DEINIT(ns, p) static int32_t ns##_net_deinit(struct net *p)
# define NET_FUNCNAME(ns, p, param) static int32_t ns##_net_funcname(struct net *p, uint32_t param)
#endif
/**\}*/
#endif
