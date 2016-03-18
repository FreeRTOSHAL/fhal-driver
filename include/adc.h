#ifndef ADC_H_
#define ADC_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal.h>
/**
 * \defgroup ADC ADC Subsystem
 * \ingroup HAL
 * \code
 * #include <adc.h>
 * \endcode
 *
 * This is the ADC Subsystem for controlling ADC of a SOC. 
 * \{
 */
/**
 * Private Structure of ADC driver
 */
struct adc;
#ifdef CONFIG_ADC_MULTI
/**
 * Function of ADC driver in Multi Driver implementation mode 
 */
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
/**
 * Generic ADC Interface  
 */
struct adc_generic {
	/**
	 * true = is init
	 * false = is not init
	 */
	bool init;
#ifdef CONFIG_ADC_THREAD_SAVE
	/**
	 * Mutex
	 */
	SemaphoreHandle_t lock;	
#endif
#ifdef CONFIG_ADC_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct adc_ops *ops;
#endif
};
#ifndef CONFIG_ADC_MULTI
/**
 * init ADC instance
 * \param index Index of PWM
 * \param bits bit wide of ADC common: 4, 8, 12. 0 on only get instance
 * \param hz Sample frequency
 * \return ADC Instance NULL on Error if only get instance on bits == 0 
 */
struct adc *adc_init(uint32_t index, uint8_t bits, uint32_t hz);
/**
 * Deinit ADC
 * \param adc ADC instance
 * \return -1 on error 0 on ok
 */
int32_t adc_deinit(struct adc *adc);
/**
 * Get ADC Value
 * \param adc ADC instance
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error >= 0 Value
 */
int32_t adc_get(struct adc *adc, TickType_t waittime);
/**
 * Get ADC Value in Interrupt Service Routine
 * \param adc ADC instance
 * \return -1 on error >= 0 Value
 */
int32_t adc_getISR(struct adc *adc);
/**
 * Set ADC Interrupt Callback
 * \param adc ADC instance
 * \param callback Callback
 * \param data Data parsed to Callback
 * \return -1 on error 0 on ok
 */
int32_t adc_setCallback(struct adc *adc, bool (*callback)(struct adc *adc, uint32_t channel, int32_t value, void *data), void *data);
/**
 * Start ADC Sampling only needed if Callback is set
 * \param adc ADC instance
 * \return -1 on error 0 on ok
 */
int32_t adc_start(struct adc *adc);
/**
 * Stop ADC Sampling only needed if Callback is set
 * \param adc ADC instance
 * \return -1 on error 0 on ok
 */
int32_t adc_stop(struct adc *adc);
#else
inline struct adc *adc_init(uint32_t index, uint8_t bits, uint32_t hz) {
	HAL_DEFINE_GLOBAL_ARRAY(adc);
	struct adc_generic *a = (struct adc_generic *) HAL_GET_DEV(adc, index);
	if (a == NULL) {
		return NULL;
	}
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
/**\}*/
#endif
