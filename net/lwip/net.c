#include <FreeRTOS.h>
#include <net/net.h>
#define NET_PRV
#include <net/net_prv.h>
#include <net/lwip.h>
#include <net/mac.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/ethip6.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"
#include "netif/ethernet.h"

static err_t low_level_output(struct netif *netif, struct pbuf *p) {
	struct net *net = netif->state;
	return net_send(net, (struct netbuff *) p) == 0 ? ERR_OK: -1;
}

static err_t lwip_netif_init(struct netif *netif) {
	struct net *net = netif->state;
#if LWIP_NETIF_HOSTNAME
	netif->hostname = net->name;
#endif
	MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);
	netif->name[0] = 'E';
	netif->name[1] = 'T';
	netif->num = net->id;
	netif->output = etharp_output;
#if LWIP_IPV6
	netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
	netif->linkoutput = low_level_output;
	netif->hwaddr_len = ETHARP_HWADDR_LEN;
	netif->hwaddr[0] = 0x00;
	netif->hwaddr[1] = 0x34;
	netif->hwaddr[2] = 0x56;
	netif->hwaddr[3] = 0x78;
	netif->hwaddr[4] = 0x9a;
	netif->hwaddr[5] = 0xbc;
	netif->mtu = 1500;
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
#if LWIP_CHECKSUM_CTRL_PER_NETIF
	netif->chksum_flags = NETIF_CHECKSUM_ENABLE_ALL;
#endif /* LWIP_CHECKSUM_CTRL_PER_NETIF*/
#if LWIP_IPV6 && LWIP_IPV6_MLD
	/* TODO multicast */
	/*if (netif->mld_mac_filter != NULL) {
		ip6_addr_t ip6_allnodes_ll;
		ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
		netif->mld_mac_filter(netif, &ip6_allnodes_ll, MLD6_ADD_MAC_FILTER);
	}*/
#endif
	return ERR_OK;
}

NET_INIT(lwip, index) {
	struct netif *ifret;
	int32_t ret;
	struct net *net = NET_GET_DEV(index);
	if  (net == NULL) {
		goto lwip_net_init_error0;
	}
	ret = net_genericInit(net);
	if (ret < 0) {
		goto lwip_net_init_error0;
	}
	if (ret == NET_ALREDY_INITED) {
		goto lwip_net_init_exit;
	}
	memset(&net->netif, 0, sizeof(struct netif));
	ifret = netif_add(&net->netif, NULL, NULL, NULL, net, lwip_netif_init, ethernet_input);
	if (!ifret) {
		goto lwip_net_init_error1;
	}
lwip_net_init_exit:
	return net;
lwip_net_init_error1:
	net->gen.init = false;
lwip_net_init_error0:
	return NULL;
}
NET_DEINIT(lwip, net) {
	return 0;
}
NET_ALLOC_NETBUFF(lwip, net, size) {
	struct netbuff *buff = (struct netbuff *) pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
	if (!buff) {
		LINK_STATS_INC(link.memerr);
		return NULL;
	}
	return buff;
}
NET_SET_ALIGNMENT(lwip, net, align) {
	return 0;
}
NET_RESERVE(lwip, net, b, size) {
	struct pbuf *buff = (struct pbuf *) b;
	return pbuf_header(buff, size) != 0;
}
NET_GET_PAYLOAD(lwip, net, b) {
	struct pbuf *buff = (struct pbuf *) b;
	return buff->payload;
}

NET_GET_SIZE(lwip, net, b) {
	struct pbuf *buff = (struct pbuf *) b;
	return buff->tot_len;
}
NET_SET_SIZE(lwip, net, b, size) {
	struct pbuf *buff = (struct pbuf *) b;
	if (size > buff->tot_len) {
		return -1;
	}
	/* set len to correnct value */
	while (buff != NULL && size > buff->len) {
		buff->tot_len = size;
		size -= buff->len;
		buff = buff->next;
	};
	CONFIG_ASSERT(buff != NULL);
	buff->tot_len = buff->len = size;
	/* free next buffer not needed anymore */
	if (buff->next) {
		pbuf_free(buff->next);
		buff->next = NULL;
	}
	return 0;
}
NET_FREE_BUFF(lwip, net, b) {
	struct pbuf *buff = (struct pbuf *) b;
	pbuf_free(buff);
	return 0;
}
NET_SET_TIMESTAMP(lwip, net, buff, timestamp) {
	return 0;	
}
NET_GET_TIMESTAMP(lwip, net, buff, timestamp) {
	return 0;
}
/* Called by mac layer or net layer */
NET_RECV(lwip, net, b) {
	struct pbuf *buff = (struct pbuf *) b;
	int ret;

	MIB2_STATS_NETIF_ADD(netif, ifinoctets, buff->tot_len);

	if (((u8_t*)buff->payload)[0] & 1) {
		/* broadcast or multicast packet*/
		MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
	} else {
		/* unicast packet*/
		MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
	}
	LINK_STATS_INC(link.recv);
	ret = net->netif.input(buff, &net->netif);
	if (ret != ERR_OK) {
		LWIP_DEBUGF(NETIF_DEBUG, "IP input error\n");
		LINK_STATS_INC(link.drop);
		MIB2_STATS_NETIF_INC(netif, ifindiscards);
		net_freeNetbuff(net, b);
	}
	return 0;
}
/* Called by this layer to send to mac layer */
NET_SEND(lwip, net, buff) {
	struct mac *mac = net_getMac(net);
	struct net *subNet = net_getParent(net);
	struct pbuf *p = (struct pbuf *) buff;
	int32_t ret = -1;
	
#if ETH_PAD_SIZE
  pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
	/* Mac Layer use this buffer */
	pbuf_ref(p);
	/* check mac layer is set */
	if (mac) {
		ret = mac_send(mac, buff);
	/* no mac layer check net layer is set */
	} else if (subNet) {
		ret = net_send(subNet, buff);
	}
	if (ret >= 0) {
		MIB2_STATS_NETIF_ADD(netif, ifoutoctets, p->tot_len);
		if (((u8_t*)p->payload)[0] & 1) {
			MIB2_STATS_NETIF_INC(netif, ifoutnucastpkts);
		} else {
			MIB2_STATS_NETIF_INC(netif, ifoutucastpkts);
		}
		LINK_STATS_INC(link.xmit);
	}
#if ETH_PAD_SIZE
  pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
	/* else error: no mac or net layer is set */
	return ret;
}
NET_GET_MTU(lwip, net) {
	return net->netif.mtu;
}
NET_SET_UP(lwip, net) {
	netif_set_link_up(&net->netif);
	netif_set_up(&net->netif);
	dhcp_start(&net->netif);
	return 0;
}
NET_SET_DOWN(lwip, net) {
	dhcp_stop(&net->netif);
	netif_set_down(&net->netif);
	netif_set_link_down(&net->netif);
	return 0;
}
NET_OPS(lwip);
