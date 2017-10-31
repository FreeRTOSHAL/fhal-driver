#include <adc.h>
#define ADC_PRV
#include <adc_prv.h>
#include <FreeRTOS.h>
#include <task.h>
#include <adc_test.h>
/* ISR Emulation Thread */
static void adc_test_task(void *data) {
	TickType_t oldtime;
	struct adc_test *adc = data;
	for (;;) {
		int32_t ret;
		oldtime = xTaskGetTickCount();
		if (!adc->running) {
#ifdef CONFIG_INCLUDE_vTaskSuspend
			vTaskSuspend(NULL);
#else
			vTaskDelay(1000 / portTICK_PERIOD_MS);
#endif
			continue;
		}
		ret = adc_get((struct adc *) adc, adc->ticks);
		CONFIG_ASSERT(ret >= 0);
		/* return not needed Scheduler is running and scheduled higher prio Task */
		(void) adc->callback((struct adc *) adc, adc->channelID, ret, adc->data);
		vTaskDelayUntil(&oldtime, adc->ticks);
	}
}

ADC_INIT(test, index, bits, hz) {
	struct adc_test *adc;
	int32_t ret;
	adc = (struct adc_test *) ADC_GET_DEV(index);
	if (adc == NULL) {
		goto test_adc_init_error0;
	}
	ret = adc_generic_init((struct adc *) adc);
	if (ret < 0) {
		goto test_adc_init_error0;
	}
	if (ret > 0) {
		goto test_adc_init_exit;
	}
	adc->running = false;
	/* waittime per ticks */
	adc->ticks = (1000 * portTICK_PERIOD_MS) / hz;
	adc->bits = bits;
	adc->slave = NULL;
	ret = OS_CREATE_TASK(adc_test_task, "ADC Test Task", 500, adc, CONFIG_ADC_TEST_PRIO, adc->task);
	if (ret != pdPASS) {
		goto test_adc_init_error0;
	}
test_adc_init_exit:
	return (struct adc *) adc;
test_adc_init_error0:
	return NULL;
}
int32_t adc_test_connect(struct adc *a, int32_t callback(struct adc *adc, void *data), void *data) {
	struct adc_test *adc = (struct adc_test *) a;
	if (!callback) {
		return -1;
	}
	adc->test_callback = callback;
	adc->test_data = data;
	return 0;
}
ADC_DEINIT(test, a) {
	return 0;
}
ADC_GET(test, a, waittime) {
	int32_t value;
	adc_lock(a, waittime, -1);
	value = adc_getISR(a);
	adc_unlock(a, -1);
	return value;
}
ADC_GET_ISR(test, a) {
	struct adc_test *adc = (struct adc_test *) a;
	int32_t value;
	if (adc->test_callback) {
		value = adc->test_callback(a, adc->test_data);
	} else {
		return -1;
	}
	return value;
}
ADC_SET_CALLBACK(test, a, callback, data) {
	struct adc_test *adc = (struct adc_test *) a;
	if (callback == NULL) {
		adc_stop((struct adc *) adc);
	}
	adc->callback = callback;
	adc->data = data;
	return 0;
}
ADC_START(test, a) {
	struct adc_test *adc = (struct adc_test *) a;
	adc->running = true;
#ifdef CONFIG_INCLUDE_vTaskSuspend
	vTaskResume(adc->task);
#endif
	return 0;
}
ADC_STOP(test, a) {
	struct adc_test *adc = (struct adc_test *) a;
	adc->running = false;
	return 0;
}
ADC_OPS(test);
