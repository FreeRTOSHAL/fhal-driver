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
#include <stdint.h>
#include <string.h>
#include <spi.h>
#include <tps65381.h>
#include <math.h>
#define TPS_STAT (BIT(1) | BIT(3))
#define TPS_STAT_CHECK(x) (((((x) >> 4) & 0xF) == TPS_STAT) && (((x) & 0x1) == 0x0)) 

#define TPS_SW_LOCK 0xBD
#define TPS_SW_LOCK_DATA 0x55
#define TPS_SW_UNLOCK 0xBB
#define TPS_SW_UNLOCK_DATA 0xAA
#define TPS_RD_DEV_ID 0x06
#define TPS_RD_DEV_ID_DATA 0x01
#define TPS_RD_DEV_REV 0x0C
#define TPS_RD_DEV_REV_DATA 0x30
#define TPS_WR_DEV_CFG1 0xB7
#define TPS_DEV_CFG1_VDD_3_5_SEL BIT(7)
#define TPS_DEV_CFG1_nMASK_VDD1_UV_OV BIT(6)
#define TPS_RD_DEV_CFG1 0xAF
#define TPS_WR_DEV_CFG2 0x95
#define TPS_DEV_CFG2_EN_VDD35_OT BIT(7)
#define TPS_DEV_CFG2_EN_VDD5_OT BIT(6)
#define TPS_DEV_CFG2_MASK_VBATP_OV BIT(5)
#define TPS_DEV_CFG2_POST_RUN_RST BIT(4)
#define TPS_DEV_CFG2 0x48
#define TPS_CAN_STBY 0x7D
#define TPS_RD_SAFETY_STAT_1 0x24
#define TPS_SAFETY_STAT_1_VDD5_ILIM BIT(7)
#define TPS_SAFETY_STAT_1_VDD3_5_ILIM BIT(6)
#define TPS_SAFETY_STAT_1_VSOUT1_UV BIT(5)
#define TPS_SAFETY_STAT_1_VSOUT1_OV BIT(4)
#define TPS_SAFETY_STAT_1_VSOUT1_ILIM BIT(3)
#define TPS_SAFETY_STAT_1_VSOUT1_OT BIT(2)
#define TPS_SAFETY_STAT_1_VDD5_OT BIT(1)
#define TPS_SAFETY_STAT_1_VDD_1_5_OT BTI(0)
#define TPS_RD_SAFETY_STAT_2 0xC5
#define TPS_SAFETY_STAT_2_CFG_CRC_ERR BIT(5)
#define TPS_SAFETY_STAT_2_EE_CRC_ERR BIT(4)
#define TPS_SAFETY_STAT_2_WDT_FAIL_CNT(x) (((x) >> 0) & 0x7)
#define TPS_RD_SAFETY_STAT_3 0xA3
#define TPS_SAFETY_STAT_3_NRES_ERR BIT(5)
#define TPS_SAFETY_STAT_3_LBIST_ERR BIT(4)
#define TPS_SAFETY_STAT_3_ABIST_UVOV_ERR BIT(3)
#define TPS_SAFETY_STAT_3_ABIST_UVOV_ERR_2 BIT(2)
#define TPS_SAFETY_STAT_3_LBIST_RUN BIT(1)
#define TPS_SAFETY_STAT_3_ABIST_RUN BIT(0)
#define TPS_RD_SAFETY_STAT_4 0xA5
#define TPS_SAFETY_STAT_4_SPI_ERR(x) (((x) >> 6) & 0x3)
#define TPS_SAFETY_STAT_4_LOCLK BIT(5)
#define TPS_SAFETY_STAT_4_MCU_ERR BIT(3)
#define TPS_SAFETY_STAT_4_WD_ERR BIT(2)
#define TPS_SAFETY_STAT_4_ENDRV_ERR BIT(1)
#define TPS_SAFETY_STAT_4_TRIM_ERR_VMON BIT(0)
#define TPS_RD_SAFETY_STAT_5 0xC0
#define TPS_SAFETY_STAT_5_FTM_IS_STANDBY(x) ((x & 0x7) == 0x0)
#define TPS_SAFETY_STAT_5_FTM_IS_RESET(x) ((x & 0x7) == 0x3)
#define TPS_SAFETY_STAT_5_FTM_IS_DIAGNOSTIC(x) ((x & 0x7) == 0x7)
#define TPS_SAFETY_STAT_5_FTM_IS_ACTIVE(x) ((x & 0x7) == 0x5)
#define TPS_SAFETY_STAT_5_FTM_IS_SAFE(x) ((x & 0x7) == 0x4)
#define TPS_RD_SAFETY_ERR_CFG 0x30
#define TPS_SAFETY_ERR_CFG_CFG_LOCK BIT(0)
#define TPS_WR_SAFETY_ERR_CFG 0xDB
#define TPS_SAFETY_ERR_CFG_SAFE_TO(x) (((x) & 0x7) << 5)
#define TPS_SAFETY_ERR_CFG_SAFE_LOCK_THR(x) (((x) & 0x7) << 1)
#define TPS_SAFETY_ERR_CFG_CFG_LOCK BIT(0)
#define TPS_WR_SAFETY_ERR_STAT 0xA9
#define TPS_RD_SAFETY_ERR_STAT 0xAA
#define TPS_SAFETY_ERR_STAT_ERROR_PIN_FAIL BIT(5)
#define TPS_SAFETY_ERR_STAT_WD_FAIL BIT(4)
#define TPS_SAFETY_ERR_STAT_DEV_ERR_CNT(x) (((x) & 0x7) << 0)
#define TPS_RD_SAFETY_PWD_THR_CFG 0x39
#define TPS_WR_SAFETY_PWD_THR_CFG 0x99
#define TPS_RD_SAFETY_CHECK_CTRL 0x44
#define TPS_SAFETY_CHECK_CTRL_CFG_CRC_EN BIT(7)
#define TPS_SAFETY_CHECK_CTRL_ENABLE_DRV BIT(5)
#define TPS_SAFETY_CHECK_CTRL_NO_ERROR BIT(2)
#define TPS_SAFETY_CHECK_CTRL_DIAG_EXIT_MASK BIT(1)
#define TPS_SAFETY_CHECK_CTRL_DIAG_EXIT BIT(0)
#define TPS_WR_SAFETY_CHECK_CTRL 0x93
#define TPS_RD_SAFETY_BIST_CTRL 0x3C
#define TPS_WR_SAFETY_BIST_CTRL 0x9F
#define TPS_SAFETY_BIST_CTRL_BIST_DEG_CNT(x) (((x) & 0x3) << 6)
#define TPS_SAFETY_BIST_CTRL_AUTO_BIST_DIS BIT(5)
#define TPS_SAFETY_BIST_CTRL_EE_CRC_CHK BIT(4)
#define TPS_SAFETY_BIST_CTRL_LBIST_EN BIT(2)
#define TPS_SAFETY_BIST_CTRL_ABIST_EN_2 BIT(1)
#define TPS_SAFETY_BIST_CTRL_ABIST_EN BIT(0)
#define TPS_RD_WD_WIN1_CFG 0x2E
#define TPS_WR_WD_WIN1_CFG 0xED
#define TPS_RD_WD_WIN2_CFG 0x05
#define TPS_WR_WD_WIN2_CFG 0x09
#define TPS_RD_WDT_TOKEN_VALUE 0x39
#define TPS_RD_WDT_TOKEN_VALUE_WDFAIL_TH BIT(7)
#define TPS_RD_WDT_TOKEN_VALUE_TOKEN(x) (((x) >> 0) & 0xF)
#define TPS_WR_WDT_ANSWER 0xE1
#define TPS_WR_WDT_ANSWER_WDT_ANSW_CNT(x) (((x) >> 6) & 0x3)
#define TPS_WR_WDT_ANSWER_TOKEN_ERR BIT(5)
#define TPS_WR_WDT_ANSWER_WD_WRONG_CFG BIT(4)
#define TPS_WR_WDT_ANSWER_WD_CFG_CHG BIT(3)
#define TPS_WR_WDT_ANSWER_SEQ_ERR BIT(2)
#define TPS_WR_WDT_ANSWER_TIME_OUT BIT(1)
#define TPS_WR_WDT_ANSWER_TOKEN_ERLY BIT(0)
#define TPS_RD_DEV_STAT 0x11
#define TPS_RD_DEV_STAT_IGN BIT(0)
#define TPS_RD_DEV_STAT_CANWU_L BIT(1)
#define TPS_RD_VMON_STAT_1 0x12
#define TPS_VMON_STAT_1_VBATP_OV BIT(7)
#define TPS_VMON_STAT_1_VBATP_UV BIT(6)
#define TPS_VMON_STAT_1_VCP17_OV BIT(5)
#define TPS_VMON_STAT_1_VCP12_OV BIT(4)
#define TPS_VMON_STAT_1_VCP12_UV BIT(3)
#define TPS_VMON_STAT_1_AVDD_VMON_ERR BIT(2)
#define TPS_VMON_STAT_1_BG_ERR2 BIT(1)
#define TPS_VMON_STAT_1_BG_ERR1 BIT(0)
#define TPS_RD_VMON_STAT_2 0xA6
#define TPS_VMON_STAT_2_VDD6_OV BIT(7)
#define TPS_VMON_STAT_2_VDD6_UV BIT(6)
#define TPS_VMON_STAT_2_VDD5_OV BIT(5)
#define TPS_VMON_STAT_2_VDD5_UV BIT(4)
#define TPS_VMON_STAT_2_VDD35_OV BIT(3)
#define TPS_VMON_STAT_2_VDD35_UV BIT(2)
#define TPS_VMON_STAT_2_VDD1_OV BIT(1)
#define TPS_VMON_STAT_2_VDD1_UV BIT(0)
#define TPS_RD_SENS_CTRL 0x56
#define TPS_WR_SENS_CTRL 0x7B
#define TPS_WR_SENS_CTRL_VDD5_EN BIT(4)
#define TPS_WR_SENS_CTRL_VSOUT1_EN BIT(0)

#define TPS_RD_SAFETY_FUNC_CFG 0x3A
#define TPS_WR_SAFETY_FUNC_CFG 0x35
#define TPS_SAFETY_FUNC_CFG_NO_SAFE_TO BIT(7)
#define TPS_SAFETY_FUNC_CFG_ERROR_CFG BIT(6)
#define TPS_SAFETY_FUNC_CFG_WD_CFG BIT(5)
#define TPS_SAFETY_FUNC_CFG_IGN_PWRL BIT(4)
#define TPS_SAFETY_FUNC_CFG_WD_RST_EN BIT(3)
#define TPS_SAFETY_FUNC_CFG_DIS_NRES_MON BI(2)
#define TPS_SAFETY_FUNC_CFG_VDD_3_5_SEL BIT(0)
#define TPS_RD_SAFE_CFG_CRC 0x5A
#define TPS_WR_SAFE_CFG_CRC 0x63
#define TPS_RD_DIAG_CFG_CTRL 0xDD
#define TPS_WR_DIAG_CFG_CTRL 0xCC
#define TPS_DIAG_CFG_CTRL_MUX_EN BIT(7)
#define TPS_DIAG_CFG_CTRL_SPI_SDO BIT(6)
#define TPS_DIAG_CFG_CTRL_MUX_OUT BIT(5)
#define TPS_DIAG_CFG_CTRL_INT_CON(x) (((x) & 0x7) << 2)
#define TPS_DIAG_CFG_CTRL_MUX_CFG(x) (((x) & 0x3) << 0)
#define TPS_RD_DIAG_MUX_SEL 0xAC
#define TPS_WR_DIAG_MUX_SEL 0xC9
#define TPS_RD_SAFETY_ERR_PWM_H 0xD7
#define TPS_WR_SAFETY_ERR_PWM_H 0xD8
#define TPS_RD_SAFETY_ERR_PWM_L 0x59
#define TPS_WR_SAFETY_ERR_PWM_L 0x7E
#define TPS_RD_WDT_TOKEN_FDBCK 0x78
#define TPS_WR_WDT_TOKEN_FDBCK 0x77
#define TPS_WDT_TOKEN_FDBCK_FDBK(x) (((x) & 0xF) << 4)
#define TPS_WDT_TOKEN_FDBCK_TOKEN_SEED(x) (((x) & 0xF) << 0)



struct tps65381 {
	struct spi_slave *slave;
};

static inline int32_t tps_write(struct tps65381 *tps, uint8_t cmd, uint8_t data, TickType_t waittime) {
	uint16_t wdata[] = {
		(uint16_t) cmd, 
		(uint16_t) data,
	};
	uint16_t rdata[] = {0, 0};
	int32_t ret = spiSlave_transver(tps->slave, wdata, rdata, 2, waittime);
	if (ret < 0) {
		return ret;
	}
	/* 
	 * CMD Invalid
	 */
	if (!TPS_STAT_CHECK(rdata[0])) {
		return -1;
	}
	return 0;
}

static inline int32_t tps_read(struct tps65381 *tps, uint8_t cmd, uint8_t *data, TickType_t waittime) {
	uint16_t wdata[] = {
		(uint16_t) cmd, 
		0xFF
	};
	uint16_t rdata[] = {0, 0};
	int32_t ret = spiSlave_transver(tps->slave, wdata, rdata, 2, waittime);
	if (ret < 0) {
		return ret;
	}
	/* 
	 * CMD Invalid
	 */
	if (!TPS_STAT_CHECK(rdata[0])) {
		return -1;
	}
	*data = (uint8_t) rdata[1];
	return 0;
}

struct tps65381 *tps_init(struct spi_slave *slave, TickType_t waittime) {
	int32_t ret;
	uint8_t tmp;
	struct tps65381 *tps = pvPortMalloc(sizeof(struct tps65381));
	if (tps == NULL) {
		goto tps_init_error0;
	}
	tps->slave = slave;
	ret = tps_read(tps, TPS_RD_DEV_ID, &tmp, waittime);
	if (ret < 0) {
		goto tps_init_error1;
	}
	if (tmp != TPS_RD_DEV_ID_DATA) {
		goto tps_init_error1;
	}
	ret = tps_read(tps, TPS_RD_DEV_REV, &tmp, waittime);
	if (ret < 0) {
		goto tps_init_error1;
	}
	if (tmp != TPS_RD_DEV_REV_DATA) {
		goto tps_init_error1;
	}
	return tps;
tps_init_error1:
	vPortFree(tps);
tps_init_error0:
	return NULL;
}
int32_t tps_deinit(struct tps65381 *tps) {
	vPortFree(tps);
	return 0;
}

int32_t tps_mux(struct tps65381 *tps, enum tps_diag diag, TickType_t waittime) {
	uint8_t tmp;
	int32_t ret;
	uint8_t mux = 0x0;
	
	/* 
	 * Activate Analog Mux
	 */
	tmp = TPS_DIAG_CFG_CTRL_MUX_EN | TPS_DIAG_CFG_CTRL_MUX_CFG(0x2);
	ret = tps_write(tps, TPS_WR_DIAG_CFG_CTRL, tmp, waittime);
	if (ret < 0) {
		return -1;
	}

	mux = 1 << ((uint8_t) diag);
	return tps_write(tps, TPS_WR_DIAG_MUX_SEL, mux, waittime);
}
float tps_diag(struct tps65381 *tps, enum tps_diag diag, struct adc *adc, TickType_t waittime) {
	int32_t ret;
	float val;
	ret = adc_get(adc, waittime);
	if (ret < 0) {
		return NAN;
	}
	val = (((float) ret) * (3.3 / 4096.));
	switch (diag) {
		case TPS_VDD5:
			val *= 2;
			break;
		case TPS_VDD6:
			val *= 3;
			break;
		case TPS_VCP:
			val *= 13.5;
			break;
		case TPS_VSOUT1:
			val *= 4;
			break;
		case TPS_VBAT_SAFING:
			val *= 10;
			break;
		case TPS_VBAT:
			val *= 10;
			break;
		default:
			break;
	}
	return val;
}

