#ifndef LTC6811_H_
#define LTC6811_H_
#include <hal.h>
#include <i2c.h>
#include <adc.h>
#include <spi.h>

/**\cond INTERNAL*/
struct adc_ltc6811;
struct ltc6811 {
	struct hal gen;
	/**
	 * Number of Slaves
	 * if ismaster == false must be 0
	 */
	const uint32_t numberOfSlaves;
	/**
	 * All slave mask for bitmask
	 */
	const uint32_t allSlavesMask;
	/**
	 * Driver index
	 */
	const bool isFirst;
	/**
	 * Driver index
	 */
	uint32_t index;
	/**
	 * I2C Master
	 */
	struct i2c *i2c;
	/**
	 * SPI Slave (connection to the first slave)
	 * Only be defined if ismaster == true
	 */
	struct spi_slave *spi;
	/**
	 * First Slave
	 */
	struct ltc6811 *first;
	/**
	 * Slaves
	 */
	struct ltc6811 **slaves;
	/**
	 * Bitmaks for select mutlible slaves
	 */
	int32_t selectSlaves;
	/**
	 * ADCs
	 */
	struct adc_ltc6811 **adcs;
};

/**
 * I2C Master in LTC6811
 */
struct i2c_ltc6811 {
	struct i2c_generic gen;
	struct ltc6811 *ltc;
};
/**
 * adcs
 */
struct adc_ltc6811 {
	struct adc_generic gen;
	struct ltc6811 *ltc;
};

/**\endcond*/

enum ltc_cmd {
	LTC_WRCFGA = 0x1,
	LTC_RDCFGA = 0x2,
	LTC_RDCVA = 0x4,
	LTC_RDCVB = 0x6,
	LTC_RDCVC = 0x8,
	LTC_RDCVD = 0xA,
	LTC_RDAUXA = 0xC,
	LTC_RDAUXB = 0xE,
	LTC_RDSTATA = 0x10,
	LTC_RDSTATB = 0x12,
	LTC_WRSCTRL = 0x14,
	LTC_WRPWM = 0x20,
	LTC_RDSCTRL = 0x16,
	LTC_RDPWM = 0x22,
	LTC_STSCTRL = 0x19,
	LTC_CLRSCTRL = 0x18,
	LTC_ADCV_NORM_DCP_ALL = 0x370,
	LTC_ADOW_NORM_PUP_ALL = 0x378,
	LTC_ADOW_NORM_PDN_ALL = 0x338,
	LTC_CVST_NORM_ST1 = 0x327,
	LTC_ADOL_NORM_DCP = 0x311,
	LTC_ADAX_NORM_GPIO1 = 0x561,
	LTC_ADAX_NORM_GPIO2 = 0x562,
	LTC_ADAXD_NORM_GPIO1 = 0x501,
	LTC_ADAXD_NORM_GPIO2 = 0x502,
	LTC_AXST_NORM_ST1 = 0x527,
	LTC_ADSTAT_NORM_ALL = 0x568,
	LTC_ADSTATD_NORM_ALL = 0x508,
	LTC_STATST_NORM_ST1 = 0x52F,
	LTC_ADCVAX_NORM_DCP = 0x57F,
	LTC_ADCVSC_NORM_DCP = 0x577,
	LTC_CLRCELL = 0x711,
	LTC_CLRAUX = 0x712,
	LTC_CLRSTAT = 0x713,
	LTC_PLADC = 0x714,
	LTC_DIAGN = 0x715,
	LTC_WRCOMM = 0x721,
	LTC_RDCOMM = 0x722,
	LTC_STCOMM = 0x723,
};

struct ltc6811 *ltc6811_init(uint32_t index);
int32_t ltc6811_connect(struct ltc6811 *ltc, struct spi *spi, uint8_t cs, uint16_t gpio, uint32_t bautrate);
int32_t ltc6811_connectSlave(struct ltc6811 *ltc, struct ltc6811 *slave);
int32_t ltc6811_write(struct ltc6811 *ltc, enum ltc_cmd cmd, uint8_t *data, uint32_t len);
int32_t ltc6811_read(struct ltc6811 *ltc, enum ltc_cmd cmd, uint8_t *data, uint32_t len);
uint16_t ltc6811_calcPEC(uint8_t* data, uint8_t len);
int32_t ltc6811_writeRegister(struct ltc6811 *ltc, enum ltc_cmd cmd, uint8_t *newData, uint8_t *oldData);
int32_t ltc6811_readRegister(struct ltc6811 *ltc, enum ltc_cmd cmd, uint8_t *registerContent);

/**\cond INTERNAL*/
#define LTC6811_ADC_DEV(id, adcID) \
	struct adc_ltc6811 ltc6811_adc_##id##_##adcID = { \
		.ltc = &ltc6811_dev_##id, \
	}; \
	ADC_ADDDEV(ltc6811, ltc6811_adc_##id##_##adcID)
#define ADC_PRV
#include <adc_prv.h>
extern const struct adc_ops lpc6811_adc_ops;
/**\endcond*/

#define LTC6811_ADDDEV(id, isfirst, numberofslave) \
	extern struct adc_ltc6811 *ltc6811_adcs_##id[12]; \
	struct ltc6811 ltc6811_dev_##id = { \
		HAL_NAME("LTC6811 " #id) \
		.gen.init = false, \
		.numberOfSlaves = numberofslave, \
		.allSlavesMask = ((1 << numberofslave) - 1), \
		.selectSlaves = ((1 << numberofslave) - 1), \
		.isFirst = isfirst, \
		.adcs = ltc6811_adcs_##id, \
	}; \
	HAL_ADD(ltc6811, ltc6811_dev_##id); \
	LTC6811_ADC_DEV(id, 0); \
	LTC6811_ADC_DEV(id, 1); \
	LTC6811_ADC_DEV(id, 2); \
	LTC6811_ADC_DEV(id, 3); \
	LTC6811_ADC_DEV(id, 4); \
	LTC6811_ADC_DEV(id, 5); \
	LTC6811_ADC_DEV(id, 6); \
	LTC6811_ADC_DEV(id, 7); \
	LTC6811_ADC_DEV(id, 8); \
	LTC6811_ADC_DEV(id, 9); \
	LTC6811_ADC_DEV(id, 10); \
	LTC6811_ADC_DEV(id, 11); \
	struct adc_ltc6811 *ltc6811_adcs_##id[12] = { \
		&ltc6811_adc_##id##_0, \
		&ltc6811_adc_##id##_1, \
		&ltc6811_adc_##id##_2, \
		&ltc6811_adc_##id##_3, \
		&ltc6811_adc_##id##_4, \
		&ltc6811_adc_##id##_5, \
		&ltc6811_adc_##id##_6, \
		&ltc6811_adc_##id##_7, \
		&ltc6811_adc_##id##_8, \
		&ltc6811_adc_##id##_9, \
		&ltc6811_adc_##id##_10, \
		&ltc6811_adc_##id##_11, \
	};

#define LTC6811_ID(id) HAL_GET_ID(hal, ltc6811, ltc6811_dev_##id)
#define LTC6811_ADC_ID(id, adcid) HAL_GET_ID(adc, ltc6811, ltc6811_adc_##id##_##adcid)
#define LTC6811_GET_ALL_ADCS(id, adcs) { \
	(adcs)[0] = adc_init(LTC6811_ADC_ID(id, 0), 12,0); \
	(adcs)[1] = adc_init(LTC6811_ADC_ID(id, 1), 12,0); \
	(adcs)[2] = adc_init(LTC6811_ADC_ID(id, 2), 12,0); \
	(adcs)[3] = adc_init(LTC6811_ADC_ID(id, 3), 12,0); \
	(adcs)[4] = adc_init(LTC6811_ADC_ID(id, 4), 12,0); \
	(adcs)[5] = adc_init(LTC6811_ADC_ID(id, 5), 12,0); \
	(adcs)[6] = adc_init(LTC6811_ADC_ID(id, 6), 12,0); \
	(adcs)[7] = adc_init(LTC6811_ADC_ID(id, 7), 12,0); \
	(adcs)[8] = adc_init(LTC6811_ADC_ID(id, 8), 12,0); \
	(adcs)[9] = adc_init(LTC6811_ADC_ID(id, 9), 12,0); \
	(adcs)[10] = adc_init(LTC6811_ADC_ID(id, 10), 12,0); \
	(adcs)[11] = adc_init(LTC6811_ADC_ID(id, 11), 12,0); \
} \

#endif
