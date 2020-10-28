// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (C) 2005 Marc Kleine-Budde, Pengutronix
 * Copyright (C) 2006 Andrey Volkov, Varma Electronics
 * Copyright (C) 2008-2009 Wolfgang Grandegger <wg@grandegger.com>
 * Copyright (C) 2020 Andreas Werner <kernel@andy89.org
 *
 * Based on Linux Kernel Code
 */
#include <can.h>
#define CAN_PRV
#include <can_prv.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <system.h>
/* TODO to config */
# define PRINTF(fmt, ...) printf("CAN: " fmt, ##__VA_ARGS__)

#define CAN_CALC_SYNC_SEG 1
#define CAN_CALC_MAX_ERROR 50 /* in one-tenth of a percent */

/**
 * clamp - return a value clamped to a given range with strict typechecking
 * @val: current value
 * @lo: lowest allowable value
 * @hi: highest allowable value
 *
 * This macro does strict typechecking of @lo/@hi to make sure they are of the
 * same type as @val.  See the unnecessary pointer comparisons.
 */
#define clamp(val, lo, hi) MIN((typeof(val))MAX(val, lo), hi)

/*
 * Bit-timing calculation derived from:
 *
 * Code based on LinCAN sources and H8S2638 project
 * Copyright 2004-2006 Pavel Pisa - DCE FELK CVUT cz
 * Copyright 2005      Stanislav Marek
 * email: pisa@cmp.felk.cvut.cz
 *
 * Calculates proper bit-timing parameters for a specified bit-rate
 * and sample-point, which can then be used to set the bit-timing
 * registers of the CAN controller. You can find more information
 * in the header file linux/can/netlink.h.
 */
static int can_update_sample_point(const struct can_bittiming_const *btc,
			  uint32_t sample_point_nominal, uint32_t tseg,
			  uint32_t *tseg1_ptr, uint32_t *tseg2_ptr,
			  uint32_t *sample_point_error_ptr)
{
	uint32_t sample_point_error, best_sample_point_error = UINT32_MAX;
	uint32_t sample_point, best_sample_point = 0;
	uint32_t tseg1, tseg2;
	int i;

	for (i = 0; i <= 1; i++) {
		tseg2 = tseg + CAN_CALC_SYNC_SEG - (sample_point_nominal * (tseg + CAN_CALC_SYNC_SEG)) / 1000 - i;
		tseg2 = clamp(tseg2, btc->tseg2_min, btc->tseg2_max);
		tseg1 = tseg - tseg2;
		if (tseg1 > btc->tseg1_max) {
			tseg1 = btc->tseg1_max;
			tseg2 = tseg - tseg1;
		}

		sample_point = 1000 * (tseg + CAN_CALC_SYNC_SEG - tseg2) / (tseg + CAN_CALC_SYNC_SEG);
		sample_point_error = abs(sample_point_nominal - sample_point);

		if ((sample_point <= sample_point_nominal) && (sample_point_error < best_sample_point_error)) {
			best_sample_point = sample_point;
			best_sample_point_error = sample_point_error;
			*tseg1_ptr = tseg1;
			*tseg2_ptr = tseg2;
		}
	}

	if (sample_point_error_ptr)
		*sample_point_error_ptr = best_sample_point_error;

	return best_sample_point;
}

int32_t can_calcBittiming(struct can_bittiming *bt, struct can_bittiming_const const *btc, int64_t clkFreq) {
	uint32_t bitrate;			/* current bitrate */
	uint32_t bitrate_error;		/* difference between current and nominal value */
	uint32_t best_bitrate_error = UINT32_MAX;
	uint32_t sample_point_error;	/* difference between current and nominal value */
	uint32_t best_sample_point_error = UINT32_MAX;
	uint32_t sample_point_nominal;	/* nominal sample point */
	uint32_t best_tseg = 0;		/* current best value for tseg */
	uint32_t best_brp = 0;		/* current best value for brp */
	uint32_t brp, tsegall, tseg, tseg1 = 0, tseg2 = 0;
	uint64_t v64;

	/* Use CiA recommended sample points */
	if (bt->sample_point) {
		sample_point_nominal = bt->sample_point;
	} else {
		if (bt->bitrate > 800000)
			sample_point_nominal = 750;
		else if (bt->bitrate > 500000)
			sample_point_nominal = 800;
		else
			sample_point_nominal = 875;
	}

	/* tseg even = round down, odd = round up */
	for (tseg = (btc->tseg1_max + btc->tseg2_max) * 2 + 1;
	     tseg >= (btc->tseg1_min + btc->tseg2_min) * 2; tseg--) {
		tsegall = CAN_CALC_SYNC_SEG + tseg / 2;

		/* Compute all possible tseg choices (tseg=tseg1+tseg2) */
		brp = clkFreq / (tsegall * bt->bitrate) + tseg % 2;

		/* choose brp step which is possible in system */
		brp = (brp / btc->brp_inc) * btc->brp_inc;
		if ((brp < btc->brp_min) || (brp > btc->brp_max))
			continue;

		bitrate = clkFreq / (brp * tsegall);
		bitrate_error = abs(bt->bitrate - bitrate);

		/* tseg brp biterror */
		if (bitrate_error > best_bitrate_error)
			continue;

		/* reset sample point error if we have a better bitrate */
		if (bitrate_error < best_bitrate_error)
			best_sample_point_error = UINT32_MAX;

		can_update_sample_point(btc, sample_point_nominal, tseg / 2, &tseg1, &tseg2, &sample_point_error);
		if (sample_point_error > best_sample_point_error)
			continue;

		best_sample_point_error = sample_point_error;
		best_bitrate_error = bitrate_error;
		best_tseg = tseg / 2;
		best_brp = brp;

		if (bitrate_error == 0 && sample_point_error == 0)
			break;
	}

	if (best_bitrate_error) {
		/* Error in one-tenth of a percent */
		v64 = (uint64_t)best_bitrate_error * 1000;
		//do_div(v64, bt->bitrate);
		v64 = v64 / bt->bitrate;
		bitrate_error = (uint32_t) v64;
		if (bitrate_error > CAN_CALC_MAX_ERROR) {
			PRINTF("bitrate error %ld.%ld%% too high\n", bitrate_error / 10, bitrate_error % 10);
			return -1;
		}
	 	PRINTF("bitrate error %ld.%ld%%\n",
			    bitrate_error / 10, bitrate_error % 10);
	}

	/* real sample point */
	bt->sample_point = can_update_sample_point(btc, sample_point_nominal, best_tseg,
					  &tseg1, &tseg2, NULL);

	v64 = (uint64_t)best_brp * 1000 * 1000 * 1000;
	//do_div(v64, clkFreq);
	v64 = v64 / clkFreq;
	bt->tq = (uint32_t) v64;
	bt->prop_seg = tseg1 / 2;
	bt->phase_seg1 = tseg1 - bt->prop_seg;
	bt->phase_seg2 = tseg2;

	/* check for sjw user settings */
	if (!bt->sjw || !btc->sjw_max) {
		bt->sjw = 1;
	} else {
		/* bt->sjw is at least 1 -> sanitize upper bound to sjw_max */
		if (bt->sjw > btc->sjw_max)
			bt->sjw = btc->sjw_max;
		/* bt->sjw must not be higher than tseg2 */
		if (tseg2 < bt->sjw)
			bt->sjw = tseg2;
	}

	bt->brp = best_brp;

	/* real bitrate */
	bt->bitrate = clkFreq / (bt->brp * (CAN_CALC_SYNC_SEG + tseg1 + tseg2));

	return 0;
}
