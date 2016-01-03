#include <FreeRTOS.h>
#include <task.h>
#include <stdbool.h>
#include <stdint.h>
#include <adc.h>
#define ADC_PRV
#include <adc_prv.h>
extern int32_t _dev_adc;
struct adc **adcs = (struct adc **) &_dev_adc;
int32_t adc_generic_init(struct adc *a) {
	struct adc_generic *adc = (struct adc_generic *) a;
	if(adc->init) {
		return ADC_ALREDY_INITED;
	}
#ifdef CONFIG_ADC_THREAD_SAVE
	{
		int ret = hal_init(adc);
		if (ret < 0) {
			goto adc_genericInit_error0;
		}
	}
#endif
	adc->init = true;
	return 0;
#ifdef CONFIG_ADC_THREAD_SAVE
adc_genericInit_error0:
	return -1;
#endif
}
#ifdef CONFIG_ADC_MULTI
struct adc *adc_init(uint32_t index, uint8_t bits, uint32_t hz);
int32_t adc_deinit(struct adc *adc);
int32_t adc_get(struct adc *adc, TickType_t waittime);
int32_t adc_getISR(struct adc *adc);
int32_t adc_setCallback(struct adc *adc, bool (*callback)(struct adc *adc, uint32_t channel, int32_t value, void *data), void *data);
int32_t adc_start(struct adc *adc);
int32_t adc_stop(struct adc *adc);
#endif
