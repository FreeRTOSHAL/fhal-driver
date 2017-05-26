#ifndef VLAN_H_
#define VLAN_H_
/**\cond INTERNAL*/
#define NET_PRV
#include <net/net_prv.h>
#include <stdbool.h>
struct vlan {
	uint16_t vlanid;
	struct net *net;
};
struct net_vlan {
	struct net_generic gen;
	struct vlan vlans[CONFIG_VLAN_MAX];
	const char *name;
	bool rx;
	uint16_t vlanID;
	uint16_t prio;
};
#ifdef CONFIG_NET_MULTI
extern const struct net_ops vlan_net_ops;
#endif
int32_t net_VLANConnect(struct net *vlanrx, struct net *vlantx, struct net *a, struct mac *m, struct net *p, struct net *child, uint16_t vlanID);
/**\endcond*/
#define NET_VLAN_ADDDEV(i, n, r, v, p) \
	struct net_vlan vlan_##i = { \
		NET_INIT_DEV(vlan) \
		HAL_NAME("VLAN Device " #i) \
		.name = n, \
		.rx = r, \
		.vlanID = v, \
		.prio = p, \
	}; \
	NET_ADDDEV(vlan, vlan_##i)
/**
 * Get Device ID
 * \param id Unique identifier
 * \return index id
 */
#define NET_VLAN_ID(id) HAL_GET_ID(net, vlan, vlan_##id)
#endif
