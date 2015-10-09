#ifndef ADC_PRV_H_
#define ADC_PRV_H_
#ifndef ADC_PRV
# error "Never include this file out of a ADC driver"
#endif
#include <stdbool.h>
#include <stdint.h>
#include <FreeRTOS.h>
#include <semphr.h>
#define ADC_ALREDY_INITED 1
struct adc_generic {
	bool init;
#ifdef CONFIG_ADC_THREAD_SAVE
	SemaphoreHandle_t lock;	
#endif
};
struct adc_prv {
	struct adc_generic generic;
};
int32_t adc_genericInit(struct adc *adc);
#ifdef CONFIG_ADC_THREAD_SAVE
inline int adc_lock(struct adc *adc, TickType_t waittime) {
	struct adc_prv *adc_prv = (struct adc_prv *) adc;
	return xSemaphoreTakeRecursive(adc_prv->generic.lock, waittime);
}
inline int adc_unlock(struct adc *adc) {
	struct adc_prv *adc_prv = (struct adc_prv *) adc;
	return xSemaphoreGiveRecursive(adc_prv->generic.lock);
}
#else
# define adc_lock(u, w) 1
# define adc_unlock(u) 1
#endif
#endif
