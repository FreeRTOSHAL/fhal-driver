#ifndef NET_DUMMY_
#define NET_DUMMY_
#include <net/net.h>
/**\cond INTERNAL*/
#define NET_PRV
#include <net/net_prv.h>
struct net {
	struct net_generic gen;
	size_t align;
};
#ifdef CONFIG_NET_MULTI
extern const struct net_ops dummy_net_ops;
#endif
/**\endcond*/
/**
 * Definded a new Dummy Net Device
 * \param id Unique identifier
 */
#define NET_DUMMY_ADDDEV(id) \
	struct net dummy_##id = { \
		NET_INIT_DEV(dummy) \
		HAL_NAME("Net Dummy " #id) \
	}; \
	NET_ADDDEV(dummy, dummy_##id)
HAL_DEFINE_GLOBAL_ARRAY(net);
/**
 * Get Device ID
 * \param id Unique identifier
 * \return index id
 */
#define NET_DUMMY_ID(id) HAL_GET_ID(net, dummy, dummy_##id)
#endif
