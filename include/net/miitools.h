#ifndef MIITOOLS_H_
#define MIITOOLS_H_
#include <net/eth.h>
#include <net/mii.h>
/**
 * mii_stat1000_to_ethtool_lpa_t
 * @adv: value of the MII_STAT1000 register
 *
 * A small helper function that translates MII_STAT1000
 * bits, when in 1000Base-T mode, to ethtool
 * advertisement settings.
 */
static inline uint32_t mii_stat1000_to_ethtool_lpa_t(uint32_t lpa)
{
	uint32_t result = 0;

	if (lpa & LPA_1000HALF) {
		result |= ADVERTISED_1000baseT_Half;
	}
	if (lpa & LPA_1000FULL) {
		result |= ADVERTISED_1000baseT_Full;
	}

	return result;
}
/**
 * mii_adv_to_ethtool_adv_t
 * @adv: value of the MII_ADVERTISE register
 *
 * A small helper function that translates MII_ADVERTISE bits
 * to ethtool advertisement settings.
 */
static inline uint32_t mii_adv_to_ethtool_adv_t(uint32_t adv)
{
	uint32_t result = 0;

	if (adv & ADVERTISE_10HALF) {
		result |= ADVERTISED_10baseT_Half;
	}
	if (adv & ADVERTISE_10FULL) {
		result |= ADVERTISED_10baseT_Full;
	}
	if (adv & ADVERTISE_100HALF) {
		result |= ADVERTISED_100baseT_Half;
	}
	if (adv & ADVERTISE_100FULL) {
		result |= ADVERTISED_100baseT_Full;
	}
	if (adv & ADVERTISE_PAUSE_CAP) {
		result |= ADVERTISED_Pause;
	}
	if (adv & ADVERTISE_PAUSE_ASYM) {
		result |= ADVERTISED_Asym_Pause;
	}
	return result;
}
/**
 * mii_lpa_to_ethtool_lpa_t
 * @adv: value of the MII_LPA register
 *
 * A small helper function that translates MII_LPA
 * bits, when in 1000Base-T mode, to ethtool
 * LP advertisement settings.
 */
static inline uint32_t mii_lpa_to_ethtool_lpa_t(uint32_t lpa)
{
	uint32_t result = 0;

	if (lpa & LPA_LPACK) {
		result |= ADVERTISED_Autoneg;
	}

	return result | mii_adv_to_ethtool_adv_t(lpa);
}
/**
 * ethtool_adv_to_mii_ctrl1000_t
 * @ethadv: the ethtool advertisement settings
 *
 * A small helper function that translates ethtool advertisement
 * settings to phy autonegotiation advertisements for the
 * MII_CTRL1000 register when in 1000T mode.
 */
static inline uint32_t ethtool_adv_to_mii_ctrl1000_t(uint32_t ethadv)
{
	uint32_t result = 0;

	if (ethadv & ADVERTISED_1000baseT_Half)
		result |= ADVERTISE_1000HALF;
	if (ethadv & ADVERTISED_1000baseT_Full)
		result |= ADVERTISE_1000FULL;

	return result;
}
#endif
