/*
 * This file is based on the phy_device of Linux Kernel
 * Copyright (c) 2016 Andreas Werner <kernel@andy89.org>
 * Copyright (c) 2004 Freescale Semiconductor, Inc.
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */
#include <FreeRTOS.h>
#include <task.h>
#include <hal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <net/phy.h>
#include <net/phydev.h>
#include <net/mii.h>
#include <net/miitools.h>
#include <net/eth.h>
#include <net/mac.h>
#define PHY_AN_TIMEOUT (10 * 1000) / portTICK_PERIOD_MS
#define PHY_DEBUG(phy, fmt, ...) printf("phy %lu: " fmt, phy->id, ##__VA_ARGS__)
struct phydev phy_gen;
const char *StateString[] = {
	"PHY_DOWN",
	"PHY_STARTING",
	"PHY_READY",
	"PHY_PENDING",
	"PHY_UP",
	"PHY_AN",
	"PHY_RUNNING",
	"PHY_NOLINK",
	"PHY_FORCING",
	"PHY_CHANGELINK",
	"PHY_HALTED",
	"PHY_RESUMING"
};
/* A structure for mapping a particular speed and duplex
 * combination to a particular SUPPORTED and ADVERTISED value
 */
struct phy_setting {
	int speed;
	int duplex;
	uint32_t setting;
};
/* A mapping of all SUPPORTED settings to speed/duplex */
static const struct phy_setting settings[] = {
	{
		.speed = SPEED_10000,
		.duplex = DUPLEX_FULL,
		.setting = SUPPORTED_10000baseKR_Full,
	},
	{
		.speed = SPEED_10000,
		.duplex = DUPLEX_FULL,
		.setting = SUPPORTED_10000baseKX4_Full,
	},
	{
		.speed = SPEED_10000,
		.duplex = DUPLEX_FULL,
		.setting = SUPPORTED_10000baseT_Full,
	},
	{
		.speed = SPEED_2500,
		.duplex = DUPLEX_FULL,
		.setting = SUPPORTED_2500baseX_Full,
	},
	{
		.speed = SPEED_1000,
		.duplex = DUPLEX_FULL,
		.setting = SUPPORTED_1000baseKX_Full,
	},
	{
		.speed = SPEED_1000,
		.duplex = DUPLEX_FULL,
		.setting = SUPPORTED_1000baseT_Full,
	},
	{
		.speed = SPEED_1000,
		.duplex = DUPLEX_HALF,
		.setting = SUPPORTED_1000baseT_Half,
	},
	{
		.speed = SPEED_100,
		.duplex = DUPLEX_FULL,
		.setting = SUPPORTED_100baseT_Full,
	},
	{
		.speed = SPEED_100,
		.duplex = DUPLEX_HALF,
		.setting = SUPPORTED_100baseT_Half,
	},
	{
		.speed = SPEED_10,
		.duplex = DUPLEX_FULL,
		.setting = SUPPORTED_10baseT_Full,
	},
	{
		.speed = SPEED_10,
		.duplex = DUPLEX_HALF,
		.setting = SUPPORTED_10baseT_Half,
	},
};
#define MAX_NUM_SETTINGS ARRAY_SIZE(settings)
/**
 * phy_find_setting - find a PHY settings array entry that matches speed & duplex
 * @speed: speed to match
 * @duplex: duplex to match
 *
 * Description: Searches the settings array for the setting which
 *   matches the desired speed and duplex, and returns the index
 *   of that setting.  Returns the index of the last setting if
 *   none of the others match.
 */
static inline unsigned int phy_find_setting(uint32_t speed, uint32_t duplex)
{
	uint32_t idx = 0;

	while (idx < ARRAY_SIZE(settings) &&
	       (settings[idx].speed != speed || settings[idx].duplex != duplex))
		idx++;

	return idx < MAX_NUM_SETTINGS ? idx : MAX_NUM_SETTINGS - 1;
}
/**
 * phy_find_valid - find a PHY setting that matches the requested features mask
 * @idx: The first index in settings[] to search
 * @features: A mask of the valid settings
 *
 * Description: Returns the index of the first valid setting less
 *   than or equal to the one pointed to by idx, as determined by
 *   the mask in features.  Returns the index of the last setting
 *   if nothing else matches.
 */
static inline uint32_t phy_find_valid(uint32_t idx, uint32_t features)
{
	while (idx < MAX_NUM_SETTINGS && !(settings[idx].setting & features))
		idx++;

	return idx < MAX_NUM_SETTINGS ? idx : MAX_NUM_SETTINGS - 1;
}

static int32_t phy_getPhyID(struct mac *mac, uint8_t id, uint32_t *phy_id) {
	int32_t ret;
	uint16_t phy_reg;
	ret = mac_mdioRead(mac, id, MII_PHYSID1, &phy_reg);
	if (ret < 0) {
		return -1;
	}
	*phy_id = (((uint32_t) phy_reg) & 0xFFFF) << 16;
	ret = mac_mdioRead(mac, id, MII_PHYSID2, &phy_reg);
	if (ret < 0) {
		return -1;
	}
	*phy_id |= (uint32_t) phy_reg;
	printf("id: %d phyid: 0x%08lx\n", id, *phy_id);
	return 0;
}
int32_t phy_scan(struct mac *mac, struct phy **p[], uint8_t *l) {
	struct phy **phys;
	uint8_t len;
	int32_t ret;
	uint32_t phy_id;
	int8_t id = 0;
	int i;
	len = 0;
	/**
	 * Count all Phys
	 */
	for (id = 0; id < 32; id++) {
		ret = phy_getPhyID(mac, id, &phy_id);
		if (ret < 0) {
			goto phy_scan_error0;
		}
		if (phy_id != 0xFFFFFFFF) {
			len++;
		}
	}
	phys = pvPortMalloc(sizeof(struct phy *) * len);
	if (phys == NULL) {
		ret = -1;
		goto phy_scan_error0;
	}
	for (i = 0, id = 0; id < 32 && i < len; id++) {
		ret = phy_getPhyID(mac, id, &phy_id);
		if (ret < 0) {
			goto phy_scan_error0;
		}
		if (phy_id == 0xFFFFFFFF) {
			continue;
		}
		phys[i] = phy_init(id, phy_id, mac);
		if (phys[i] == NULL) {
			goto phy_scan_error1;
		}
		i++;
	}
	*l = len;
	*p = phys;
	return 0;
phy_scan_error1:
	for (;i > 0;i--) {
		phy_deinit(phys[i-1]);
	}
	vPortFree(phys);
phy_scan_error0:
	return ret;
}
static int32_t phy_findDriver(struct phy *phy) {
	HAL_DEFINE_GLOBAL_ARRAY(phydev);
	struct phydev **devs;
	for (devs = (struct phydev **) &_dev_phydev; devs < ((struct phydev **) &_dev_phydev_end); devs++) {
		struct phydev *dev = *devs;
		if ((phy->phyid & dev->phy_id_mask) == (dev->phy_id & dev->phy_id_mask)) {
			phy->dev = dev;
			printf("Found driver %s for phy: %ld with phyid: 0x%08lx\n", dev->name, phy->id, phy->phyid);
			return 1;
		}
	}
	printf("Found no driver for phy: %ld with phyid: 0x%08lx -> use generic driver\n", phy->id, phy->phyid);
	return -1;
}
struct phy *phy_init(
	int32_t id,
	int32_t phyid,
	struct mac *mdio
) {
	int32_t ret;
	struct phy *phy = pvPortMalloc(sizeof(struct phy));
	if (phy == NULL) {
		goto phy_init_error0;
	}
	memset(phy, 0, sizeof(struct phy));
	phy->mdio = mdio;
	phy->id = id;
	phy->phyid = phyid;

	ret = phy_findDriver(phy);
	if (ret < 0) {
		phy->dev = &phy_gen;
	}
	phy->supported = 0xFFFFFFFF;
	phy->advertising = 0xFFFFFFFF;
	phy->state = PHY_READY;
	/* todo interrups */
	return phy;
phy_init_error0:
	return NULL;
}
int32_t phy_deinit(struct phy *phy) {
	/* TODO deinit */
	return 0;
}
static int32_t phy_init_hw(struct phy *phy) {
	int32_t ret;
	ret = phy_soft_reset(phy);
	if (ret < 0) {
		return -1;
	}
	ret = phy_config_init(phy);
	if (ret < 0) {
		return -1;
	}
	if (phy->supported & SUPPORTED_Autoneg) {
		phy->autoneg = AUTONEG_ENABLE;
	}
	return 0;
}
static void phy_sanitize_settings(struct phy *phydev) {
	int32_t idx;
	uint32_t features = phydev->supported;
	if ((features & SUPPORTED_Autoneg) == 0) {
		phydev->autoneg = AUTONEG_DISABLE;
	}
	idx = phy_find_valid(phy_find_setting(phydev->speed, phydev->duplex), features);
	phydev->speed = settings[idx].speed;
	phydev->duplex = settings[idx].duplex;
}
static int32_t phy_start_aneg(struct phy *phydev) {
	int32_t ret;
	if (AUTONEG_DISABLE == phydev->autoneg) {
		phy_sanitize_settings(phydev);
	}
	phydev->lp_advertising = 0;
	ret = phy_config_aneg(phydev);
	if (ret < 0) {
		return -1;
	}
	if (phydev->state != PHY_HALTED) {
		if (AUTONEG_ENABLE == phydev->autoneg) {
			phydev->state = PHY_AN;
		} else {
			phydev->state = PHY_FORCING;
		}
	}
	return 0;
}
static void phy_state_machine(void *data) {
	int32_t ret;
	struct phy *phy = data;
	TickType_t time = xTaskGetTickCount();
	TickType_t oldtime = time;
	int32_t timeout;
	TickType_t tmp;
	enum phy_state oldstate;
	uint32_t oldspeed = phy->speed;
	uint32_t oldduplex = phy->duplex;
	for(;;) {
		oldstate = phy->state;
		oldspeed = phy->speed;
		oldduplex = phy->duplex;
		switch(phy->state) {
			case PHY_DOWN:
			case PHY_STARTING:
			case PHY_READY:
			case PHY_PENDING:
				break;
			case PHY_UP:
				timeout = PHY_AN_TIMEOUT;
				phy_start_aneg(phy);
				break;
			case PHY_AN:
				if (!phy->link) {
					phy->state = PHY_NOLINK;
					/* notificate mac layer link has changed */
					mac_adjust_link(phy->mac);
				}
				/* Check Timeout */
				/* Overflow */
				if (time > oldtime) {
					tmp = time - oldtime;
				} else {
					tmp = (UINT32_MAX - time) + oldtime;
				}
				timeout -= tmp;
				if (timeout < 0) {
					PHY_DEBUG(phy, "Timeout try again\n");
					/* Try again */
					phy->state = PHY_UP;
				}
				ret = phy_aneg_done(phy);
				if (ret < 0) {
					/* TODO error */
					break;
				}
				if (ret > 0) {
					phy->state = PHY_RUNNING;
					/* notificate mac layer link has changed */
					mac_adjust_link(phy->mac);
				}
				break;
			case PHY_NOLINK:
				ret = phy_read_status(phy);
				if (ret < 0) {
					/* TODO error */
					break;
				}
				if (phy->link) {
					if (AUTONEG_ENABLE == phy->autoneg) {
						ret = phy_aneg_done(phy);
						if (ret < 0) {
							/* TODO error */
							break;
						}
						if (ret == 0) {
							phy->state = PHY_UP;
							timeout = PHY_AN_TIMEOUT;
							break;
						}
						phy->state = PHY_RUNNING;
						/* notificate mac layer link has changed */
						mac_adjust_link(phy->mac);
					}
				}
				break;
			case PHY_FORCING:
				/* TODO */
				CONFIG_ASSERT(0);
				break;

			case PHY_RUNNING:
				{
					uint32_t oldlink = phy->link;
					ret = phy_read_status(phy);
					if (ret < 0) {
						/* TODO error */
						break;
					}
					if (oldlink != phy->link) {
						phy->state = PHY_CHANGELINK;
					}
					if (oldspeed != phy->speed || oldduplex !=  phy->duplex) {
						/* notificate mac layer link has changed */
						mac_adjust_link(phy->mac);
					}
				}
				break;
			case PHY_CHANGELINK:
				ret = phy_read_status(phy);
				if (ret < 0) {
					/* TODO error */
					break;
				}
				if (phy->link) {
					phy->state = PHY_RUNNING;
				} else {
					phy->state = PHY_NOLINK;
				}
				/* notificate mac layer link has changed */
				mac_adjust_link(phy->mac);
				break;
			case PHY_HALTED:
				if (phy->link) {
					phy->link = 0;
					/* notificate mac layer link has changed */
					mac_adjust_link(phy->mac);
					/* TODO Shutdown phy */
				}
				break;
			case PHY_RESUMING:
				/* Not Supported */
				CONFIG_ASSERT(0);
				break;
			default:
				CONFIG_ASSERT(0);
		}
		if (oldstate != phy->state || oldspeed != phy->speed || oldduplex !=  phy->duplex) {
			PHY_DEBUG(phy, "State changed: %s -> %s\n", StateString[oldstate], StateString[phy->state]);
			if (phy->state == PHY_RUNNING) {
				PHY_DEBUG(phy, "Speed: %lu MBit %s\n", phy->speed, (phy->duplex == DUPLEX_FULL)? "Full": "Half");
			}
			oldtime = time;
			time = xTaskGetTickCount();
		} else {
			oldtime = time;
			vTaskDelayUntil(&time, 1000 / portTICK_PERIOD_MS);
		}
	}
}
static int32_t phy_start_machine(struct phy *phy) {
	BaseType_t ret = xTaskCreate(phy_state_machine, "Phy Task", 255, phy, CONFIG_PHY_TASK_PRIO, &phy->task);
	if (ret != pdPASS) {
		return -1;
	}
	return 0;
}
int32_t phy_connect(struct phy *phy, struct mac *mac) {
	int32_t ret;
	if (phy->task != NULL) {
		/* Phy is already connected */
		return -1;
	}
	phy->mac = mac;
	phy->state = PHY_READY;
	ret = phy_init_hw(phy);
	if (ret < 0) {
		return -1;
	}
	/* resume phy */
	ret = phy_resume(phy);
	if (ret < 0) {
		return -1;
	}
	ret = phy_start_machine(phy);
	if (ret < 0) {
		return -1;
	}
	return 0;
}
int32_t phy_start(struct phy *phy) {
	switch (phy->state) {
		case PHY_STARTING:
			phy->state = PHY_PENDING;
			break;
		case PHY_READY:
			phy->state = PHY_NOLINK;
		default:
			break;
	}
	return 0;
}
int32_t genphy_pollReset(struct phy *phy) {
	uint32_t retries = 12;
	int32_t ret;
	uint16_t value;
	do {
		vTaskDelay(50 / portTICK_PERIOD_MS);
		ret = phy_read(phy, MII_BMCR, &value);
		if (ret < 0) {
			return ret;
		}
	} while (value & BMCR_RESET && --retries);
	if (value & BMCR_RESET) {
		return -1;
	}
	/* Some chips (smsc911x) may still need up to another 1ms after the
	 * BMCR_RESET bit is cleared before they are usable.
	 */
	vTaskDelay(1 / portTICK_PERIOD_MS);
	return 0;
}
int32_t genphy_soft_reset(struct phy *phy) {
	int32_t ret = phy_write(phy, MII_BMCR, BMCR_RESET);
	if (ret < 0) {
		return ret;
	}
	return genphy_pollReset(phy);
}
int32_t genphy_config_init(struct phy *phydev) {
	uint16_t val;
	int32_t ret;
	uint32_t features;

	features = (SUPPORTED_TP | SUPPORTED_MII
			| SUPPORTED_AUI | SUPPORTED_FIBRE |
			SUPPORTED_BNC | SUPPORTED_Pause | SUPPORTED_Asym_Pause);

	/* Do we support autonegotiation? */
	ret = phy_read(phydev, MII_BMSR, &val);
	if (ret < 0) {
		return ret;
	}

	if (val & BMSR_ANEGCAPABLE) {
		features |= SUPPORTED_Autoneg;
	}

	if (val & BMSR_100FULL) {
		features |= SUPPORTED_100baseT_Full;
	}
	if (val & BMSR_100HALF) {
		features |= SUPPORTED_100baseT_Half;
	}
	if (val & BMSR_10FULL) {
		features |= SUPPORTED_10baseT_Full;
	}
	if (val & BMSR_10HALF) {
		features |= SUPPORTED_10baseT_Half;
	}

	if (val & BMSR_ESTATEN) {
		ret = phy_read(phydev, MII_ESTATUS, &val);
		if (ret < 0) {
			return ret;
		}

		if (val & ESTATUS_1000_TFULL) {
			features |= SUPPORTED_1000baseT_Full;
		}
		if (val & ESTATUS_1000_THALF) {
			features |= SUPPORTED_1000baseT_Half;
		}
	}

	phydev->supported &= features;
	phydev->advertising &= features;

	return 0;
}
/**
 * genphy_setup_forced - configures/forces speed/duplex from @phydev
 * @phydev: target phy_device struct
 *
 * Description: Configures MII_BMCR to force speed/duplex
 *   to the values in phydev. Assumes that the values are valid.
 *   Please see phy_sanitize_settings().
 */
int32_t genphy_setup_forced(struct phy *phydev)
{
	int32_t ret;
	uint16_t ctl;
	ret = phy_read(phydev, MII_BMCR, &ctl);
	if (ret < 0) {
		return -1;
	}

	ctl &= BMCR_LOOPBACK | BMCR_ISOLATE | BMCR_PDOWN;
	phydev->pause = 0;
	phydev->asym_pause = 0;

	if (SPEED_1000 == phydev->speed) {
		ctl |= BMCR_SPEED1000;
	} else if (SPEED_100 == phydev->speed) {
		ctl |= BMCR_SPEED100;
	}

	if (DUPLEX_FULL == phydev->duplex) {
		ctl |= BMCR_FULLDPLX;
	}

	return phy_write(phydev, MII_BMCR, ctl);
}

/**
 * genphy_restart_aneg - Enable and Restart Autonegotiation
 * @phydev: target phy_device struct
 */
int32_t genphy_restart_aneg(struct phy *phydev)
{
	int32_t ret;
	uint16_t ctl;
	ret = phy_read(phydev, MII_BMCR, &ctl);
	if (ret < 0) {
		return ret;
	}

	ctl |= BMCR_ANENABLE | BMCR_ANRESTART;

	/* Don't isolate the PHY if we're negotiating */
	ctl &= ~BMCR_ISOLATE;

	return phy_write(phydev, MII_BMCR, ctl);
}
/**
 * genphy_config_advert - sanitize and advertise auto-negotiation parameters
 * @phydev: target phy_device struct
 *
 * Description: Writes MII_ADVERTISE with the appropriate values,
 *   after sanitizing the values to make sure we only advertise
 *   what is supported.  Returns < 0 on error, 0 if the PHY's advertisement
 *   hasn't changed, and > 0 if it has changed.
 */
int32_t genphy_config_advert(struct phy *phydev)
{
	uint32_t advertise;
	uint32_t oldadv;
	uint16_t adv;
	uint16_t bmsr;
	int32_t err = 0;
	int32_t changed = 0;
	int32_t ret;

	/* Only allow advertising what this PHY supports */
	phydev->advertising &= phydev->supported;
	advertise = phydev->advertising;

	/* Setup standard advertisement */
	ret = phy_read(phydev, MII_ADVERTISE, &adv);
	if (ret < 0) {
		return ret;
	}

	oldadv = adv;
	adv &= ~(ADVERTISE_ALL | ADVERTISE_100BASE4 | ADVERTISE_PAUSE_CAP |
		 ADVERTISE_PAUSE_ASYM);
	adv |= ethtool_adv_to_mii_adv_t(advertise);

	if (adv != oldadv) {
		ret = phy_write(phydev, MII_ADVERTISE, adv);

		if (ret < 0) {
			return ret;
		}
		changed = 1;
	}

	ret = phy_read(phydev, MII_BMSR, &bmsr);
	if (ret < 0) {
		return ret;
	}

	/* Per 802.3-2008, Section 22.2.4.2.16 Extended status all
	 * 1000Mbits/sec capable PHYs shall have the BMSR_ESTATEN bit set to a
	 * logical 1.
	 */
	if (!(bmsr & BMSR_ESTATEN)) {
		return changed;
	}

	/* Configure gigabit if it's supported */
	ret = phy_read(phydev, MII_CTRL1000, &adv);
	if (ret < 0) {
		return ret;
	}

	oldadv = adv;
	adv &= ~(ADVERTISE_1000FULL | ADVERTISE_1000HALF);

	if (phydev->supported & (SUPPORTED_1000baseT_Half |
				 SUPPORTED_1000baseT_Full)) {
		adv |= ethtool_adv_to_mii_ctrl1000_t(advertise);
	}

	if (adv != oldadv) {
		changed = 1;
	}

	err = phy_write(phydev, MII_CTRL1000, adv);
	if (err < 0) {
		return err;
	}

	return changed;
}
int32_t genphy_config_aneg(struct phy *phydev) {
	int32_t result;
	int32_t ret;

	if (AUTONEG_ENABLE != phydev->autoneg) {
		return genphy_setup_forced(phydev);
	}

	result = genphy_config_advert(phydev);
	if (result < 0) {/* error */
		return result;
	}
	if (result == 0) {
		uint16_t ctl;
		/* Advertisement hasn't changed, but maybe aneg was never on to
		 * begin with?  Or maybe phy was isolated?
		 */
		ret = phy_read(phydev, MII_BMCR, &ctl);

		if (ret < 0) {
			return ret;
		}

		if (!(ctl & BMCR_ANENABLE) || (ctl & BMCR_ISOLATE)) {
			result = 1; /* do restart aneg */
		}
	}

	/* Only restart aneg if we are advertising something different
	 * than we were before.
	 */
	if (result > 0) {
		result = genphy_restart_aneg(phydev);
	}

	return result;
}

int32_t genphy_aneg_done(struct phy *phydev) {
	int32_t ret;
	uint16_t retval;
	ret = phy_read(phydev, MII_BMSR, &retval);

	return (ret < 0) ? ret : (retval & BMSR_ANEGCOMPLETE);
}
/**
 * genphy_update_link - update link status in @phydev
 * @phydev: target phy_device struct
 *
 * Description: Update the value in phydev->link to reflect the
 *   current link value.  In order to do this, we need to read
 *   the status register twice, keeping the second value.
 */
int32_t genphy_update_link(struct phy *phydev)
{
	uint16_t status;
	int32_t ret;

	/* Do a fake read */
	ret = phy_read(phydev, MII_BMSR, &status);
	if (ret < 0) {
		return ret;
	}

	/* Read link and autonegotiation status */
	ret = phy_read(phydev, MII_BMSR, &status);
	if (ret < 0) {
		return ret;
	}

	if ((status & BMSR_LSTATUS) == 0) {
		phydev->link = 0;
	} else {
		phydev->link = 1;
	}

	return 0;
}
int32_t genphy_resume(struct phy *phydev)
{
	uint16_t value;
	uint32_t ret;
	ret = phy_read(phydev, MII_BMCR, &value);
	if (ret < 0) {
		return -1;
	}
	ret = phy_write(phydev, MII_BMCR, value & ~BMCR_PDOWN);
	return ret;
}

int32_t genphy_read_status(struct phy *phydev) {
	int32_t ret;
	uint16_t lpa;
	uint16_t lpagb = 0;
	uint16_t common_adv;
	uint16_t common_adv_gb = 0;
	uint16_t adv;

	/* Update the link, but return if there was an error */
	ret = genphy_update_link(phydev);
	if (ret) {
		return ret;
	}

	phydev->lp_advertising = 0;

	if (AUTONEG_ENABLE == phydev->autoneg) {
		if (phydev->supported & (SUPPORTED_1000baseT_Half
					| SUPPORTED_1000baseT_Full)) {
			
			ret = phy_read(phydev, MII_STAT1000, &lpagb);
			if (ret < 0) {
				return ret;
			}

			ret = phy_read(phydev, MII_CTRL1000, &adv);
			if (ret < 0) {
				return ret;
			}

			phydev->lp_advertising =
				mii_stat1000_to_ethtool_lpa_t(lpagb);
			common_adv_gb = lpagb & adv << 2;
		}

		ret = phy_read(phydev, MII_LPA, &lpa);
		if (ret < 0) {
			return ret;
		}

		phydev->lp_advertising |= mii_lpa_to_ethtool_lpa_t(lpa);

		ret = phy_read(phydev, MII_ADVERTISE, &adv);
		if (ret < 0)
			return ret;

		common_adv = lpa & adv;

		phydev->speed = SPEED_10;
		phydev->duplex = DUPLEX_HALF;
		phydev->pause = 0;
		phydev->asym_pause = 0;

		if (common_adv_gb & (LPA_1000FULL | LPA_1000HALF)) {
			phydev->speed = SPEED_1000;

			if (common_adv_gb & LPA_1000FULL) {
				phydev->duplex = DUPLEX_FULL;
			}
		} else if (common_adv & (LPA_100FULL | LPA_100HALF)) {
			phydev->speed = SPEED_100;

			if (common_adv & LPA_100FULL) {
				phydev->duplex = DUPLEX_FULL;
			}
		} else {
			if (common_adv & LPA_10FULL) {
				phydev->duplex = DUPLEX_FULL;
			}
		}
		if (phydev->duplex == DUPLEX_FULL) {
			phydev->pause = lpa & LPA_PAUSE_CAP ? 1 : 0;
			phydev->asym_pause = lpa & LPA_PAUSE_ASYM ? 1 : 0;
		}
	} else {
		uint16_t bmcr;
		int32_t ret = phy_read(phydev, MII_BMCR, &bmcr);

		if (ret < 0) {
			return ret;
		}

		if (bmcr & BMCR_FULLDPLX) {
			phydev->duplex = DUPLEX_FULL;
		} else {
			phydev->duplex = DUPLEX_HALF;
		}

		if (bmcr & BMCR_SPEED1000) {
			phydev->speed = SPEED_1000;
		} else if (bmcr & BMCR_SPEED100) {
			phydev->speed = SPEED_100;
		} else {
			phydev->speed = SPEED_10;
		}

		phydev->pause = 0;
		phydev->asym_pause = 0;
	}

	return 0;
}

struct phydev phy_gen = {
	.phy_id = 0xffffffff,
	.phy_id_mask = 0xffffffff,
	.name = "Generic PHY",
	.features = PHY_GBIT_FEATURES | SUPPORTED_MII | SUPPORTED_AUI | SUPPORTED_FIBRE | SUPPORTED_BNC,
	.soft_reset = genphy_soft_reset,
	.config_init = genphy_config_init,
	.config_aneg = genphy_config_aneg,
	.aneg_done = genphy_aneg_done,
	.read_status = genphy_read_status,
	.ack_interrupt = NULL,
	.config_intr = NULL,
	.recv_timestamp = NULL,
	.resume = genphy_resume,
};
int32_t phy_read(struct phy *phy, uint8_t addr, uint16_t *value);
int32_t phy_write(struct phy *phy, uint8_t addr, uint16_t value);
int32_t phy_soft_reset(struct phy *phy);
int32_t phy_config_init(struct phy *phy);
int32_t phy_config_aneg(struct phy *phy);
int32_t phy_read_status(struct phy *phy);
int32_t phy_ack_interrupt(struct phy *phy);
int32_t phy_config_intr(struct phy *phy);
int32_t phy_recv_timestamp(struct phy *phy, uint64_t *timestamp);
int32_t phydev_soft_reset(struct phy *phy);
int32_t phydev_config_init(struct phy *phy);
int32_t phydev_config_aneg(struct phy *phy);
int32_t phydev_aneg_done(struct phy *phy);
int32_t phydev_read_status(struct phy *phy);
int32_t phydev_ack_interrupt(struct phy *phy);
int32_t phydev_config_intr(struct phy *phy);
int32_t phydev_recv_timestamp(struct phy *phy, uint64_t *timestamp);
int32_t phy_aneg_done(struct phy *phy);
int32_t phydev_resume(struct phy *phy);
int32_t phy_resume(struct phy *phy);
