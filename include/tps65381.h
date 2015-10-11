#ifndef TPS65381_h_
#define TPS65381_h_
#include <FreeRTOS.h>
#include <stdint.h>
#include <spi.h>
#include <adc.h>
struct tps65381;
enum tps_diag {
	TPS_VDD5,
	TPS_VDD6,
	TPS_VCP,
	TPS_VSOUT1,
	TPS_VBAT_SAFING,
	TPS_VBAT,
	TPS_MAIN_BG,
	TPS_VMON_BG
};
struct tps65381 *tps_init(struct spi_slave *slave, TickType_t waittime);
int32_t tps_deinit(struct tps65381 *tps);
int32_t tps_mux(struct tps65381 *tps, enum tps_diag diag, TickType_t waittime);
float tps_diag(struct tps65381 *tps, enum tps_diag diag, struct adc *adc, uint32_t adcChannel, TickType_t waittime);
#endif
