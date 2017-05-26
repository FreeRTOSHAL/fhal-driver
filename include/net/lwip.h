#ifndef LWIP_H_
#define LWIP_H_
/**\cond INTERNAL*/
#define NET_PRV
#include <net/net_prv.h>
#include <lwip/netif.h>
struct net {
	struct net_generic gen;
	const char *name;
	uint32_t id;
	struct netif netif;
};
#ifdef CONFIG_NET_MULTI
extern const struct net_ops lwip_net_ops;
#endif
/**\endcond*/
#define NET_LWIP_ADDDEV(i, n) \
	struct net lwip_##i = { \
		NET_INIT_DEV(lwip) \
		HAL_NAME("LWIP Eth Device " #i) \
		.name = n, \
		.id = i, \
	}; \
	NET_ADDDEV(lwip, lwip_##i)
/**
 * Get Device ID
 * \param id Unique identifier
 * \return index id
 */
#define NET_LWIP_ID(id) HAL_GET_ID(net, lwip, lwip_##id)
#endif
