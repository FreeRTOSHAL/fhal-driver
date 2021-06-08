#include <adc.h>
#define ADC_PRV
#include <adc_prv.h>
#include <spi.h>
#include <mcp320x.h>
#include <FreeRTOS.h>
#include <task.h>
#ifdef CONFIG_MCP320X_DEBUG
# define PRINTF(fmt, ...) printf("MCP320X: " fmt, ##__VA_ARGS__)
#else
# define PRINTF(fmt, ...)
#endif
/* ISR Emulation Thread */
static void mcp320x_task(void *data) {
	TickType_t oldtime;
	struct adc_mcp320x *adc = data;
	for (;;) {
		int32_t ret;
		oldtime = xTaskGetTickCount();
		if (!adc->running) {
#ifdef CONFIG_INCLUDE_vTaskSuspend
			vTaskSuspend(NULL);
#else
			vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
			if (!adc->adcc->slave) {
				adc->running = false;
			}
			continue;
		}
		ret = adc_get((struct adc *) adc, adc->ticks);
		CONFIG_ASSERT(ret >= 0);
		/* return not needed Scheduler is running and scheduled higher prio Task */
		if (adc->callback) {
			(void) adc->callback((struct adc *) adc, adc->channelID, ret, adc->data);
		} else {
			adc_stop((struct adc * ) adc);
		}
		vTaskDelayUntil(&oldtime, adc->ticks);
	}
}

void *mcp320x_contoller_init(uint32_t index) {
	int32_t ret;
	struct adc_mcp320x_contoller *adcc;
	adcc = (struct adc_mcp320x_contoller *) HAL_GET_DEV(hal, index);
	if (adcc == NULL) {
		return NULL;
	}
	if (adcc->gen.init) {
		return adcc;
	}
	ret = hal_init(adcc);
	if (ret < 0) {
		return NULL;
	}
	adcc->gen.init = true;
	adcc->slave = NULL;
	return adcc;
}

ADC_INIT(mcp320x, index, bits, hz) {
	struct adc_mcp320x *adc;
	int32_t ret;
	adc = (struct adc_mcp320x *) ADC_GET_DEV(index);
	if (adc == NULL) {
		goto mcp320x_adc_init_error0;
	}
	ret = adc_generic_init((struct adc *) adc);
	if (ret < 0) {
		goto mcp320x_adc_init_error0;
	}
	if (ret > 0) {
		goto mcp320x_adc_init_exit;
	}
	if (!adc->adcc->gen.init) {
		goto mcp320x_adc_init_error0;
	}
	adc->running = false;
	/* waittime per ticks */
	adc->ticks = (1000 * portTICK_PERIOD_MS) / hz;
	adc->bits = bits;
mcp320x_adc_init_exit:
	return (struct adc *) adc;
mcp320x_adc_init_error0:
	return NULL;
}
int32_t mcp320x_connect(void *ac, struct spi *spi, uint8_t cs, uint16_t gpio, uint32_t baudrate) {
	struct adc_mcp320x_contoller *adcc = (struct adc_mcp320x_contoller *) ac;
	struct spi_opt options = {
		.lsb = false,
		.cpol = false,
		.cpha = false,
		.cs = cs,
		.csLowInactive = false, 
		.gpio = gpio, 
		.size = 8,
		.wdelay = 5, /* min 0.0050 us */
		.cs_hold = 50, /* min 0.0500 us */
		.cs_delay = 10, /* min 0.0100 us */
		.bautrate = baudrate==0?1000000:baudrate, /* 1 MHz at VDD: 2.7V and 2 MHz at VDD: 5 V */
	};
	adcc->slave = spiSlave_init(spi, &options);
	if (!adcc->slave) {
		return -1;
	}
	return 0;
}
ADC_DEINIT(mcp320x, a) {
	struct adc_mcp320x *adc = (struct adc_mcp320x *) a;
	adc->gen.init = false;
	return -1;
}
ADC_GET(mcp320x, a, waittime) {
	struct adc_mcp320x *adc = (struct adc_mcp320x *) a;
	uint16_t send[3] = {
		/* start bit */
		BIT(2) | 
		/* channel 8 - 15 are diff channel, diff value is inverted */
		((((~adc->channelID) >> 3) & 0x1) << 1) |
		((adc->channelID >> 2) & 0x1),
		((adc->channelID & 0x3) << 6),
	};
	uint16_t value[3];
	int32_t ret;
	adc_lock(a, waittime, -1);
	HAL_LOCK(adc->adcc, waittime, -1);
	PRINTF("Send: 0x%x 0x%x 0x%x\n", send[0], send[1], send[2]);
	ret = spiSlave_transver(adc->adcc->slave, send, value, 3, waittime);
	if (ret < 0) {
		HAL_UNLOCK(adc->adcc, -1);
		adc_unlock(a, -1);
		return -1;
	}
	PRINTF("Recv: 0x%x 0x%x 0x%x\n", value[0], value[1], value[2]);
	HAL_UNLOCK(adc->adcc, -1);
	adc_unlock(a, -1);
	/* Bit 5 must 0x0 else error! */
	if (((value[1] >> 4) & 0x1) != 0x0) {
		return -1;
	}
	PRINTF("Value: 0x%x\n", ((value[1] & 0xF) << 8) | value[2]);
	return ((value[1] & 0xF) << 8) | value[2];
}
ADC_GET_ISR(mcp320x, a) {
	struct adc_mcp320x *adc = (struct adc_mcp320x *) a;
	uint16_t send[3] = {
		/* start bit */
		BIT(2) | 
		/* channel 8 - 15 are diff channel, diff value is inverted */
		((((~adc->channelID) >> 3) & 0x1) << 1) |
		((adc->channelID >> 2) & 0x1),
		((adc->channelID & 0x3) << 6),
	};
	uint16_t value[3];
	int32_t ret;
	PRINTF("Send: 0x%x 0x%x 0x%x\n", send[0], send[1], send[2]);
	ret = spiSlave_transverISR(adc->adcc->slave, send, value, 3);
	if (ret < 0) {
		return -1;
	}
	PRINTF("Recv: 0x%x 0x%x 0x%x\n", value[0], value[1], value[2]);
	/* Bit 5 must 0x0 else error! */
	if (((value[1] >> 4) & 0x1) != 0x0) {
		return -1;
	}
	PRINTF("Value: 0x%x\n", ((value[1] & 0xF) << 8) | value[2]);
	return ((value[1] & 0xF) << 8) | value[2];
}
ADC_SET_CALLBACK(mcp320x, a, callback, data) {
	struct adc_mcp320x *adc = (struct adc_mcp320x *) a;
	int32_t ret;
	if (callback == NULL) {
		adc_stop((struct adc *) adc);
	}
	adc->callback = callback;
	adc->data = data;
	if (adc->task == NULL) {
		ret = OS_CREATE_TASK(mcp320x_task, "MCP320x Task", 100, adc, CONFIG_MCP320X_PRIO, adc->task);
		if (ret != pdPASS) {
			return -1;
		}
	}
	return 0;
}
ADC_START(mcp320x, a) {
	struct adc_mcp320x *adc = (struct adc_mcp320x *) a;
	if (!adc->adcc->slave) {
		return -1;
	}
	adc->running = true;
#ifdef CONFIG_INCLUDE_vTaskSuspend
	vTaskResume(adc->task);
#endif
	return 0;
}
ADC_STOP(mcp320x, a) {
	struct adc_mcp320x *adc = (struct adc_mcp320x *) a;
	adc->running = false;
	return 0;
}
ADC_OPS(mcp320x);

