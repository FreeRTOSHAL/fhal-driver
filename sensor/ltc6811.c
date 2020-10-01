/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2020
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <spi.h>
#include <hal.h>
#include <adc.h>
#define ADC_PRV
#include <adc_prv.h>
#include <i2c.h>
#define I2C_PRV
#include <i2c_prv.h>
#include <ltc6811.h>
#define LTC6811_PRV
#include <ltc6811_prv.h>
#include <alloca.h>

#define LTC6811_TIMEOUT (1000 / portTICK_PERIOD_MS) // TODO Calc max time

struct ltc6811 *ltc6811_init(uint32_t index) {
	int32_t ret;
	struct ltc6811 *ltc = (struct ltc6811 *) HAL_GET_GLOBAL_DEV(index);
	if (ltc->gen.init) {
		return ltc;
	}
	ret = hal_init(ltc);
	if (ret < 0) {
		goto ltc6811_init_error0;
	}
	ltc->index = index;
	ltc->gen.init = true;
	PRINTF("LTC6811 Init wait for connect...\n");
ltc6811_init_error0:
	return NULL;
}

int32_t ltc6811_connect(struct ltc6811 *ltc, struct spi *spi, uint8_t cs, uint16_t gpio, uint32_t bautrate) {
	/* Allredy connected */ 
	if (ltc->spi != NULL) {
		goto ltc6811_connect_error0;
	}
	{
		struct spi_opt opt = {
			.lsb = false,
			.cpol = true,
			.cpha = true,
			.cs = cs, 
			.csLowInactive = false,
			.gpio = gpio,
			.size = 8,
			.wdelay = 0, /* t8: 60 ns */
			.cs_hold = 1, /* t6: 0.65 us */
			.cs_delay = 1, /* t7: 1s */
			.bautrate = bautrate,
		};
		ltc->spi = spiSlave_init(spi, &opt);
		if (ltc->spi == NULL) {
			goto ltc6811_connect_error0;
		}
	}
	return 0;
ltc6811_connect_error0:
	return -1;
}

static int32_t ltc6811_connectSlave(struct ltc6811 *ltc, struct ltc6811_slave *slave) {
	int i;
	for (i = 0; i < ltc->numberOfSlaves; i++) {
		if (ltc->slaves[i] == NULL) {
			break;
		}
	}
	if (i == ltc->numberOfSlaves) {
		return -1;
	}
	ltc->slaves[i] = slave;
	return 0;
}

struct ltc6811_slave *ltc6811_slave_init(uint32_t index) {
	int32_t ret;
	struct ltc6811_slave *ltc = (struct ltc6811_slave *) HAL_GET_GLOBAL_DEV(index);
	if (ltc->gen.init) {
		return ltc;
	}
	ret = hal_init(ltc);
	if (ret < 0) {
		goto ltc6811_slave_init_error0;
	}
	ltc->index = index;
	ltc->gen.init = true;
	PRINTF("LTC6811 Slave Init\n");
	ret = ltc6811_connectSlave(ltc->master, ltc);
	if (ret < 0) {
		goto ltc6811_slave_init_error0;
	}
	return ltc;
ltc6811_slave_init_error0:
	return NULL;
}

int32_t ltc6811_write(struct ltc6811 *ltc, enum ltc_cmd cmd, uint8_t *data, uint32_t dataLen) {
	uint32_t len = 2 /* Command */ + 2 /* Command PEC */ + dataLen /* Data inclusive PEC! */;
	uint16_t *sendData = alloca(len);
	uint16_t *recvData = alloca(len);
	uint8_t tmp[2];
	uint16_t pec;
	int32_t ret;
	int i;
	memset(sendData, 0xFF, len);
	tmp[0] = (cmd >> 8) & 0xFF;
	tmp[1] = (cmd >> 0) & 0xFF;
	pec = ltc6811_calcPEC(tmp, 2);
	sendData[0] = tmp[0];
	sendData[1] = tmp[1];
	sendData[2] = (pec >> 8) & 0xFF;
	sendData[3] = (pec >> 0) & 0xFF;
	for (i = 0; i < dataLen; i++) {
		sendData[4 + i] = data[i];
	}
	ret = spiSlave_transver(ltc->spi, sendData, recvData, len, LTC6811_TIMEOUT);
	if (ret < 0) {
		return -1;
	}

	return 0;
}

int32_t ltc6811_read(struct ltc6811 *ltc, enum ltc_cmd cmd, uint8_t *data, uint32_t lenData) {
	uint32_t len = 2 /* Command */ + 2 /* Command PEC */ + lenData /* Data inclusive PEC! */;
	uint16_t *sendData = alloca(len);
	uint16_t *recvData = alloca(len);
	uint8_t tmp[2];
	uint16_t pec;
	int32_t ret;
	int i;
	memset(sendData, 0xFF, len);
	tmp[0] = (cmd >> 8) & 0xFF;
	tmp[1] = (cmd >> 0) & 0xFF;
	pec = ltc6811_calcPEC(tmp, 2);
	sendData[0] = tmp[0];
	sendData[1] = tmp[1];
	sendData[2] = (pec >> 8) & 0xFF;
	sendData[3] = (pec >> 0) & 0xFF;
	ret = spiSlave_transver(ltc->spi, sendData, recvData, len, LTC6811_TIMEOUT);
	if (ret < 0) {
		return -1;
	}
	for (i = 0; i < lenData; i++) {
		data[i] = recvData[4 + i];
	}

	return 0;
}

int32_t ltc6811_writeRegister(struct ltc6811 *ltc, enum ltc_cmd cmd, uint8_t *newData, uint8_t *oldData) {
	int i;
	int16_t pec;
	uint8_t *data = alloca(ltc->numberOfSlaves * 8);
	for (i = 0; i < ltc->numberOfSlaves; i++) {
		/* the first data is used by the last slave in the chain */
		/* check if salve is seleced write new value */
		if ((ltc->selectSlaves >> (ltc->numberOfSlaves - 1 - i)) & 0x1) {
			data[(i * 8) + 0] = newData[0];
			data[(i * 8) + 1] = newData[1];
			data[(i * 8) + 2] = newData[2];
			data[(i * 8) + 3] = newData[3];
			data[(i * 8) + 4] = newData[4];
			data[(i * 8) + 5] = newData[5];
		} else {
			data[(i * 8) + 0] = oldData[0];
			data[(i * 8) + 1] = oldData[1];
			data[(i * 8) + 2] = oldData[2];
			data[(i * 8) + 3] = oldData[3];
			data[(i * 8) + 4] = oldData[4];
			data[(i * 8) + 5] = oldData[5];
		}
		/* Calc PEC */
		pec = ltc6811_calcPEC(&data[i*6], 6);
		data[(i * 8) + 6] = (pec >> 8) & 0xFF;
		data[(i * 8) + 7] = (pec >> 8) & 0xFF;
	}
	return ltc6811_write(ltc, cmd, data, ltc->numberOfSlaves * 8);
}

int32_t ltc6811_readRegister(struct ltc6811 *ltc, enum ltc_cmd cmd, uint8_t *registerContent) {
	int i;
	int j;
	int16_t pec;
	int16_t orgPec;
	uint8_t *data = alloca(ltc->numberOfSlaves * 8);
	int32_t ret;
	ret = ltc6811_read(ltc, cmd, data, ltc->numberOfSlaves * 8);
	if (ret < 0) {
		PRINTF("CMD: 0x%x: Read error\n", cmd);
		return -1;
	}
	for (i = ltc->numberOfSlaves - 1, j = 0; i >= 0; i++, j++) {
		orgPec = ltc6811_calcPEC(&data[i * 8], 6);
		pec = ((data[(i * 8) + 6] << 8) & data[(i * 8) + 7]);
		if (orgPec != pec) {
			PRINTF("CMD: 0x%x, slave: %d: PEC is wrong (%u == %u)\n", cmd, i, orgPec, pec);
			return -2;
		}
		registerContent[(j * 6) + 0] = data[(i * 8) + 0];
		registerContent[(j * 6) + 1] = data[(i * 8) + 1];
		registerContent[(j * 6) + 2] = data[(i * 8) + 2];
		registerContent[(j * 6) + 3] = data[(i * 8) + 3];
		registerContent[(j * 6) + 4] = data[(i * 8) + 4];
		registerContent[(j * 6) + 5] = data[(i * 8) + 5];
	}
	return 0;
}
ADC_INIT(ns, index, bits, hz) {
	return NULL;
}
ADC_DEINIT(ns, a) {
	return -1;
}
ADC_GET(ns, a, waittime) {
	return -1;
}
ADC_GET_ISR(ns, a) {
	return -1;
}
ADC_SET_CALLBACK(ns, a, callback, data) {
	return -1;
}
ADC_START(ns, a) {
	return -1;
}
ADC_STOP(ns, a) {
	return -1;
}
ADC_OPS(ltc6811);
