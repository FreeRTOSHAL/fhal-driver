#ifndef ADCS747X_H_
#define ADCS747X_H_
/**\cond INTERNAL*/
#include <adc.h>
#define ADC_PRV
#include <adc_prv.h>
#include <spi.h>
#include <FreeRTOS.h>
#include <task.h>
extern const struct adc_ops adcs747x_adc_ops;
struct adc_adcs747x {
	struct adc_generic gen;
	bool running;
	uint32_t ticks;
	uint32_t bits;
	uint32_t channelID;
	OS_DEFINE_TASK(task, 500);
	struct spi_slave *slave;
	bool (*callback)(struct adc *adc, uint32_t channel, int32_t value, void *data);
	void *data;
};
/**\endcond*/
int32_t adcs747x_connect(struct adc *a, struct spi *spi, uint8_t cs, uint16_t gpio);
#define ADD_ADCS747X(_channelID) \
	struct adc_adcs747x adcs747x_##_channelID = { \
		ADC_INIT_DEV(adcs747x) \
		HAL_NAME("TI ADCS747x Channel " #_channelID) \
		.channelID = _channelID, \
	};\
	ADC_ADDDEV(adcs747x, adcs747x_##_channelID)
#define ADCS747X_ID(_channelID) HAL_GET_ID(adc, adcs747x, adcs747x_##_channelID)
#endif
