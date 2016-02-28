#ifndef TPS65381_h_
#define TPS65381_h_
#include <FreeRTOS.h>
#include <stdint.h>
#include <spi.h>
#include <adc.h>
/**
 * \defgroup TPS65381 TPS65381 driver
 * \ingroup HAL
 * \code
 * #include <tps65381.h>
 * \endcode
 * 
 * This is the TPS65381 Driver.
 * \todo Implement Watchdog
 * \{
 */
/**
 * TPS65381 Prive Driver Structur
 */
struct tps65381;
/**
 * Select Voltage forwood to Diag Pin
 */
enum tps_diag {
	/**
	 * VDD5
	 */
	TPS_VDD5,
	/**
	 * VDD6
	 */
	TPS_VDD6,
	/**
	 * VCP
	 */
	TPS_VCP,
	/**
	 * VSOut 1
	 */
	TPS_VSOUT1,
	/**
	 * VBAT Safing
	 */
	TPS_VBAT_SAFING,
	/**
	 * VBAT
	 */
	TPS_VBAT,
	/**
	 * Main BG
	 */
	TPS_MAIN_BG,
	/**
	 * VMON BG
	 */
	TPS_VMON_BG
};
/**
 * Init TPS65381
 * \param slave SPI Slave Instance
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return TPS65381 Instance or NULL on error
 */
struct tps65381 *tps_init(struct spi_slave *slave, TickType_t waittime);
/**
 * Deinit TPS65381
 * \param tps TPS65381 Instance
 * \return -1 on error 0 on ok
 */
int32_t tps_deinit(struct tps65381 *tps);
/**
 * Mux Diag Pin
 * 
 * \param tps TPS65381 Instance
 * \param diag Mux this Pin
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t tps_mux(struct tps65381 *tps, enum tps_diag diag, TickType_t waittime);
/**
 * Measurement Diag Pin 
 * 
 * \param tps TPS65381 Instance
 * \param diag Mux this Pin
 * \param adc ADC Pin
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return Voltage on Diag Pin
 */
float tps_diag(struct tps65381 *tps, enum tps_diag diag, struct adc *adc, TickType_t waittime);
/**\}*/
#endif
