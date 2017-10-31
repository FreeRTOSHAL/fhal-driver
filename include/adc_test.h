#ifndef ADC_TEST_H_
#define ADC_TEST_H_
/**\cond INTERNAL*/
#include <adc.h>
#define ADC_PRV
#include <adc_prv.h>
#include <spi.h>
#include <FreeRTOS.h>
#include <task.h>
extern const struct adc_ops test_ops;
struct adc_test {
	struct adc_generic gen;
	bool running;
	uint32_t ticks;
	uint32_t bits;
	uint32_t channelID;
	OS_DEFINE_TASK(task, 500);
	struct spi_slave *slave;
	bool (*callback)(struct adc *adc, uint32_t channel, int32_t value, void *data);
	void *data;

	int32_t (*test_callback)(struct adc *adc, void *data);
	void *test_data;
};
/**\endcond*/
int32_t adc_test_connect(struct adc *adc, int32_t callback(struct adc *adc, void *data), void *data);
#define ADD_ADC_TEST(_channelID) \
	struct adc_test test_##_channelID = { \
		ADC_INIT_DEV(test) \
		HAL_NAME("Test ADC Channel " #_channelID) \
		.channelID = _channelID, \
	};\
	ADC_ADDDEV(test, test_##_channelID)
#define TEST_ID(_channelID) HAL_GET_ID(adc, test, test_##_channelID)
#endif
