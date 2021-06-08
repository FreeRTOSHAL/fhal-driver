#include <dac.h>
#define DAC_PRV
#include <dac_prv.h>
#include <spi.h>
#include <mcp492x.h>
#include <FreeRTOS.h>
#include <task.h>
#include <gpio.h>
#include <devs.h>
#ifdef CONFIG_MCP492X_DEBUG
# define PRINTF(fmt, ...) printf("MCP492X: " fmt, ##__VA_ARGS__)
#else
# define PRINTF(fmt, ...)
#endif
/* ISR Emulation Thread */
void *mcp492x_controller_init(uint32_t index) {
	int32_t ret;
	struct dac_mcp492x_controller *dacc;
	dacc = (struct dac_mcp492x_controller *) HAL_GET_DEV(hal, index);
	if (dacc == NULL) {
		return NULL;
	}
	if (dacc->gen.init) {
		return dacc;
	}
	ret = hal_init(dacc);
	if (ret < 0) {
		return NULL;
	}
	dacc->gen.init = true;
	dacc->slave = NULL;
	return dacc;
}

DAC_INIT(mcp492x, index, bits) {
	struct dac_mcp492x *dac;
	int32_t ret;
	dac = (struct dac_mcp492x *) DAC_GET_DEV(index);
	if (dac == NULL) {
		goto mcp492x_dac_init_error0;
	}
	ret = dac_generic_init((struct dac *) dac);
	if (ret < 0) {
		goto mcp492x_dac_init_error0;
	}
	if (ret > 0) {
		goto mcp492x_dac_init_exit;
	}
	if (!dac->dacc->gen.init) {
		goto mcp492x_dac_init_error0;
	}
	/* waittime per ticks */
	dac->bits = bits;
mcp492x_dac_init_exit:
	return (struct dac *) dac;
mcp492x_dac_init_error0:
	return NULL;
}
int32_t mcp492x_connect(void *ac, struct spi *spi, uint8_t cs, uint16_t gpio, uint16_t lvdac, uint32_t baudrate) {
	struct dac_mcp492x_controller *dacc = (struct dac_mcp492x_controller *) ac;
	struct spi_opt options = {
		.lsb = false,
		.cpol = false,
		.cpha = false,
		.cs = cs,
		.csLowInactive = false, 
		.gpio = gpio, 
		.size = 8,
		.wdelay = 5, /* min 0.0050 us */
		.cs_hold = 40, /* min 0.0400 us */
		.cs_delay = 15, /* min 0.0150 us */
		.bautrate = baudrate==0?10000000:baudrate, /* 10 MHz maximum */
	};
	dacc->slave = spiSlave_init(spi, &options);
	if (!dacc->slave) {
		return -1;
	}

	struct gpio *ggpio = gpio_init(GPIO_ID);
	dacc->lvdac = gpioPin_init(ggpio, lvdac, GPIO_OUTPUT, GPIO_PULL_UP);
	gpioPin_setPin(dacc->lvdac);
	return 0;
}
DAC_DEINIT(mcp492x, a) {
	struct dac_mcp492x *dac = (struct dac_mcp492x *) a;
	dac->gen.init = false;
	return -1;
}
DAC_SET(mcp492x, a, d, waittime) {
	struct dac_mcp492x *dac = (struct dac_mcp492x *) a;
	uint16_t send[2] = {
		/* channel bit */
		(dac->channelID?BIT(7):0) | 
		/* buffer vref, 1x gain, no SHDN */
		BIT(6) | BIT(5) | BIT(4) |
		((d >> 8) & 0xF),
		d & 0xFF
	};
	int32_t ret;
	dac_lock(a, waittime, -1);
	HAL_LOCK(dac->dacc, waittime, -1);
	PRINTF("Send: 0x%x 0x%x\n", send[0], send[1]);
	ret = spiSlave_send(dac->dacc->slave, send, 2, waittime);
	/* TODO: wait 100ns minimum */
	gpioPin_clearPin(dac->dacc->lvdac);
	uint32_t i;
	for(i=0; i<3 ;i++) {
		__asm volatile ("nop");
	}
	gpioPin_setPin(dac->dacc->lvdac);
	if (ret < 0) {
		HAL_UNLOCK(dac->dacc, -1);
		dac_unlock(a, -1);
		return -1;
	}
	HAL_UNLOCK(dac->dacc, -1);
	dac_unlock(a, -1);
	return 0;
}
DAC_SET_ISR(mcp492x, a, d) {
	struct dac_mcp492x *dac = (struct dac_mcp492x *) a;
	uint16_t send[2] = {
		/* channel bit */
		(dac->channelID?BIT(7):0) | 
		/* buffer vref, 1x gain, no SHDN */
		BIT(6) | BIT(5) | BIT(4) |
		((d >> 8) & 0xF),
		d & 0xFF
	};
	int32_t ret;
	PRINTF("Send: 0x%x 0x%x 0x%x\n", send[0], send[1]);
	ret = spiSlave_sendISR(dac->dacc->slave, send, 2);
	/* TODO: wait 100ns minimum */
	gpioPin_clearPin(dac->dacc->lvdac);
	uint32_t i;
	for(i=0; i<3 ;i++) {
		__asm volatile ("nop");
	}
	gpioPin_setPin(dac->dacc->lvdac);
	if (ret < 0) {
		return -1;
	}
	return 0;
}
DAC_OPS(mcp492x);

