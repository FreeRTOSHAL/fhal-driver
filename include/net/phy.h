#ifndef PHY_H_
#define PHY_H_
#include <FreeRTOS.h>
#include <task.h>
#include <stdint.h>
#include <net/mii.h>
#include <net/mac.h>
#include <hal.h>
struct phy;
#define PHY_DEFAULT_FEATURES	(SUPPORTED_Autoneg | \
				 SUPPORTED_TP | \
				 SUPPORTED_MII)

#define PHY_10BT_FEATURES	(SUPPORTED_10baseT_Half | \
				 SUPPORTED_10baseT_Full)

#define PHY_100BT_FEATURES	(SUPPORTED_100baseT_Half | \
				 SUPPORTED_100baseT_Full)

#define PHY_1000BT_FEATURES	(SUPPORTED_1000baseT_Half | \
				 SUPPORTED_1000baseT_Full)

#define PHY_BASIC_FEATURES	(PHY_10BT_FEATURES | \
				 PHY_100BT_FEATURES | \
				 PHY_DEFAULT_FEATURES)

#define PHY_GBIT_FEATURES	(PHY_BASIC_FEATURES | \
				 PHY_1000BT_FEATURES)
#define PHY_INTERRUPT_DISABLED	0x0
#define PHY_INTERRUPT_ENABLED	0x80000000
/*
 * Set phydev->irq to PHY_POLL if interrupts are not supported,
 * or not desired for this PHY.  Set to PHY_IGNORE_INTERRUPT if
 * the attached driver handles the interrupt
 */
#define PHY_POLL		-1
#define PHY_IGNORE_INTERRUPT	-2
#define PHY_HAS_INTERRUPT	0x00000001
#define PHY_HAS_MAGICANEG	0x00000002
#define PHY_IS_INTERNAL		0x00000004
#define MDIO_DEVICE_IS_PHY	0x80000000
/* Interface Mode definitions */
typedef enum {
	PHY_INTERFACE_MODE_NA,
	PHY_INTERFACE_MODE_MII,
	PHY_INTERFACE_MODE_GMII,
	PHY_INTERFACE_MODE_SGMII,
	PHY_INTERFACE_MODE_TBI,
	PHY_INTERFACE_MODE_REVMII,
	PHY_INTERFACE_MODE_RMII,
	PHY_INTERFACE_MODE_RGMII,
	PHY_INTERFACE_MODE_RGMII_ID,
	PHY_INTERFACE_MODE_RGMII_RXID,
	PHY_INTERFACE_MODE_RGMII_TXID,
	PHY_INTERFACE_MODE_RTBI,
	PHY_INTERFACE_MODE_SMII,
	PHY_INTERFACE_MODE_XGMII,
	PHY_INTERFACE_MODE_MOCA,
	PHY_INTERFACE_MODE_QSGMII,
	PHY_INTERFACE_MODE_MAX,
} phy_interface_t;
enum phy_state {
	/**
	 * PHY device and driver are not ready for anything.  probe
	 * should be called if and only if the PHY is in this state,
	 * given that the PHY device exists.
	 * - PHY driver probe function will, depending on the PHY, set
	 * the state to STARTING or READY
	 */
	PHY_DOWN = 0,
	/**
	 * PHY device is coming up, and the ethernet driver is
	 * not ready.  PHY drivers may set this in the probe function.
	 * If they do, they are responsible for making sure the state is
	 * eventually set to indicate whether the PHY is UP or READY,
	 * depending on the state when the PHY is done starting up.
	 * - PHY driver will set the state to READY
	 * - start will set the state to PENDING
	 */
	PHY_STARTING,
	/**
	 * PHY is ready to send and receive packets, but the
	 * controller is not.  By default, PHYs which do not implement
	 * probe will be set to this state by phy_probe().  If the PHY
	 * driver knows the PHY is ready, and the PHY state is STARTING,
	 * then it sets this STATE.
	 * - start will set the state to UP
	 */
	PHY_READY,
	/**
	 * PHY device is coming up, but the ethernet driver is
	 * ready.  phy_start will set this state if the PHY state is
	 * STARTING.
	 * - PHY driver will set the state to UP when the PHY is ready
	 */
	PHY_PENDING,
	/**
	 * The PHY and attached device are ready to do work.
	 * Interrupts should be started here.
	 * - timer moves to AN
	 */
	PHY_UP,
	/*
	 * The PHY is currently negotiating the link state.  Link is
	 * therefore down for now.  phy_timer will set this state when it
	 * detects the state is UP.  config_aneg will set this state
	 * whenever called with phydev->autoneg set to AUTONEG_ENABLE.
	 * - If autonegotiation finishes, but there's no link, it sets
	 *   the state to NOLINK.
	 * - If aneg finishes with link, it sets the state to RUNNING,
	 *   and calls adjust_link
	 * - If autonegotiation did not finish after an arbitrary amount
	 *   of time, autonegotiation should be tried again if the PHY
	 *   supports "magic" autonegotiation (back to AN)
	 * - If it didn't finish, and no magic_aneg, move to FORCING.
	 */
	PHY_AN,
	/**
	 * PHY is currently up, running, and possibly sending
	 * and/or receiving packets
	 * - timer will set CHANGELINK if we're polling (this ensures the
	 *   link state is polled every other cycle of this state machine,
	 *   which makes it every other second)
	 * - irq will set CHANGELINK
	 * - config_aneg will set AN
	 * - phy_stop moves to HALTED
	 */
	PHY_RUNNING,
	/**
	 * PHY is up, but not currently plugged in.
	 * - If the timer notes that the link comes back, we move to RUNNING
	 * - config_aneg moves to AN
	 * - phy_stop moves to HALTED
	 */
	PHY_NOLINK,
	/**
	 * PHY is being configured with forced settings
	 * - if link is up, move to RUNNING
	 * - If link is down, we drop to the next highest setting, and
	 *   retry (FORCING) after a timeout
	 * - phy_stop moves to HALTED
	*/
	PHY_FORCING,
	/**
	 * PHY experienced a change in link state
	 * - timer moves to RUNNING if link
	 * - timer moves to NOLINK if the link is down
	 * - phy_stop moves to HALTED
	 */
	PHY_CHANGELINK,
	/**
	 * PHY is up, but no polling or interrupts are done. Or
	 * PHY is in an error state.
 	 * - phy_start moves to RESUMING
	 */
	PHY_HALTED,
	/**
	 * RESUMING: PHY was halted, but now wants to run again.
	 * - If we are forcing, or aneg is done, timer moves to RUNNING
	 * - If aneg is not done, timer moves to AN
	 * - phy_stop moves to HALTED
	 */
	PHY_RESUMING

};
struct phydev;
struct phy {
	struct phydev *dev;
	uint32_t phyid;
	uint32_t id;
	uint32_t supported;
	uint32_t advertising;
	uint32_t lp_advertising;
	enum phy_state state;
	uint32_t pause;
	uint32_t asym_pause;
	uint32_t speed;
	uint32_t duplex;
	uint32_t autoneg;
	uint32_t link;
	uint32_t interrupts;
	phy_interface_t interface;
	struct mac *mdio;
	struct mac *mac;
	OS_DEFINE_TASK(task, 512);
};
int32_t phy_scan(struct mac *mac, struct phy **phys[], uint8_t *len);
struct phy *phy_init(
	int32_t id,
	int32_t phyid,
	struct mac *mdio
);
int32_t phy_deinit(struct phy *phy);
/**
 * Connect phy to Mac
 * Start Phy Handler Task
 * \param mac mac
 * \return 0 on ok -1 on error
 */
int32_t phy_connect(struct phy *phy, struct mac *mac);
int32_t phy_start(struct phy *phy);
#include <net/phydev.h>
#include <net/mac.h>
inline int32_t phy_read(struct phy *phy, uint8_t addr, uint16_t *value) {
	return mac_mdioRead(phy->mdio, phy->id, addr, value);
}
inline int32_t phy_write(struct phy *phy, uint8_t addr, uint16_t value) {
	return mac_mdioWrite(phy->mdio, phy->id, addr, value);
}
inline int32_t phy_soft_reset(struct phy *phy) {
	return phydev_soft_reset(phy);
}
inline int32_t phy_config_init(struct phy *phy) {
	return phydev_config_init(phy);
}
inline int32_t phy_config_aneg(struct phy *phy) {
	return phydev_config_aneg(phy);
}
inline int32_t phy_aneg_done(struct phy *phy) {
	return phydev_aneg_done(phy);
}
inline int32_t phy_read_status(struct phy *phy) {
	return phydev_read_status(phy);
}
inline int32_t phy_ack_interrupt(struct phy *phy) {
	return phydev_ack_interrupt(phy);
}
inline int32_t phy_recv_timestamp(struct phy *phy, uint64_t *timestamp) {
	return phydev_recv_timestamp(phy, timestamp);
}
inline int32_t phy_resume(struct phy *phy) {
	return phydev_resume(phy);
}
#endif
