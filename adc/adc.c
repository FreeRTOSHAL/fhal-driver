#include <FreeRTOS.h>
#include <task.h>
#include <stdbool.h>
#include <stdint.h>
#include <adc.h>
#define ADC_PRV
#include <adc_prv.h>


int32_t adc_genericInit(struct adc *adc) {
	struct adc_prv *adc_prv = (struct adc_prv *) adc;
	if(adc_prv->generic.init) {
		return ADC_ALREDY_INITED;
	}
#ifdef CONFIG_ADC_THREAD_SAVE
	{
		adc_prv->generic.lock = xSemaphoreCreateRecursiveMutex();
		if (adc_prv->generic.lock == NULL) {
			goto adc_genericInit_error0;
		}
	}
#endif
	adc_prv->generic.init = true;
	return 0;
#ifdef CONFIG_ADC_THREAD_SAVE
adc_genericInit_error0:
	return -1;
#endif
}
#ifdef CONFIG_ADC_THREAD_SAVE
int adc_lock(struct adc *adc, TickType_t waittime);
int adc_unlock(struct adc *adc);
#endif
