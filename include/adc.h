#ifndef ADC_H_
#define ADC_H_
#include <FreeRTOS.h>
#include <stdint.h>

struct adc;
struct adc *adc_init(uint32_t index, uint8_t bits, uint32_t hz);
int32_t adc_deinit(struct adc *adc);
int32_t adc_channel(struct adc *adc, uint32_t channel);
int32_t adc_get(struct adc *adc, uint32_t channel, TickType_t waittime);
#endif
