#ifndef PHYDEV_H_
#define PHYDEV_H_
#include <net/phy.h>
struct phydev {
	bool reserved;
	const char *name;
	uint32_t phy_id;
	uint32_t phy_id_mask;
	uint32_t features;
	uint32_t flags;
	int32_t (*soft_reset)(struct phy *phy);
	int32_t (*config_init)(struct phy *phy);
	int32_t (*config_aneg)(struct phy *phy);
	int32_t (*aneg_done)(struct phy *phy);
	int32_t (*read_status)(struct phy *phy);
	int32_t (*ack_interrupt)(struct phy *phy);
	int32_t (*config_intr)(struct phy *phy);
	int32_t (*recv_timestamp)(struct phy *phy, uint64_t *timestamp);
	int32_t (*resume)(struct phy *phy);
};
int32_t genphy_soft_reset(struct phy *phy);
int32_t genphy_config_init(struct phy *phy);
int32_t genphy_config_aneg(struct phy *phy);
int32_t genphy_aneg_done(struct phy *phy);
int32_t genphy_read_status(struct phy *phy);
int32_t genphy_resume(struct phy *phydev);
inline int32_t phydev_soft_reset(struct phy *phy) {
	if (phy->dev->soft_reset) {
		return phy->dev->soft_reset(phy);
	}
	return genphy_soft_reset(phy);
}
inline int32_t phydev_config_init(struct phy *phy) {
	if (phy->dev->config_init) {
		return phy->dev->config_init(phy);
	}
	return genphy_config_init(phy);
}
inline int32_t phydev_config_aneg(struct phy *phy) {
	if (phy->dev->config_aneg) {
		return phy->dev->config_aneg(phy);
	}
	return genphy_config_aneg(phy);
}
inline int32_t phydev_aneg_done(struct phy *phy) {
	if (phy->dev->aneg_done) {
		return phy->dev->aneg_done(phy);
	}
	return genphy_aneg_done(phy);
}
inline int32_t phydev_read_status(struct phy *phy) {
	if (phy->dev->read_status) {
		return phy->dev->read_status(phy);
	}
	return genphy_read_status(phy);
}
inline int32_t phydev_ack_interrupt(struct phy *phy) {
	if (phy->dev->ack_interrupt) {
		return phy->dev->ack_interrupt(phy);
	}
	return -1;
}
inline int32_t phydev_config_intr(struct phy *phy) {
	if (phy->dev->config_intr) {
		return phy->dev->config_intr(phy);
	}
	return -1;
}
inline int32_t phydev_recv_timestamp(struct phy *phy, uint64_t *timestamp) {
	if (phy->dev->recv_timestamp) {
		return phy->dev->recv_timestamp(phy, timestamp);
	}
	return -1;
}
inline int32_t phydev_resume(struct phy *phy) {
	if (phy->dev->resume) {
		return phy->dev->resume(phy);
	}
	return genphy_resume(phy);;
}
/**
 * Add a new PHY Driver
 */
#define PHYDEV_ADDDEV(ns, p) HAL_ADDDEV(phydev, ns, p)
/*
 * Define Global Array for Driver Access
 */
HAL_DEFINE_GLOBAL_ARRAY(phydev);
#endif
