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
	uint32_t index;
	/**
	 * Task
	 */
	OS_DEFINE_TASK(adcTask, 500);
	/**
	 * Bitmaks for select mutlible slaves
	 */
	int32_t selectSlaves;
	/**
	 * Slaves
	 */
	struct ltc6811_slave **slaves;
	/**
	 * SPI Slave
	 */
	struct spi_slave *spi;
};

struct ltc6811_slave {
	struct hal gen;
	/**
	 * Driver index
	 */
	uint32_t index;
	/**
	 * I2C Master
	 */
	struct i2c *i2c;
	/**
	 * First Slave
	 */
	struct ltc6811 *master;
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
	struct ltc6811_slave *ltc;
};
/**
 * adcs
 */
struct adc_ltc6811 {
	struct adc_generic gen;
	struct ltc6811_slave *ltc;
};

/**\endcond*/
/*
 * TDOD Missing Commands?
 * 
 * Write Configuration Register Group B (WRCFGB)
 * Read Configuration Register Group A (RDCFGA)
 * Read Configuration Register Group B (RDCFGB)
 * Read Cell Voltage Register Group E (RDCVE)
 * Read Cell Voltage Register Group F (RDCVF)
 * Read Auxiliary Register Group C (RDAUXC)
 * Read Auxiliary Register Group D (RDAUXD)
 * Write PWM/S Control Register Group B (WRPSB)
 * Read PWM/S Control Register Group B (RDPSB)
 */
/**
 * Write Configuration Register Group A (WRCFGA)
 */
#define LTC_CMD_WRCFGA (0x1)
/**
 * Read Configuration Register Group A (RDCFGA)
 */
#define LTC_CMD_RDCFGA (0x2)
/**
 * Read Cell Voltage Register Group A (RDCVA)
 */
#define LTC_CMD_RDCVA (0x4)
/**
 * Read Cell Voltage Register Group B (RDCVB)
 */
#define LTC_CMD_RDCVB (0x6)
/**
 * Read Cell Voltage Register Group C (RDCVC)
 */
#define LTC_CMD_RDCVC (0x8)
/**
 * Read Cell Voltage Register Group D (RDCVD)
 */
#define LTC_CMD_RDCVD (0xA)
/**
 * Read Auxiliary Register Group A (RDAUXA)
 */
#define LTC_CMD_RDAUXA (0xC)
/**
 * Read Auxiliary Register Group B (RDAUXB)
 */
#define LTC_CMD_RDAUXB (0xE)
/**
 * Read Status Register Group A (RDSTATA)
 */
#define LTC_CMD_RDSTATA (0x10)
/**
 * Read Status Register Group B (RDSTATB)
 */
#define LTC_CMD_RDSTATB (0x12)
/**
 * Write S Control Register Group (WRSCTRL)
 */
#define LTC_CMD_WRSCTRL (0x14)
/**
 * Write PWM Register Group (WRPWM)
 */
#define LTC_CMD_WRPWM (0x20)
/**
 * Read S Control Register Group (RDSCTRL)
 */
#define LTC_CMD_RDSCTRL (0x16)
/**
 * Read PWM Register Group (RDPWM)
 */
#define LTC_CMD_RDPWM (0x22)
/**
 * Start S Control Pulsing and Poll Status (STSCTRL)
 */
#define LTC_CMD_STSCTRL (0x19)
/**
 * Clear S Control Register Group (CLRSCTRL)
 */
#define LTC_CMD_CLRSCTRL (0x18)
/**
 * Start Cell Voltage ADC Conversion and Poll Status (ADCV)
 * @param CH Cell Selection for ADC Conversion
 *        |     |                    | Total Conversion Time in the 8 ADC Modes ||||||||
 *        |  CH |                    | 27kHz | 14kHz | 7kHz  | 3kHz  | 2kHz  | 1kHz  | 422Hz  | 26Hz  |
 *        | :-- | :----------------- | :---: | :---: | :---: | :---: | :---: | :---: | :----: | :---: |
 *        | 000 | All Cells          | 1.1ms | 1.3ms | 2.3ms | 3.0ms | 4.4ms | 7.2ms | 12.8ms | 201ms |
 *        | 001 | Cell 1 and Cell 7  | 201μs | 230μs | 405μs | 501μs | 754μs | 1.2ms | 2.2ms  |  34ms |
 *        | 010 | Cell 2 and Cell 8  |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 011 | Cell 3 and Cell 9  |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 100 | Cell 4 and Cell 10 |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 101 | Cell 5 and Cell 11 |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 110 | Cell 6 and Cell 12 |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 * @param DCP Discharge Permitted 0 = Discharge Not Permitted 1 = Discharge Permitted
 * @param MD ADC Mode
 *        | MD  | ADCOPT(CFGR0[0]) = 0 | ADCOPT(CFGR0[0]) = 1 | 
 *        | :-: | :------------------: | :------------------: |
 *        | 00  | 422Hz Mode           | 1kHz Mode            |
 *        | 01  | 27kHz Mode (Fast)    | 14kHz Mode           |
 *        | 10  | 7kHz Mode (Normal)   | 3kHz Mode            |
 *        | 11  | 26Hz Mode (Filtered) | 2kHz Mode            |
 */
#define LTC_CMD_ADCV(CH, DCP, MD) (BIT(5) | BIT(6) | BIT(9) | (((CH) & 0x7) << 0) | (((DCP) & 0x1) << 4) | (((MD) & 0x3) << 7))
#define LTC_CMD_ADCV_NORM_DCP_ALL (0x370)
/**
 * Start Open Wire ADC Conversion and Poll Status ADOW 0 1 1 1 (PUP)
 * @param CH Cell Selection for ADC Conversion
 *        |     |                    | Total Conversion Time in the 8 ADC Modes ||||||||
 *        |  CH |                    | 27kHz | 14kHz | 7kHz  | 3kHz  | 2kHz  | 1kHz  | 422Hz  | 26Hz  |
 *        | :-- | :----------------- | :---: | :---: | :---: | :---: | :---: | :---: | :----: | :---: |
 *        | 000 | All Cells          | 1.1ms | 1.3ms | 2.3ms | 3.0ms | 4.4ms | 7.2ms | 12.8ms | 201ms |
 *        | 001 | Cell 1 and Cell 7  | 201μs | 230μs | 405μs | 501μs | 754μs | 1.2ms | 2.2ms  |  34ms |
 *        | 010 | Cell 2 and Cell 8  |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 011 | Cell 3 and Cell 9  |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 100 | Cell 4 and Cell 10 |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 101 | Cell 5 and Cell 11 |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 110 | Cell 6 and Cell 12 |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 * @param DCP Discharge Permitted 0 = Discharge Not Permitted 1 = Discharge Permitted
 * @param PUP Pull-Up/Pull-Down Current for Open Wire Conversions 0 = Pull-Down Current 1 = Pull-Up Current
 * @param MD ADC Mode
 *        | MD  | ADCOPT(CFGR0[0]) = 0 | ADCOPT(CFGR0[0]) = 1 | 
 *        | :-: | :------------------: | :------------------: |
 *        | 00  | 422Hz Mode           | 1kHz Mode            |
 *        | 01  | 27kHz Mode (Fast)    | 14kHz Mode           |
 *        | 10  | 7kHz Mode (Normal)   | 3kHz Mode            |
 *        | 11  | 26Hz Mode (Filtered) | 2kHz Mode            |
 */
#define LTC_CMD_ADOW(CH, DCP, PUP, MD) (BIT(3) | BIT(5) | BIT(9) | (((CH) & 0x7) << 0) |(((PUP) & 0x1) << 6) ((DCP & 0x1) << 4) | (((MD) & 0x3) << 7))
#define LTC_CMD_ADOW_NORM_PUP_ALL (0x378)
#define LTC_CMD_ADOW_NORM_PDN_ALL (0x338)
/**
 * Start Self Test Cell Voltage Conversion and Poll Status (CVST)
 * @param ST Self Test Mode Selection 
 *        |    |             | Self Test Conversion Result |||||||||
 *        | ST |             | 27kHz  | 14kHz  | 7kHz   | 3kHz   | 2kHz   | 1kHz   | 422Hz  | 26Hz   |
 *        | :--| :---------: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
 *        | 01 | Self Test 1 | 0x9565 | 0x9553 | 0x9555 | 0x9555 | 0x9555 | 0x9555 | 0x9555 | 0x9555 |
 *        | 10 | Self Test 2 | 0x6A9A | 0x6AAC | 0x6AAA | 0x6AAA | 0x6AAA | 0x6AAA | 0x6AAA | 0x6AAA |
 * @param MD ADC Mode
 *        | MD  | ADCOPT(CFGR0[0]) = 0 | ADCOPT(CFGR0[0]) = 1 | 
 *        | :-: | :------------------: | :------------------: |
 *        | 00  | 422Hz Mode           | 1kHz Mode            |
 *        | 01  | 27kHz Mode (Fast)    | 14kHz Mode           |
 *        | 10  | 7kHz Mode (Normal)   | 3kHz Mode            |
 *        | 11  | 26Hz Mode (Filtered) | 2kHz Mode            |
 */
#define LTC_CMD_CVST(ST, MD) (BIT(0) | BIT(1) | BIT(2) | BIT(9) | (((ST) & 0x3) << 5) | (((MD) & 0x3) << 7))
#define LTC_CMD_CVST_NORM_ST1 (0x327)
/**
 * Start Overlap Measurement of Cell 7 Voltage (ADOL)
 * @param DCP Discharge Permitted 0 = Discharge Not Permitted 1 = Discharge Permitted
 * @param MD ADC Mode
 *        | MD  | ADCOPT(CFGR0[0]) = 0 | ADCOPT(CFGR0[0]) = 1 | 
 *        | :-: | :------------------: | :------------------: |
 *        | 00  | 422Hz Mode           | 1kHz Mode            |
 *        | 01  | 27kHz Mode (Fast)    | 14kHz Mode           |
 *        | 10  | 7kHz Mode (Normal)   | 3kHz Mode            |
 *        | 11  | 26Hz Mode (Filtered) | 2kHz Mode            |
 */
#define LTC_CMD_ADOL(DCP, MD) (BIT(0) | BIT(9) | (((DCP) & 0x1) << 4) | (((MD) & 0x3) << 7))
#define LTC_CMD_ADOL_NORM_DCP (0x311)
/**
 * Start GPIOs ADC Conversion and Poll Status (ADAX)
 * @param CHG GPIO Selection for ADC Conversion
 *        |     | Total Conversion Time in the 8 ADC Modes |||||||||
 *        | CHG |                   | 27kHz | 14kHz | 7kHz  | 3kHz  | 2kHz  | 1kHz  | 422Hz  | 26Hz  |
 *        | :-- | :---------------- | :---: | :---: | :---: | :---: | :---: | :---: | :----: | :---: |
 *        | 000 | GPIO 1-5, 2nd Ref | 1.1ms | 1.3ms | 2.3ms | 3.0ms | 4.4ms | 7.2ms | 12.8ms | 201ms |
 *        | 001 | GPIO 1            | 201μs | 230μs | 405μs | 501μs | 754μs | 1.2ms | 2.2ms  | 34ms  |
 *        | 010 | GPIO 2            |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 011 | GPIO 3            |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 100 | GPIO 4            |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 101 | GPIO 5            |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 110 | 2 nd Reference    |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 * @param MD ADC Mode
 *        | MD  | ADCOPT(CFGR0[0]) = 0 | ADCOPT(CFGR0[0]) = 1 | 
 *        | :-: | :------------------: | :------------------: |
 *        | 00  | 422Hz Mode           | 1kHz Mode            |
 *        | 01  | 27kHz Mode (Fast)    | 14kHz Mode           |
 *        | 10  | 7kHz Mode (Normal)   | 3kHz Mode            |
 *        | 11  | 26Hz Mode (Filtered) | 2kHz Mode            |
 */
#define LTC_CMD_ADAX(CHG, MD) (BIT(5) | BIT(6) | BIT(10) | (((CHG) & 0x7) << 0) | (((MD & 0x3) << 7)))
#define LTC_CMD_ADAX_NORM_GPIO1 (0x561)
#define LTC_CMD_ADAX_NORM_GPIO2 (0x562)
/**
 * Start GPIOs ADC Conversion With Digital Redundancy and Poll Status (ADAXD)
 * @param CHG GPIO Selection for ADC Conversion
 *        |     | Total Conversion Time in the 8 ADC Modes |||||||||
 *        | CHG |                   | 27kHz | 14kHz | 7kHz  | 3kHz  | 2kHz  | 1kHz  | 422Hz  | 26Hz  |
 *        | :-- | :---------------- | :---: | :---: | :---: | :---: | :---: | :---: | :----: | :---: |
 *        | 000 | GPIO 1-5, 2nd Ref | 1.1ms | 1.3ms | 2.3ms | 3.0ms | 4.4ms | 7.2ms | 12.8ms | 201ms |
 *        | 001 | GPIO 1            | 201μs | 230μs | 405μs | 501μs | 754μs | 1.2ms | 2.2ms  | 34ms  |
 *        | 010 | GPIO 2            |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 011 | GPIO 3            |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 100 | GPIO 4            |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 101 | GPIO 5            |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 *        | 110 | 2 nd Reference    |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^    |   ^   |
 * @param MD ADC Mode
 *        | MD  | ADCOPT(CFGR0[0]) = 0 | ADCOPT(CFGR0[0]) = 1 | 
 *        | :-: | :------------------: | :------------------: |
 *        | 00  | 422Hz Mode           | 1kHz Mode            |
 *        | 01  | 27kHz Mode (Fast)    | 14kHz Mode           |
 *        | 10  | 7kHz Mode (Normal)   | 3kHz Mode            |
 *        | 11  | 26Hz Mode (Filtered) | 2kHz Mode            |
 */
#define LTC_CMD_ADAXD(CHG, MD) (BIT(10) | (((CHG) & 0x7) << 0) | (((MD & 0x3) << 7)))
#define LTC_CMD_ADAXD_NORM_GPIO1 (0x501)
#define LTC_CMD_ADAXD_NORM_GPIO2 (0x502)
/**
 * Start Self Test GPIOs Conversion and Poll Status (AXST)
 * @param ST Self Test Mode Selection 
 *        |    |             | Self Test Conversion Result |||||||||
 *        | ST |             | 27kHz  | 14kHz  | 7kHz   | 3kHz   | 2kHz   | 1kHz   | 422Hz  | 26Hz   |
 *        | :--| :---------: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
 *        | 01 | Self Test 1 | 0x9565 | 0x9553 | 0x9555 | 0x9555 | 0x9555 | 0x9555 | 0x9555 | 0x9555 |
 *        | 10 | Self Test 2 | 0x6A9A | 0x6AAC | 0x6AAA | 0x6AAA | 0x6AAA | 0x6AAA | 0x6AAA | 0x6AAA |
 * @param MD ADC Mode
 *        | MD  | ADCOPT(CFGR0[0]) = 0 | ADCOPT(CFGR0[0]) = 1 | 
 *        | :-: | :------------------: | :------------------: |
 *        | 00  | 422Hz Mode           | 1kHz Mode            |
 *        | 01  | 27kHz Mode (Fast)    | 14kHz Mode           |
 *        | 10  | 7kHz Mode (Normal)   | 3kHz Mode            |
 *        | 11  | 26Hz Mode (Filtered) | 2kHz Mode            |
 */
#define LTC_CMD_AXST(ST, MD) (BIT(0) | BIT(1) | BIT(2) | BIT(10) | (((ST) & 0x3) << 5) | (((MD) & 0x3) << 7))
#define LTC_CMD_AXST_NORM_ST1 (0x527)
/**
 * Start Status Group ADC Conversion and Poll Status (ADSTAT)
 * @param CHST Status Group Selection
 *        |      | Total Conversion Time in the 8 ADC Modes ||||||||
 *        | CHST |                  | 27kHz | 14kHz | 7kHz  | 3kHz  | 2kHz  | 1kHz  | 422Hz | 26Hz  |
 *        | 000  | SC, ITMP, VA, VD | 748μs | 865μs | 1.6ms | 2.0ms | 3.0ms | 4.8ms | 8.5ms | 134ms |
 *        | 001  | SC               | 201μs | 230μs | 405μs | 501μs | 754μs | 1.2ms | 2.2ms | 34ms  |
 *        | 010  | ITMP             |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |
 *        | 011  | VA               |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |
 *        | 100  | VD               |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |
 * @param MD ADC Mode
 *        | MD  | ADCOPT(CFGR0[0]) = 0 | ADCOPT(CFGR0[0]) = 1 | 
 *        | :-: | :------------------: | :------------------: |
 *        | 00  | 422Hz Mode           | 1kHz Mode            |
 *        | 01  | 27kHz Mode (Fast)    | 14kHz Mode           |
 *        | 10  | 7kHz Mode (Normal)   | 3kHz Mode            |
 *        | 11  | 26Hz Mode (Filtered) | 2kHz Mode            |
 */
#define LTC_CMD_ADSTAT(CHST, MD) (BIT(3) | BIT(5) | BIT(6) | BIT(10) | (((CHST) & 0x7) << 0) | (((MD) & 0x3) << 7))
#define LTC_CMD_ADSTAT_NORM_ALL (0x568)
/**
 * Start Status Group ADC Conversion With Digital Redundancy and Poll Status (ADSTATD)
 * @param CHST Status Group Selection
 *        |      | Total Conversion Time in the 8 ADC Modes ||||||||
 *        | CHST |                  | 27kHz | 14kHz | 7kHz  | 3kHz  | 2kHz  | 1kHz  | 422Hz | 26Hz  |
 *        | 000  | SC, ITMP, VA, VD | 748μs | 865μs | 1.6ms | 2.0ms | 3.0ms | 4.8ms | 8.5ms | 134ms |
 *        | 001  | SC               | 201μs | 230μs | 405μs | 501μs | 754μs | 1.2ms | 2.2ms | 34ms  |
 *        | 010  | ITMP             |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |
 *        | 011  | VA               |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |
 *        | 100  | VD               |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |   ^   |
 * @param MD ADC Mode
 *        | MD  | ADCOPT(CFGR0[0]) = 0 | ADCOPT(CFGR0[0]) = 1 | 
 *        | :-: | :------------------: | :------------------: |
 *        | 00  | 422Hz Mode           | 1kHz Mode            |
 *        | 01  | 27kHz Mode (Fast)    | 14kHz Mode           |
 *        | 10  | 7kHz Mode (Normal)   | 3kHz Mode            |
 *        | 11  | 26Hz Mode (Filtered) | 2kHz Mode            |
 */
#define LTC_CMD_ADSTATD(CHST, MD) (BIT(3) | BIT(10) | (((CHST) & 0x7) << 0) | (((MD) & 0x3) << 7))
#define LTC_CMD_ADSTATD_NORM_ALL (0x508)
/**
 * Start Self Test Status Group Conversion and Poll Status (STATST)
 * @param ST Self Test Mode Selection 
 *        |    |             | Self Test Conversion Result |||||||||
 *        | ST |             | 27kHz  | 14kHz  | 7kHz   | 3kHz   | 2kHz   | 1kHz   | 422Hz  | 26Hz   |
 *        | :--| :---------: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
 *        | 01 | Self Test 1 | 0x9565 | 0x9553 | 0x9555 | 0x9555 | 0x9555 | 0x9555 | 0x9555 | 0x9555 |
 *        | 10 | Self Test 2 | 0x6A9A | 0x6AAC | 0x6AAA | 0x6AAA | 0x6AAA | 0x6AAA | 0x6AAA | 0x6AAA |
 * @param MD ADC Mode
 *        | MD  | ADCOPT(CFGR0[0]) = 0 | ADCOPT(CFGR0[0]) = 1 | 
 *        | :-: | :------------------: | :------------------: |
 *        | 00  | 422Hz Mode           | 1kHz Mode            |
 *        | 01  | 27kHz Mode (Fast)    | 14kHz Mode           |
 *        | 10  | 7kHz Mode (Normal)   | 3kHz Mode            |
 *        | 11  | 26Hz Mode (Filtered) | 2kHz Mode            |
 */
#define LTC_CMD_STATST(ST, MD) (BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(10) | (((ST) & 0x3) << 5) | (((MD) & 0x3) << 7))
#define LTC_CMD_STATST_NORM_ST1 (0x52F)
/**
 * Start Combined Cell Voltage and GPIO1, GPIO2 Conversion and Poll Status (ADCVAX)
 * @param DCP Discharge Permitted 0 = Discharge Not Permitted 1 = Discharge Permitted
 * @param MD ADC Mode
 *        | MD  | ADCOPT(CFGR0[0]) = 0 | ADCOPT(CFGR0[0]) = 1 | 
 *        | :-: | :------------------: | :------------------: |
 *        | 00  | 422Hz Mode           | 1kHz Mode            |
 *        | 01  | 27kHz Mode (Fast)    | 14kHz Mode           |
 *        | 10  | 7kHz Mode (Normal)   | 3kHz Mode            |
 *        | 11  | 26Hz Mode (Filtered) | 2kHz Mode            |
 */
#define LTC_CMD_ADCVAX(DCP, MD) (BIT(0) | BIT(1) | BIT(2) | BIT(3) | BIT(6) | BIT(10) | (((DCP) & 0x1) << 4) | (((MD) & 0x3) << 7))
#define LTC_CMD_ADCVAX_NORM_DCP (0x57F)
/**
 * Start Combined Cell Voltage and SC Conversion and Poll Status (ADCVSC)
 * @param DCP Discharge Permitted 0 = Discharge Not Permitted 1 = Discharge Permitted
 * @param MD ADC Mode
 *        | MD  | ADCOPT(CFGR0[0]) = 0 | ADCOPT(CFGR0[0]) = 1 | 
 *        | :-: | :------------------: | :------------------: |
 *        | 00  | 422Hz Mode           | 1kHz Mode            |
 *        | 01  | 27kHz Mode (Fast)    | 14kHz Mode           |
 *        | 10  | 7kHz Mode (Normal)   | 3kHz Mode            |
 *        | 11  | 26Hz Mode (Filtered) | 2kHz Mode            |
 */
#define LTC_CMD_ADCVSC(DCP, MD) (BIT(0) | BIT(1) | BIT(2) | BIT(6) | BIT(10) | (((DCP) & 0x1) << 4) | (((MD) & 0x3) << 7))
#define LTC_CMD_ADCVSC_NORM_DCP (0x577)
/**
 * Clear Cell Voltage Register Groups (CLRCELL)
 */
#define LTC_CMD_CLRCELL (0x711)
/**
 * Clear Auxiliary Register Groups (CLRAUX)
 */
#define LTC_CMD_CLRAUX (0x712)
/**
 * Clear Status Register Groups (CLRSTAT)
 */
#define LTC_CMD_CLRSTAT (0x713)
/**
 * Poll ADC Conversion Status (PLADC)
 */
#define LTC_CMD_PLADC (0x714)
/**
 * Diagnose MUX and Poll Status (DIAGN)
 */
#define LTC_CMD_DIAGN (0x715)
/**
 * Write COMM Register Group (WRCOMM)
 */
#define LTC_CMD_WRCOMM (0x721)
/**
 * Read COMM Register Group (RDCOMM)
 */
#define LTC_CMD_RDCOMM (0x722)
/**
 * Start Communication I2C/SPI (STCOMM)
 */
#define LTC_CMD_STCOMM (0x723)

struct ltc6811 *ltc6811_init(uint32_t index);
struct ltc6811_slave *ltc6811_slave_init(uint32_t index);
int32_t ltc6811_connect(struct ltc6811 *ltc, struct spi *spi, uint8_t cs, uint16_t gpio, uint32_t bautrate);
int32_t ltc6811_write(struct ltc6811 *ltc, enum ltc_cmd cmd, uint8_t *data, uint32_t len);
int32_t ltc6811_read(struct ltc6811 *ltc, enum ltc_cmd cmd, uint8_t *data, uint32_t len);
uint16_t ltc6811_calcPEC(uint8_t* data, uint8_t len);
int32_t ltc6811_writeRegister(struct ltc6811 *ltc, enum ltc_cmd cmd, uint8_t *newData, uint8_t *oldData);
int32_t ltc6811_readRegister(struct ltc6811 *ltc, enum ltc_cmd cmd, uint8_t *registerContent);

/**\cond INTERNAL*/
#define LTC6811_ADC_DEV(masterid, id, adcID) \
	struct adc_ltc6811 ltc6811_adc_##masterid##_##id##_##adcID = { \
		.ltc = &ltc6811_slave_##masterid##_##id, \
	}; \
	ADC_ADDDEV(ltc6811, ltc6811_adc_##masterid##_##id##_##adcID)
#define ADC_PRV
#include <adc_prv.h>
extern const struct adc_ops lpc6811_adc_ops;
/**\endcond*/
#define LTC6811_ADDDEV(id, numberofslave) \
	struct ltc6811_slave *ltc6811_slaves_##id[numberofslave]; \
	struct ltc6811 ltc6811_dev_##id = { \
		HAL_NAME("LTC6811 " #id) \
		.gen.init = false, \
		.numberOfSlaves = numberofslave, \
		.allSlavesMask = ((1 << numberofslave) - 1), \
		.selectSlaves = ((1 << numberofslave) - 1), \
		.slaves = ltc6811_slaves_##id, \
	}; \
	HAL_ADD(ltc6811, ltc6811_dev_##id);

#define LTC6811_SLAVE_ADDDEV(masterid, id) \
	extern struct adc_ltc6811 *ltc6811_adcs_##id[12]; \
	struct ltc6811_slave ltc6811_slave_##masterid##_##id = { \
		HAL_NAME("LTC6811 " #id) \
		.gen.init = false, \
		.master = &ltc6811_dev_##masterid,\
		.adcs = ltc6811_adcs_##id, \
	}; \
	HAL_ADD(ltc6811, ltc6811_slave_##masterid##_##id); \
	LTC6811_ADC_DEV(masterid, id, 0); \
	LTC6811_ADC_DEV(masterid, id, 1); \
	LTC6811_ADC_DEV(masterid, id, 2); \
	LTC6811_ADC_DEV(masterid, id, 3); \
	LTC6811_ADC_DEV(masterid, id, 4); \
	LTC6811_ADC_DEV(masterid, id, 5); \
	LTC6811_ADC_DEV(masterid, id, 6); \
	LTC6811_ADC_DEV(masterid, id, 7); \
	LTC6811_ADC_DEV(masterid, id, 8); \
	LTC6811_ADC_DEV(masterid, id, 9); \
	LTC6811_ADC_DEV(masterid, id, 10); \
	LTC6811_ADC_DEV(masterid, id, 11); \
	struct adc_ltc6811 *ltc6811_adcs_##id[12] = { \
		&ltc6811_adc_##masterid##_##id##_0, \
		&ltc6811_adc_##masterid##_##id##_1, \
		&ltc6811_adc_##masterid##_##id##_2, \
		&ltc6811_adc_##masterid##_##id##_3, \
		&ltc6811_adc_##masterid##_##id##_4, \
		&ltc6811_adc_##masterid##_##id##_5, \
		&ltc6811_adc_##masterid##_##id##_6, \
		&ltc6811_adc_##masterid##_##id##_7, \
		&ltc6811_adc_##masterid##_##id##_8, \
		&ltc6811_adc_##masterid##_##id##_9, \
		&ltc6811_adc_##masterid##_##id##_10, \
		&ltc6811_adc_##masterid##_##id##_11, \
	};

#define LTC6811_ID(id) HAL_GET_ID(hal, ltc6811, ltc6811_dev_##id)
#define LTC6811_SLAVE_ID(masterid, id) HAL_GET_ID(hal, ltc6811, ltc6811_slave_##masterid##_##id)
#define LTC6811_ADC_ID(masterid, id, adcid) HAL_GET_ID(adc, ltc6811, ltc6811_adc_##masterid##_##id##_##adcid)
#define LTC6811_GET_ALL_ADCS(masterid, id, adcs) { \
	(adcs)[0] = adc_init(LTC6811_ADC_ID(masterid, id, 0), 12,0); \
	(adcs)[1] = adc_init(LTC6811_ADC_ID(masterid, id, 1), 12,0); \
	(adcs)[2] = adc_init(LTC6811_ADC_ID(masterid, id, 2), 12,0); \
	(adcs)[3] = adc_init(LTC6811_ADC_ID(masterid, id, 3), 12,0); \
	(adcs)[4] = adc_init(LTC6811_ADC_ID(masterid, id, 4), 12,0); \
	(adcs)[5] = adc_init(LTC6811_ADC_ID(masterid, id, 5), 12,0); \
	(adcs)[6] = adc_init(LTC6811_ADC_ID(masterid, id, 6), 12,0); \
	(adcs)[7] = adc_init(LTC6811_ADC_ID(masterid, id, 7), 12,0); \
	(adcs)[8] = adc_init(LTC6811_ADC_ID(masterid, id, 8), 12,0); \
	(adcs)[9] = adc_init(LTC6811_ADC_ID(masterid, id, 9), 12,0); \
	(adcs)[10] = adc_init(LTC6811_ADC_ID(masterid, id, 10), 12,0); \
	(adcs)[11] = adc_init(LTC6811_ADC_ID(masterid, id, 11), 12,0); \
} \

#endif
