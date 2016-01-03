#ifndef ADC_H_
#define ADC_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal.h>

struct adc;
#ifdef CONFIG_ADC_MULTI
struct adc_ops {
	struct adc *(*adc_init)(uint32_t index, uint8_t bits, uint32_t hz);
	int32_t (*adc_deinit)(struct adc *adc);
	int32_t (*adc_get)(struct adc *adc, TickType_t waittime);
	int32_t (*adc_getISR)(struct adc *adc);
	int32_t (*adc_setCallback)(struct adc *adc, bool (*callback)(struct adc *adc, uint32_t channel, int32_t value, void *data), void *data);
	int32_t (*adc_start)(struct adc *adc);
	int32_t (*adc_stop)(struct adc *adc);
};
#endif
struct adc_generic {
	bool init;
#ifdef CONFIG_ADC_THREAD_SAVE
	SemaphoreHandle_t lock;	
#endif
#ifdef CONFIG_ADC_MULTI
	const struct adc_ops *ops;
#endif
};

extern struct adc **adcs;
#ifndef CONFIG_ADC_MULTI
struct adc *adc_init(uint32_t index, uint8_t bits, uint32_t hz);
int32_t adc_deinit(struct adc *adc);
int32_t adc_get(struct adc *adc, TickType_t waittime);
int32_t adc_getISR(struct adc *adc);
int32_t adc_setCallback(struct adc *adc, bool (*callback)(struct adc *adc, uint32_t channel, int32_t value, void *data), void *data);
int32_t adc_start(struct adc *adc);
int32_t adc_stop(struct adc *adc);
#else
inline struct adc *adc_init(uint32_t index, uint8_t bits, uint32_t hz) {
	struct adc_generic *a = (struct adc_generic *) adcs[index];
	return a->ops->adc_init(index, bits, hz);
}
inline int32_t adc_deinit(struct adc *adc) {
	struct adc_generic *a = (struct adc_generic *) adc;
	return a->ops->adc_deinit(adc);
}
inline int32_t adc_get(struct adc *adc, TickType_t waittime) {
	struct adc_generic *a = (struct adc_generic *) adc;
	return a->ops->adc_get(adc, waittime);
}
inline int32_t adc_getISR(struct adc *adc) {
	struct adc_generic *a = (struct adc_generic *) adc;
	return a->ops->adc_getISR(adc);
}
inline int32_t adc_setCallback(struct adc *adc, bool (*callback)(struct adc *adc, uint32_t channel, int32_t value, void *data), void *data) {
	struct adc_generic *a = (struct adc_generic *) adc;
	return a->ops->adc_setCallback(adc, callback, data);
}
inline int32_t adc_start(struct adc *adc) {
	struct adc_generic *a = (struct adc_generic *) adc;
	return a->ops->adc_start(adc);
}
inline int32_t adc_stop(struct adc *adc) {
	struct adc_generic *a = (struct adc_generic *) adc;
	return a->ops->adc_stop(adc);
}
#endif
#endif
