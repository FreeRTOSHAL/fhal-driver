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
#ifndef MAC_PRV_
#define MAC_PRV_
#ifndef MAC_PRV
# error "Never include this file out of a MAC driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
/**
 * Error Code Returned if Already inited
 */
#define MAC_ALREDY_INITED 1
/**
 * Generic Init Function
 * 
 * Check Driver Already inited and init mutex if active
 * \param mac Instants of Driver
 * \return MAC_ALREDY_INITED on alredy init 0 on not init < 0 error
 */
int32_t mac_genericInit(struct mac *mac);
/**
 * Lock Driver
 */
# define mac_lock(u, w, e) HAL_LOCK(u, w, e)
/**
 * Unlock Driver
 */
# define mac_unlock(u, e) HAL_UNLOCK(u, e)
/**
 * Add Driver Instance
 * \param ns Driver namespace
 * \param p Pointer to driver instance
 */
#define MAC_ADDDEV(ns, p) HAL_ADDDEV(mac, ns, p)
/*
 * Define Global Array for Driver Access
 */
HAL_DEFINE_GLOBAL_ARRAY(mac);
/**
 * Simple function for access a dev from driver
 * \param index index
 * \return see HAL_GET_DEV
 */
#define MAC_GET_DEV(index) HAL_GET_DEV(mac, index)
#ifndef CONFIG_MAC_MULTI
# define MAC_OPS(ns)
# define MAC_INIT_DEV(ns) 

# define MAC_INIT(ns, index) struct mac *mac_init(uint32_t index)
# define MAC_DEINIT(ns, p) int32_t mac_deinit(struct mac *p)

# define MAC_MDIO_READ(ns, p, id, addr, value) int32_t mac_mdioRead(struct mac *p, uint8_t id, uint8_t addr, uint16_t *value)
# define MAC_MDIO_WRITE(ns, p, id, addr, value) int32_t mac_mdioWrite(struct mac *p, uint8_t id, uint8_t addr, uint16_t value)
# define MAC_SEND(ns, p, buff) int32_t mac_send(struct mac *p, struct netbuff *buff)
# define MAC_ENABLE(ns, p) int32_t mac_enable(struct mac *p)
# define MAC_DISABLE(ns, p) int32_t mac_disable(struct mac *p)
# define MAC_SET_MAC_ADDRESS(ns, p, address) int32_t mac_setMacAddress(struct mac *mac, uint8_t *address)
# define MAC_GET_MAC_ADDRESS(ns, p, address) int32_t mac_getMacAddress(struct mac *mac, uint8_t *address)
# define MAC_GET_MTU(ns, p) int32_t mac_getMTU(struct mac *p)
# define MAC_ADJUST_LINK(ns, p) void mac_adjust_link(struct mac *p)
# define MAC_CONNECT(ns, p, ph, n) int32_t mac_connect(struct mac *p, struct phy *ph, struct net *n)
#else
# error todo implement
/**
 * Define Operation
 * \param ns Driver Namespace
 */
# define MAC_OPS(ns) static const struct mac_ops ns##_mac_ops = { \
	.mac_init = &ns##_mac_init,\
	.mac_deinit = &ns##_mac_deinit,\
	.mac_funcname = &ns##_mac_funcname, \
}
/**
 * Init Driver Struct 
 * \param ns Driver Namespace
 */
# define MAC_INIT_DEV(ns) .gen.ops = &ns##_mac_ops,

/**
 * Define mac_init() Implementation 
 * \param ns Driver namespace Variablenname
 * \param index Index Variablenname
 */
# define MAC_INIT(ns, index) static struct mac *ns##_mac_init(uint32_t index)
/**
 * Define mac_deinit() Implementation
 * \param ns Driver namespace Variablenname
 * \param p Instance Variablenname
 */
# define MAC_DEINIT(ns, p) static int32_t ns##_mac_deinit(struct mac *p)
/**
 * Define mac_funcname() Implementation
 * \param ns Driver namespace Variablenname
 * \param p Instance Variablenname
 * \param param Param Variablenname
 */
# define MAC_FUNCNAME(ns, p, param) static int32_t ns##_mac_funcname(struct mac *p, uint32_t param)
#endif
/**\}*/
#endif
