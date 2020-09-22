/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <net/mac.h>
#define MAC_PRV
#include <net/mac_prv.h>
int32_t mac_genericInit(struct mac *m) {
	struct mac_generic *mac = (struct mac_generic *) m;
	if (hal_isInit(mac)) {
		return MAC_ALREDY_INITED;
	}
	{
		int32_t ret = hal_init(mac);
		if (ret < 0) {
			goto mac_genericInit_error0;
		}
	}
	mac->init = true;
	return 0;
mac_genericInit_error0:
	return -1;
}

enum mac_type mac_getType(struct mac *mac);
enum mac_type mac_getStatus(struct mac *mac);
int32_t mac_setNet(struct mac *mac, struct net *net);
struct net *mac_getNet(struct mac *mac);
int32_t mac_setPhy(struct mac *mac, struct phy *phy);
struct phy *mac_getPhy(struct mac *mac);
int32_t mac_setPhys(struct mac *mac, struct phy **phys, uint8_t len);
struct phy **mac_getPhys(struct mac *mac, uint8_t *len);
