/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2020
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
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
#include <system.h>

#define LTC6811_TIMEOUT (1000 / portTICK_PERIOD_MS) // TODO Calc max time

#ifdef CONFIG_LTC6811_CONVERSION_HZ_27kHz
# define LTC6811_CONVERSION_TIME 1
#endif
#ifdef CONFIG_LTC6811_CONVERSION_HZ_14kHz
# define LTC6811_CONVERSION_TIME 1
#endif
#ifdef CONFIG_LTC6811_CONVERSION_HZ_7kHz
# define LTC6811_CONVERSION_TIME 2
#endif
#ifdef CONFIG_LTC6811_CONVERSION_HZ_3kHz
# define LTC6811_CONVERSION_TIME 3
#endif
#ifdef CONFIG_LTC6811_CONVERSION_HZ_2kHz
# define LTC6811_CONVERSION_TIME 4
#endif
#ifdef CONFIG_LTC6811_CONVERSION_HZ_1kHz
# define LTC6811_CONVERSION_TIME 7
#endif
#ifdef CONFIG_LTC6811_CONVERSION_HZ_422Hz
# define LTC6811_CONVERSION_TIME 12
#endif
#ifdef CONFIG_LTC6811_CONVERSION_HZ_26Hz
# define LTC6811_CONVERSION_TIME 201
#endif

static void ltc6811_adcTask(void *data);

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
	ltc->adcIsRunning = false;
	ret = OS_CREATE_TASK(ltc6811_adcTask, "LTC ADC Task", 512, ltc, CONFIG_LTC6811_PRIO, ltc->adcTask);
	if (ret != pdPASS) {
		PRINTF("Can't create LTC6811 ADC Task\n");
		goto ltc6811_init_error0;
	}
	PRINTF("LTC6811 Init wait for connect...\n");
ltc6811_init_error0:
	return NULL;
}

int32_t ltc6811_connect(struct ltc6811 *ltc, struct spi *spi, uint8_t cs, uint16_t gpio, uint32_t bautrate) {
	int32_t ret;
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
	{
		uint16_t data[ltc->numberOfSlaves];
		memset(data, 0xFF, ltc->numberOfSlaves * sizeof(uint16_t));
		/* Wakeup all slaves */
		ret = spiSlave_send(ltc->spi, data, ltc->numberOfSlaves, LTC6811_TIMEOUT);
		if (ret < 0) {
			goto ltc6811_connect_error0;
		}
	}
	{
		uint8_t registerContent[6];
		registerContent[0] = 0x0;
#ifdef CONFIG_LTC6811_GPIO1_PULLUP
		registerContent[0] |= LTC_CMD_WRCFGA_0_GPIO_PULLUP(0);
#endif
#ifdef CONFIG_LTC6811_GPIO2_PULLUP
		registerContent[0] |= LTC_CMD_WRCFGA_0_GPIO_PULLUP(1);
#endif
#ifdef CONFIG_LTC6811_GPIO3_PULLUP
		registerContent[0] |= LTC_CMD_WRCFGA_0_GPIO_PULLUP(2);
#endif
#ifdef CONFIG_LTC6811_GPIO4_PULLUP
		registerContent[0] |= LTC_CMD_WRCFGA_0_GPIO_PULLUP(3);
#endif
#ifdef CONFIG_LTC6811_GPIO5_PULLUP
		registerContent[0] |= LTC_CMD_WRCFGA_0_GPIO_PULLUP(4);
#endif
#ifdef CONFIG_LTC6811_REFON
		registerContent[0] |= LTC_CMD_WRCFGA_0_REFON;
#endif
		//registerContent[0] |= LTC_CMD_WRCFGA_0_DTEN; // Read-Only
#if defined(CONFIG_LTC6811_CONVERSION_HZ_14kHz) || defined(CONFIG_LTC6811_CONVERSION_HZ_3kHz) || defined(CONFIG_LTC6811_CONVERSION_HZ_1kHz) || defined(CONFIG_LTC6811_CONVERSION_HZ_2kHz)
		/* if 14kHz, 3kHz, 1kHz or 2kHz -> 1 else 0 */
		registerContent[0] |= LTC_CMD_WRCFGA_0_ADCOPT;
#endif
		registerContent[1] = (ltc->cellUnderVoltage & 0xFF);
		registerContent[2] = ((ltc->cellUnderVoltage >> 8) & 0xF) | ((ltc->cellOverVoltage & 0xF) << 4);
		registerContent[3] = ((ltc->cellOverVoltage >> 4) & 0xFF);
		registerContent[4] = 0x0;
		/* TODO Move to Kconfig */
		registerContent[5] = LTC_CMD_WRCFGA_5_DCTO(0);
		/* Select All */
		ltc->selectSlaves = ltc->allSlavesMask;
		/* old Value not used by this */
		ret = ltc6811_writeRegister(ltc, LTC_CMD_WRCFGA, registerContent, NULL);
		if (ret < 0) {
			goto ltc6811_connect_error1;
		}
	}
	return 0;
ltc6811_connect_error1:
	spiSlave_deinit(ltc->spi);
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

int32_t ltc6811_write(struct ltc6811 *ltc, ltc_cmd_t cmd, uint8_t *data, uint32_t dataLen) {
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

int32_t ltc6811_read(struct ltc6811 *ltc, ltc_cmd_t cmd, uint8_t *data, uint32_t lenData) {
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

int32_t ltc6811_writeRegister(struct ltc6811 *ltc, ltc_cmd_t cmd, uint8_t *newData, uint8_t *oldData) {
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

int32_t ltc6811_readRegister(struct ltc6811 *ltc, ltc_cmd_t cmd, uint8_t *registerContent) {
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

int32_t ltc6811_setADCCallback(struct ltc6811 *ltc, bool (*callback)(struct ltc6811 *ltc, void *data), void *data) {
	ltc->callbackData = data;
	ltc->callback = callback;
	return 0;
}

int32_t ltc6811_startADC(struct ltc6811 *ltc) {
	ltc->adcIsRunning = true;
	vTaskResume(ltc->adcTask);
	return 0;
}

int32_t ltc6811_stopADC(struct ltc6811 *ltc) {
	ltc->adcIsRunning = false;
	return 0;
}

/**
 * This Task collect automatically all ADC Data from all Slaves 
 */
static void ltc6811_adcTask(void *data) {
	int32_t ret;
	struct ltc6811 *ltc = data;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	for (;;) {
		/* if we not running go to sleep */
		if (!ltc->adcIsRunning) {
			vTaskSuspend(NULL);
			xLastWakeTime = xTaskGetTickCount();
		}
		/* get Master Lock */
		ret = hal_lock(ltc, 100 / portTICK_PERIOD_MS);
		/* if lock failed something went wrong */
		CONFIG_ASSERT(ret == 0);
		/* select all Slaves  */
		ltc->selectSlaves = ltc->allSlavesMask;
		/* Clear Cells */
		ret = ltc6811_write(ltc, LTC_CMD_CLRCELL, NULL, 0);
		if (ret != 0) {
			PRINTF("Can't clear Cells\n");
			CONFIG_ASSERT(ret == 0);
		}
		/* Start Cell Voltage ADC Conversion and Poll Status */
		/* in Normal Mode, All Cells, Discharges Permitted  */
		ret = ltc6811_write(ltc, LTC_CMD_ADCV(0, false, 0x2), NULL, 0);
		if (ret != 0) {
			PRINTF("Can't clear Cells\n");
			CONFIG_ASSERT(ret == 0);
		}
		{
			int i;
			bool finished = false;
			uint8_t adc_state[ltc->numberOfSlaves];
			/* should be correct, wait time is calculated by datasheet */
			/* TODO release lock for LTC if possible? */
			/* TODO Check LTC can be preformed action wile ADC is performing */
			vTaskDelay(LTC6811_CONVERSION_TIME / portTICK_PERIOD_MS);
			do {
				ret = ltc6811_read(ltc, LTC_CMD_PLADC, adc_state, ltc->numberOfSlaves);
				if (ret < 0) {
					PRINTF("Can't read ADC Conversion Status\n");
					CONFIG_ASSERT(ret == 0);
				}
				for (i = 0; i < ltc->numberOfSlaves; i++) {
					/* if one slave is not finished break and try again */
					if (adc_state[0] != 0) {
						finished = false;
						break;
					} else {
						/* least one slave is finish check the others */
						finished = true;
					}
				}
				if (!finished) {
					/* Ok Wait ones more */
					vTaskDelay(1 / portTICK_PERIOD_MS);
				}
			} while(!finished);
		}
		{
			int i, j;
			ltc_cmd_t cmd[4] = {LTC_CMD_RDCVA, LTC_CMD_RDCVB, LTC_CMD_RDCVC, LTC_CMD_RDCVD};
			uint8_t *registerContent = alloca(ltc->numberOfSlaves * 6);
			for(i = 0; i < ARRAY_SIZE(cmd); i++) {
				/* Read Cell Voltage PEC is checked automatically */
				ret = ltc6811_readRegister(ltc, cmd[i], registerContent);
				if (ret < 0) {
					PRINTF("Can't read Cell Voltage Register Group A\n");

					CONFIG_ASSERT(ret == 0);
				}
				for (j = 0; j < ltc->numberOfSlaves; j++) {
					/* Current ADC base */
					uint32_t ab = (i * 3);
					/* Current Register Base */
					uint32_t rb = (j * 6);
					/* Decode all values and write it to adc */
					ltc->slaves[j]->adcs[0 + ab]->value = (((uint16_t) registerContent[1 + rb]) << 8) | ((uint16_t) registerContent[0 + rb]);
					ltc->slaves[j]->adcs[1 + ab]->value = (((uint16_t) registerContent[3 + rb]) << 8) | ((uint16_t) registerContent[2 + rb]);
					ltc->slaves[j]->adcs[2 + ab]->value = (((uint16_t) registerContent[5 + rb]) << 8) | ((uint16_t) registerContent[4 + rb]);
				}
			}

		}

		hal_unlock(ltc);
		CONFIG_ASSERT(ret == 0);
		/* release Master Lock */

		if (ltc->callback) {
			/* call callback and Notify User if callback is set */
			ltc->callback(ltc, ltc->callbackData);
		}
 		vTaskDelayUntil(&xLastWakeTime, CONFIG_LTC6811_FREQ_MS / portTICK_PERIOD_MS);
	}
}

ADC_INIT(ltc6811, index, bits, hz) {
	struct adc_ltc6811 *adc;
	int32_t ret;
	adc = (struct adc_ltc6811 *) ADC_GET_DEV(index);
	ret = adc_generic_init((struct adc *) adc);
	if (ret < 0) {
		goto ltc_adc_init_error0;
	}
	if (ret > 0) {
		goto ltc_adc_init_exit;
	}
	/* set to max value */
	adc->value = 0xFFFF;
	/* ignore bits and hz defined over ltc6811 */
	(void) bits;
	(void) hz;
ltc_adc_init_exit:
	return (struct adc *) adc;
ltc_adc_init_error0:
	return NULL;
}
ADC_DEINIT(ltc6811, a) {
	struct adc_ltc6811 *adc = (struct adc_ltc6811 *) a;
	adc->gen.init = false;
	return 0;
}
ADC_GET(ltc6811, a, waittime) {
	uint16_t value;
	struct adc_ltc6811 *adc = (struct adc_ltc6811 *) a;
	struct ltc6811 *ltc = adc->ltc->master;
	int32_t ret;
	/* get ltc lock, if locked ADC is running for all slaves */
	ret = hal_lock(ltc, waittime);
	if (ret != 0) {
		return -1;
	}
	value = adc->value;
	ret = hal_unlock(ltc);
	if (ret != 0) {
		return -1;
	}
	return value;
}
ADC_GET_ISR(ltc6811, a) {
	uint16_t value;
	struct adc_ltc6811 *adc = (struct adc_ltc6811 *) a;
	/* WARING: unsave! it is possible ADC is running, can't prevent this in ISR */
	value = adc->value;
	return value;
}
ADC_SET_CALLBACK(ltc6811, a, callback, data) {
	/* Not Supported see ltc6811_setCallback */
	return -1;
}
ADC_START(ltc6811, a) {
	/* Not Supported see ltc6811_startADC */
	return -1;
}
ADC_STOP(ltc6811, a) {
	/* Not Supported see ltc6811_stopADC */
	return -1;
}
ADC_OPS(ltc6811);
#ifdef CONFIG_LTC6811_I2C
I2C_INIT(ltc6811, index, mode) {
	struct i2c_ltc6811 *i2c;
	int32_t ret;
	i2c = (struct i2c_ltc6811 *) ADC_GET_DEV(index);
	ret = i2c_genericInit((struct i2c *) i2c);
	if (ret < 0) {
		goto ltc_i2c_init_error0;
	}
	if (ret > 0) {
		goto ltc_i2c_init_exit;
	}
ltc_i2c_init_exit:
	return (struct i2c *) i2c;
ltc_i2c_init_error0:
	return NULL;
}
I2C_DEINIT(ltc6811, i) {
	return -1;
}
I2C_SEND(ltc6811, i, id, data, len, waittime) {
	return -1;
}
I2C_RECV(ltc6811, i, id, data, len, waittime) {
	return -1;
}
I2C_TRANSVER(ltc6811, i, id, sendData, sendLen, recvData, recvLen, waittime) {
	return -1;
}
I2C_SEND_ISR(ltc6811, i, id, data, len) {
	return -1;
}
I2C_RECV_ISR(ltc6811, i, id, data, len) {
	return -1;
}
I2C_TRANSVER_ISR(ltc6811, i, id, sendData, sendLen, recvData, recvLen) {
	return -1;
}
I2C_OPS(ltc6811);
#endif
