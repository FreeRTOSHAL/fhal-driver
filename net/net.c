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
#include <net/net.h>
#define NET_PRV
#include <net/net_prv.h>

int32_t net_genericInit(struct net *t) {
	struct net_generic *net = (struct net_generic *) t;
	if (hal_isInit(net)) {
		return NET_ALREDY_INITED;
	}
#ifdef CONFIG_NET_THREAD_SAVE
	{
		int32_t ret = hal_init(net);
		if (ret < 0) {
			goto net_generic_init_error0;
		}
	}
#endif
	net->init = true;
	return 0;
#ifdef CONFIG_NET_THREAD_SAVE
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
