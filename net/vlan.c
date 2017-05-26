#include <net/vlan.h>
#include <net/net.h>
#define NET_PRV
#include <net/net_prv.h>
#include <net/mac.h>
#include <string.h>

#define PRINTF(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define VLANID 0x8100
#define VLAN_HEADER_POS 12
#define VLAN_HEADER_SIZE 4
#define VLAN_TCI_PRIO(x) (((x) & 0x7) << 6)
#define VLAN_TCI_VLANID(x) (((x) & 0xFFF) << 0)
struct vlan_header {
	uint16_t id;
	uint16_t tci;
} PACKED;

NET_INIT(vlan, index) {
	int32_t ret;
	struct net_vlan *net = (struct net_vlan *) NET_GET_DEV(index);
	int i;
	if  (net == NULL) {
		goto vlan_net_init_error0;
	}
	ret = net_genericInit((struct net *) net);
	if (ret < 0) {
		goto vlan_net_init_error0;
	}
	if (ret == NET_ALREDY_INITED) {
		goto vlan_net_init_exit;
	}
	if (net->rx) {
		for (i = 0; i < CONFIG_VLAN_MAX; i++) {
			net->vlans[i].net = NULL;
		}
	}
vlan_net_init_exit:
	return (struct net *) net;
vlan_net_init_error0:
	return NULL;
}
NET_DEINIT(vlan, n) {
	return 0;
}
static int32_t net_addVlan(struct net *v, struct net *net, uint16_t vlanID) {
	int i;
	struct net_vlan *vlan = (struct net_vlan *) v;
	if (!vlan->rx) {
		return -1;
	}
	for (i = 0; i < CONFIG_VLAN_MAX; i++) {
		if (!vlan->vlans[i].net) {
			break;
		}
	}
	if (i == CONFIG_VLAN_MAX) {
		return -1;
	}
	vlan->vlans[i].vlanid = vlanID;
	vlan->vlans[i].net = net;
	return 0;
}
int32_t net_VLANConnect(struct net *vlanrx, struct net *vlantx, struct net *a, struct mac *m, struct net *p, struct net *child, uint16_t vlanID) {
	/*
	 * ----------
	 * | Parent |
	 * ----------                Used for Netbuff Alloc
	 *     ^ parent / net            --------------
	 *     |  ---------------------->| AllcoChild |
	 * mac v  | child                --------------
	 * ---------- childs[i] ---------
	 * | vlanrx |---------->| child |
	 * ----------           ---------
	 *     ^                    | parent
	 *     |                    |
	 *     |  Parent / child    |
	 * ---------                |
	 * | vlantx |<---------------
	 * ----------
	 */
	struct net *parent = net_getParent(vlanrx);
	struct net *allocChild = net_getChild(vlanrx);
	struct mac *mac = net_getMac(vlanrx);
	int ret;
	if (!mac && m) {
		net_setMac(vlanrx, m);
		mac = m;
		mac_setNet(mac, vlanrx);
	}
	if (!parent && p) {
		net_setParent(vlanrx, p);
		parent = p;
		net_setChild(p, vlanrx);
	}
	if (!allocChild) {
		CONFIG_ASSERT(a != NULL);
		net_setChild(vlanrx, a);
		allocChild = a;
	}
	CONFIG_ASSERT(parent == p);
	CONFIG_ASSERT(mac == m);
	CONFIG_ASSERT(mac || parent);
	CONFIG_ASSERT(allocChild = a);
	ret = net_addVlan(vlanrx, child, vlanID);
	if (ret < 0) {
		goto net_VLANConnect_error0;
	}
	net_setParent(vlantx, vlanrx);
	net_setChild(vlantx, vlanrx);
	net_setParent(child, vlantx);
	return 0;
net_VLANConnect_error0:
	return -1;
}
NET_ALLOC_NETBUFF(vlan, n, size) {
	struct net *child = net_getChild(n);
	return net_allocNetbuff(child, size);
}
NET_RESERVE(vlan, n, buff, size) {
	struct net *child = net_getChild(n);
	return net_reserve(child, buff, size);
}
NET_SET_ALIGNMENT(vlan, n, align) {
	struct net *child = net_getChild(n);
	return net_setAlignment(child, align);
}
NET_GET_PAYLOAD(vlan, n, buff) {
	struct net *child = net_getChild(n);
	return net_getPayload(child, buff);
}
NET_GET_SIZE(vlan, n, buff) {
	struct net *child = net_getChild(n);
	return net_getSize(child, buff);
}
NET_SET_SIZE(vlan, n, buff, size) {
	struct net *child = net_getChild(n);
	return net_setSize(child, buff, size);
}
NET_FREE_BUFF(vlan, n, buff) {
	struct net *child = net_getChild(n);
	return net_freeNetbuff(child, buff);
}
NET_SET_TIMESTAMP(vlan, n, buff, timestamp) {
	struct net *child = net_getChild(n);
	return net_setTimestamp(child, buff, timestamp);
}
NET_GET_TIMESTAMP(vlan, n, buff, timestamp) {
	struct net *child = net_getChild(n);
	return net_getTimestamp(child, buff, timestamp);
}
NET_RECV(vlan, n, buff) {
	struct net_vlan *net = (struct net_vlan *) n;
	uint8_t *payload = net_getPayload(n, buff);
	uint32_t vlanID = 1;
	uint32_t i;
	int32_t ret;
	struct vlan_header *header;
	uint8_t tmpbuff[VLAN_HEADER_POS];
	if (!net->rx) {
		goto vlan_recv_error0;
	}
	/* check is VLAN C-TAG Header */
	header = (struct vlan_header *) (payload + VLAN_HEADER_POS);
	if (be16_to_cpu(header->id) == VLANID) {
		uint16_t tci = be16_to_cpu(header->tci);
		vlanID = tci & 0xFFF;

		/* remove VLAN Tag */
		memcpy(tmpbuff, payload, VLAN_HEADER_POS * sizeof(uint8_t));
		ret = net_reserve(n, buff, -VLAN_HEADER_SIZE);
		if (ret < 0) {
			PRINTF("no space to add vlan Header\n");
			goto vlan_recv_error0;
		}
		payload = net_getPayload(n, buff);
		memcpy(payload, tmpbuff, VLAN_HEADER_POS * sizeof(uint8_t));
	}
	for (i = 0; i < CONFIG_VLAN_MAX; i++) {
		if (net->vlans[i].net != NULL) {
			if (net->vlans[i].vlanid == vlanID) {
				break;
			}
		}
	}
	if (i == CONFIG_VLAN_MAX) {
		PRINTF("vlanid: %lu not supported, drop packed\n", vlanID);
		net_freeNetbuff(n, buff);
		ret = 0;
		goto vlan_recv_exit;
	}
	ret = net_recv(net->vlans[i].net, buff);
vlan_recv_exit:
	return ret;
vlan_recv_error0:
	net_freeNetbuff(n, buff);
	return -1;
}
NET_SEND(vlan, n, buff) {
	struct net_vlan *net = (struct net_vlan *) n;
	struct net *parent = net_getParent(n);
	struct mac *mac = net_getMac(n);
	int32_t ret;
	uint8_t *payload;
	uint8_t *newpayload;
	uint16_t tci;
	struct vlan_header *header;
	CONFIG_ASSERT(net);
	CONFIG_ASSERT(parent || mac);
	/* in rx case send to mac or net parent */
	if (net->rx) {
		if (mac) {
			return mac_send(mac, buff);
		} else if (parent) {
			return net_send(parent, buff);
		} else {
			goto vlan_send_error0;
		}
	}
	/* send packed with VLAN Tag only vlanID > 1 */
	if (net->vlanID > 1) {
		payload = net_getPayload(n, buff);
		ret = net_reserve(n, buff, VLAN_HEADER_SIZE);
		if (ret < 0) {
			PRINTF("no space to add vlan Header\n");
			goto vlan_send_error0;
		}
		newpayload = net_getPayload(n, buff);
		{
			int32_t i;
			for (i = 0; i < VLAN_HEADER_POS; i++) {
				newpayload[i] = payload[i];
			}
		}
		payload = newpayload;
		header = (struct vlan_header *) (payload + VLAN_HEADER_POS);
		tci = VLAN_TCI_PRIO(net->prio) | VLAN_TCI_VLANID(net->vlanID);
		header->id = cpu_to_be16(VLANID);
		header->tci = cpu_to_be16(tci);
	}
	if (mac) {
		ret = mac_send(mac, buff);
	} else if (net) {
		ret = net_send(parent, buff);
	} else {
		goto vlan_send_error0;
	}
	return ret;
vlan_send_error0:
	net_freeNetbuff(n, buff);
	return -1;
}
NET_GET_MTU(vlan, n) {
	struct net *parent = net_getParent(n);
	return net_getMTU(parent);
}
NET_SET_UP(vlan, n) {
	struct net_vlan *net = (struct net_vlan *) n;
	int32_t i; 
	if (net->rx) {
		for (i = 0; i < CONFIG_VLAN_MAX; i++) {
			if (net->vlans[i].net) {
				net_setUp(net->vlans[i].net);
			}
		}
	} else {
		struct net *parent = net_getParent(n);
		return net_setUp(parent);
	}
	return 0;
}
NET_SET_DOWN(vlan, n) {
	struct net_vlan *net = (struct net_vlan *) n;
	int32_t i; 
	if (net->rx) {
		for (i = 0; i < CONFIG_VLAN_MAX; i++) {
			if (net->vlans[i].net) {
				net_setDown(net->vlans[i].net);
			}
		}
	} else {
		struct net *parent = net_getParent(n);
		return net_setDown(parent);
	}
	return 0;
}
NET_OPS(vlan);
