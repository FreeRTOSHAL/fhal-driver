#include <adc.h>
#define ADC_PRV
#include <adc_prv.h>
#include <spi.h>
#include <adcs747x.h>
#include <FreeRTOS.h>
#include <task.h>
/* ISR Emulation Thread */
static void adcs747x_task(void *data) {
	TickType_t oldtime;
	struct adc_adcs747x *adc = data;
	for (;;) {
		int32_t ret;
		oldtime = xTaskGetTickCount();
		if (!adc->running) {
#ifdef CONFIG_INCLUDE_vTaskSuspend
			vTaskSuspend(NULL);
#else
			vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
			if (!adc->slave) {
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

ADC_INIT(adcs747x, index, bits, hz) {
	struct adc_adcs747x *adc;
	int32_t ret;
	adc = (struct adc_adcs747x *) ADC_GET_DEV(index);
	if (adc == NULL) {
		goto adcs747x_adc_init_error0;
	}
	ret = adc_generic_init((struct adc *) adc);
	if (ret < 0) {
		goto adcs747x_adc_init_error0;
	}
	if (ret > 0) {
		goto adcs747x_adc_init_exit;
	}
	adc->running = false;
	/* waittime per ticks */
	adc->ticks = (1000 * portTICK_PERIOD_MS) / hz;
	adc->bits = bits;
	adc->slave = NULL;
	ret = OS_CREATE_TASK(adcs747x_task, "ADC747x Task", 500, adc, CONFIG_ADCS747X_PRIO, adc->task);
	if (ret != pdPASS) {
		goto adcs747x_adc_init_error0;
	}
adcs747x_adc_init_exit:
	return (struct adc *) adc;
adcs747x_adc_init_error0:
	return NULL;
}
int32_t adcs747x_connect(struct adc *a, struct spi *spi, uint8_t cs, uint16_t gpio) {
	struct adc_adcs747x *adc = (struct adc_adcs747x *) a;
	struct spi_opt options = {
		.lsb = false,
		.cpol = false,
		.cpha = false,
		.cs = cs,
		.csLowInactive = false, 
		.gpio = gpio, 
		.size = 16,
		.wdelay = 50,
		.cs_hold = 50,
		.cs_delay = 10,
		.bautrate = 1000000,
	};
	adc->slave = spiSlave_init(spi, &options);
	if (!adc->slave) {
		return -1;
	}
	return 0;
}
ADC_DEINIT(adcs747x, a) {
	struct adc_adcs747x *adc = (struct adc_adcs747x *) a;
	adc->gen.init = false;
	return -1;
}
ADC_GET(adcs747x, a, waittime) {
	struct adc_adcs747x *adc = (struct adc_adcs747x *) a;
	uint16_t value;
	int32_t ret;
	adc_lock(a, waittime, -1);
	ret = spiSlave_recv(adc->slave, &value, 1, waittime);
	if (ret < 0) {
		adc_unlock(a, -1);
		return -1;
	}
	adc_unlock(a, -1);
	return value;
}
ADC_GET_ISR(adcs747x, a) {
	struct adc_adcs747x *adc = (struct adc_adcs747x *) a;
	uint16_t value;
	int32_t ret;
	ret = spiSlave_recvISR(adc->slave, &value, 1);
	if (ret < 0) {
		return -1;
	}
	return value;

}
ADC_SET_CALLBACK(adcs747x, a, callback, data) {
	struct adc_adcs747x *adc = (struct adc_adcs747x *) a;
	if (callback == NULL) {
		adc_stop((struct adc *) adc);
	}
	adc->callback = callback;
	adc->data = data;
	return 0;
}
ADC_START(adcs747x, a) {
	struct adc_adcs747x *adc = (struct adc_adcs747x *) a;
	if (!adc->slave) {
		return -1;
	}
	adc->running = true;
#ifdef CONFIG_INCLUDE_vTaskSuspend
	vTaskResume(adc->task);
#endif
	return 0;
}
ADC_STOP(adcs747x, a) {
	struct adc_adcs747x *adc = (struct adc_adcs747x *) a;
	adc->running = false;
	return 0;
}
ADC_OPS(adcs747x);
