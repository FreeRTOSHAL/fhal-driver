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
