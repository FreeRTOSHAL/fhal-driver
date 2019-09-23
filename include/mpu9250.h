/*
 * Copyright (c) 2016 Andreas Werner <kernel@andy89.org>
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 */
#ifndef MPU_9250_H_
#define MPU_9250_H_
#include <stdint.h>
#include <spi.h>
#include <vec.h>
#include <accel.h>
#include <gyro.h>
/**
 * \defgroup MPU9250 MPU9250 driver
 * \ingroup HAL
 * \code
 * #include <mpu9250.h>
 * \endcode
 * 
 * This is the MPU9250 Driver.
 * \{
 */

/**\cond INTERNAL*/
#define MPU_READ BIT(7)

#define MPU_SELF_TEST_X_GYRO 0x00
#define MPU_SELF_TEST_Y_GYRO 0x01
#define MPU_SELF_TEST_Z_GYRO 0x02
#define MPU_SELF_TEST_X_ACCEL 0x0D
#define MPU_SELF_TEST_Y_ACCEL 0x0E
#define MPU_SELF_TEST_Z_ACCEL 0x0F
#define MPU_XG_OFFSET_H 0x13
#define MPU_XG_OFFSET_L 0x14
#define MPU_YG_OFFSET_H 0x15
#define MPU_YG_OFFSET_L 0x16
#define MPU_ZG_OFFSET_H 0x17
#define MPU_ZG_OFFSET_L 0x18
#define MPU_SMPLRT_DIV 0x19
#define MPU_CONFIG 0x1A
#define MPU_CONFIG_DLPF_CFG(x) (((x) & 0x7) << 0)
#define MPU_CONFIG_EXT_SYNC_SET(x) (((x) & 0x7) << 3)
#define MPU_CONFIG_FIFO_MODE BIT(6)
#define MPU_GYRO_CONFIG 0x1B
#define MPU_GYRO_CONFIG_FCHOICE_B(x) (((x) & 0x3) << 0)
#define MPU_GYRO_CONFIG_GYRO_FS_SEL(x) (((x) & 0x3) << 3)
#define MPU_GYRO_CONFIG_ZGYRO_CT_EN BIT(5)
#define MPU_GYRO_CONFIG_YGYRO_CT_EN BIT(6)
#define MPU_GYRO_CONFIG_XGYRO_CT_EN BIT(7)
#define MPU_ACCEL_CONFIG 0x1C
#define MPU_ACCEL_CONFIG_ACCEL_FS_SEL(x) (((x) & 0x3) << 3)
#define MPU_ACCEL_CONFIG_AZ_ST_EN BIT(5)
#define MPU_ACCEL_CONFIG_AY_ST_EN BIT(6)
#define MPU_ACCEL_CONFIG_AX_ST_EN BIT(7)
#define MPU_ACCEL_CONFIG_2 0x1D
#define MPU_ACCEL_CONFIG_2_A_DLPF_CFG(x) (((x) & 0x3) << 0)
#define MPU_ACCEL_CONFIG_2_ACCEL_FCHOICE_B(x) (((x) & 0x3) << 2)
#define MPU_LP_ACCEL_ODR 0x1E
#define MPU_LP_ACCEL_ODR_LPOSC_CLKSEL (((x) & 0xF) << 0)
#define MPU_WOM_THR 0x1F
#define MPU_FIFO_EN 0x23
#define MPU_FIFO_EN_SLV0 BIT(0)
#define MPU_FIFO_EN_SLV1 BIT(1)
#define MPU_FIFO_EN_SLV2 BIT(2)
#define MPU_FIFO_EN_ACCEL BIT(3)
#define MPU_FIFO_EN_GYRO_ZOUT BIT(4)
#define MPU_FIFO_EN_GYRO_YOUT BIT(5)
#define MPU_FIFO_EN_GYRO_GYRO_XOUT BIT(6)
#define MPU_FIFO_EN_TEMP_FIFO_EN BIT(7)
#define MPU_I2C_MST_CTRL 0x24
#define MPU_I2C_MST_CTRL_I2C_MST_CLK(x) (((x) & 0xF) << 0)
#define MPU_I2C_MST_CTRL_I2C_MST_P_NSR(x) BIT(4)
#define MPU_I2C_MST_CTRL_SLV_3_FIFO_EN BIT(5)
#define MPU_I2C_MST_CTRL_WAIT_FOR_ES BIT(6)
#define MPU_I2C_MST_CTRL_MULT_MST_EN BIT(7)
#define MPU_I2C_SLV0_ADDR 0x25
#define MPU_I2C_SLV_ADDR_I2C_ID(x) (((x) & 0x7F) << 0)
#define MPU_I2C_SLV_ADDR_I2C_SLV0_RNW BIT(7)
#define MPU_I2C_SLV0_REG 0x38
#define MPU_I2C_SLV0_CTRL 0x27
#define MPU_I2C_SLV_CTRL_I2C_SLV_LENG(x) (((x) & 0xF) << 0)
#define MPU_I2C_SLV_CTRL_I2C_SLV_GRP BIT(4)
#define MPU_I2C_SLV_CTRL_I2C_SLV_REG_DIS BIT(5)
#define MPU_I2C_SLV_CTRL_I2C_SLV_BYTE_SW BIT(6)
#define MPU_I2C_SLV_CTRL_I2C_SLV_EN BIT(7)
#define MPU_I2C_SLV1_ADDR 0x28
#define MPU_I2C_SLV1_REG 0x29
#define MPU_I2C_SLV1_CTRL 0x2A
#define MPU_I2C_SLV2_ADDR 0x2B
#define MPU_I2C_SLV2_REG 0x2C
#define MPU_I2C_SLV2_CTRL 0x2D
#define MPU_I2C_SLV3_ADDR 0x2E
#define MPU_I2C_SLV3_REG 0x2F
#define MPU_I2C_SLV3_CTRL 0x30
#define MPU_I2C_SLV4_ADDR 0x31
#define MPU_I2C_SLV4_REG 0x32
#define MPU_I2C_SLV4_DO 0x33
#define MPU_I2C_SLV4_CTRL 0x34
#define MPU_I2C_SLV4_DI 0x35
#define MPU_I2C_MST_STATUS 0x36
#define MPU_I2C_MST_STATUS_I2C_SLV0_NACK BIT(0)
#define MPU_I2C_MST_STATUS_I2C_SLV1_NACK BIT(1)
#define MPU_I2C_MST_STATUS_I2C_SLV2_NACK BIT(2)
#define MPU_I2C_MST_STATUS_I2C_SLV3_NACK BIT(3)
#define MPU_I2C_MST_STATUS_I2C_SLV4_NACK BIT(4)
#define MPU_I2C_MST_STATUS_I2C_LOST_ARB BIT(5)
#define MPU_I2C_MST_STATUS_I2C_SLV4_DONE BIT(6)
#define MPU_I2C_MST_STATUS_PASS_THROUGH BIT(7)
#define MPU_INT_PIN_CFG 0x37
#define MPU_INT_PIN_CFG_BYPASS_EN BIT(1)
#define MPU_INT_PIN_CFG_FSYNC_INT_MODE_EN BIT(2)
#define MPU_INT_PIN_CFG_ACTL_FSYNC BIT(3)
#define MPU_INT_PIN_CFG_INT_ANYRD_2CLEAR BIT(4)
#define MPU_INT_PIN_CFG_LATCH_INT_EN BIT(5)
#define MPU_INT_PIN_CFG_OPEN BIT(6)
#define MPU_INT_PIN_CFG_ACTL BIT(7)
#define MPU_INT_ENABLE 0x38
#define MPU_INT_ENABLE_RAW_RDY_EN BIT(0)
#define MPU_INT_ENABLE_FSYNC_INT_EN BIT(3)
#define MPU_INT_ENABLE_FIFO_OFLOW_EN BIT(4)
#define MPU_INT_ENABLE_WOM_EN BIT(5)
#define MPU_INT_STATUS 0x3A
#define MPU_INT_STATUS_RAW_DATA_RDY_INT BIT(0)
#define MPU_INT_STATUS_FSYNC_INT BIT(3)
#define MPU_INT_STATUS_FIFO_OFLOW_INT BIT(4)
#define MPU_INT_STATUS_WOM_INT BIT(6)
#define MPU_ACCEL_XOUT_H 0x3B
#define MPU_ACCEL_XOUT_L 0x3C
#define MPU_ACCEL_YOUT_H 0x3D
#define MPU_ACCEL_YOUT_L 0x3E
#define MPU_ACCEL_ZOUT_H 0x3F
#define MPU_ACCEL_ZOUT_L 0x40
#define MPU_TEMP_OUT_H 0x41
#define MPU_TEMP_OUT_L 0x42
#define MPU_GYRO_XOUT_H 0x43
#define MPU_GYRO_XOUT_L 0x44
#define MPU_GYRO_YOUT_H 0x45
#define MPU_GYRO_YOUT_L 0x46
#define MPU_GYRO_ZOUT_H 0x47
#define MPU_GYRO_ZOUT_L 0x48
#define MPU_EXT_SENS_DATA(x) (0x49 + x)
#define MPU_I2C_SLV0_DO 0x63
#define MPU_I2C_SLV1_DO 0x64
#define MPU_I2C_SLV2_DO 0x65
#define MPU_I2C_SLV3_DO 0x66
#define MPU_I2C_MST_DELAY_CTRL 0x67
#define MPU_I2C_MST_DELAY_CTRL_I2C_SLV0_DLY_EN BIT(0)
#define MPU_I2C_MST_DELAY_CTRL_I2C_SLV1_DLY_EN BIT(1)
#define MPU_I2C_MST_DELAY_CTRL_I2C_SLV2_DLY_EN BIT(2)
#define MPU_I2C_MST_DELAY_CTRL_I2C_SLV3_DLY_EN BIT(3)
#define MPU_I2C_MST_DELAY_CTRL_I2C_SLV4_DLY_EN BIT(4)
#define MPU_I2C_MST_DELAY_CTRL_DELAY_ES_SHADOW BIT(5)
#define MPU_SIGNAL_PATH_RESET 0x68
#define MPU_SIGNAL_PATH_RESET_TEMP_RST BIT(0)
#define MPU_SIGNAL_PATH_RESET_ACCEL_RST BIT(1)
#define MPU_SIGNAL_PATH_RESET_GYRO_RST BIT(2)
#define MPU_MOT_DETECT_CTRL 0x69
#define MPU_MOT_DETECT_CTRL_ACCEL_INTEL_MODE BIT(6)
#define MPU_MOT_DETECT_CTRL_ACCEL_INTEL_EN BIT(7)
#define MPU_USER_CTRL 0x6A
#define MPU_USER_CTRL_SIG_COND_RST BIT(0)
#define MPU_USER_CTRL_I2C_MST_RST BIT(1)
#define MPU_USER_CTRL_FIFO_RST BIT(2)
#define MPU_USER_CTRL_I2C_IF_DIS BIT(4)
#define MPU_USER_CTRL_I2C_MST_EN BIT(5)
#define MPU_USER_CTRL_FIFO_EN BIT(6)
#define MPU_PWR_MGMT_1 0x6B
#define MPU_PWR_MGMT_1_CLKSEL(x) (((x) & 0x7) << 0)
#define MPU_PWR_MGMT_1_PD_PTAT BIT(3)
#define MPU_PWR_MGMT_1_GYRO_STANDBY BIT(4)
#define MPU_PWR_MGMT_1_CYCLE BIT(5)
#define MPU_PWR_MGMT_1_SLEEP BIT(6)
#define MPU_PWR_MGMT_1_H_RESET BIT(7)
#define MPU_PWR_MGMT_2 0x6C
#define MPU_PWR_MGMT_2_DIS_ZG BIT(0)
#define MPU_PWR_MGMT_2_DIS_YG BIT(1)
#define MPU_PWR_MGMT_2_DIS_XG BIT(2)
#define MPU_PWR_MGMT_2_DIS_ZA BIT(3)
#define MPU_PWR_MGMT_2_DIS_YA BIT(4)
#define MPU_PWR_MGMT_2_DIS_XA BIT(5)
#define MPU_FIFO_COUNTH 0x72
#define MPU_FIFO_COUNTL 0x73
#define MPU_FIFO_R_W 0x74
#define MPU_WHO_AM_I 0x75
#define MPU_WHO_AM_I_VAL 0x71
#define MPU_WHO_AM_I_VAL_ICM20938 0x71
#define MPU_XA_OFFSET_H 0x77
#define MPU_XA_OFFSET_L 0x78
#define MPU_YA_OFFSET_H 0x7A
#define MPU_YA_OFFSET_L 0x7B
#define MPU_ZA_OFFSET_H 0x7D
#define MPU_ZA_OFFSET_L 0x7E
/**\endcond*/
/**
 * MPU9250 Gravity Constant 
 */
#define MPU_GRAVITY 16384

/**
 * Private Structure of MPU9250 driver
 */
struct mpu9250;
/**
 * Accelerator Driver Structure
 */
struct mpu9250_accel {
	/**
	 * Generic Accelerator Structure
	 */
	struct accel_generic gen;
	/**
	 * MPU9250 Instance
	 */
	struct mpu9250 *mpu;
	/**
	 * Basis Values
	 */
	struct vector accelBasis;
};
/**
 * Gyroscope Driver Structure
 */
struct mpu9250_gyro {
	/**
	 * Generic Gyroscope Structure
	 */
	struct gyro_generic gen;
	/**
	 * MPU9250 Instance
	 */
	struct mpu9250 *mpu;
	/**
	 * Basis Values
	 */
	struct vector gyroBasis;
};

/**
 * Private Structure of MPU9250 driver
 */
struct mpu9250 {
	struct hal gen;
	/**
	 * SPI Options
	 */
	const struct spi_opt opt;
	/**
	 * SPI Index ID
	 */
	const uint32_t spi;

	/**
	 * Index of Driver
	 */
	uint32_t index;
	/**
	 * SPI Slave Instance
	 */
	struct spi_slave *slave;
	/**
	 * Accelerator Instance
	 */
	struct mpu9250_accel *accel;
	/**
	 * Gyro Instance
	 */
	struct mpu9250_gyro *gyro;
};
/**
 * Vector
 */
struct mpu9250_vector {
	/**
	 * x Value
	 */ 
	float x;
	/**
	 * y Value
	 */
	float y;
	/**
	 * z Value
	 */
	float z;
};
/**
 * MPU9250 init
 * \param index Driver Index
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return MPU9250 Instance or NULL on error
 */
struct mpu9250 *mpu9250_init(uint32_t index, TickType_t waittime);
/**
 * Deinit MPU9250 Instance
 * \param mpu MPU9250 Instance
 * \return -1 on error 0 on ok
 */
int32_t mpu9250_deinit(struct mpu9250 *mpu);
/**
 * Reset MPU9250
 * \param mpu MPU9250 Instance
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t mpu9250_reset(struct mpu9250 *mpu, TickType_t waittime);
/**
 * Get actual Accelerator Value
 * \param mpu MPU9250 Instance
 * \param vec Value
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t mpu9250_getAccel(struct mpu9250 *mpu, struct mpu9250_vector *vec, TickType_t waittime);
/**
 * Get actual Gyroscope Value
 * \param mpu MPU9250 Instance
 * \param vec Value
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t mpu9250_getGyro(struct mpu9250 *mpu, struct mpu9250_vector *vec, TickType_t waittime);
/**\cond INTERNAL*/
#define ACCEL_PRV
#include <accel_prv.h>
#define GYRO_PRV
#include <gyro_prv.h>
extern const struct gyro_ops mpu9250_gyro_ops;
extern const struct accel_ops mpu9250_accel_ops;
/**\endcond*/
/** 
 * The Number of MPU9250 is Board spezific, the User Code must Call
 * this Macro to define a new MPU9250 dev
 * \warning do not use the created Variable directly! Use the init function!
 * \param id Unique identifier
 * \param spi_id Index of SPI Dev
 * \param cs_id CS Number or SPI_OPT_CS_DIS
 * \param gpio_id GPIO Pin Number or SPI_OPT_GPIO_DIS
 * \param baut Bautrate
 */
#define MPU9250_ADDDEV(id, spi_id, cs_id, gpio_id, baut) \
		struct mpu9250 mpu9250_##id; \
		struct mpu9250_accel mpu9250_accel_##id = { \
			ACCEL_INIT_DEV(mpu9250) \
			HAL_NAME("MPU9250 " #id " Accelerator") \
			.mpu = &mpu9250_##id, \
			.accelBasis = {0, 0, 0}, \
		};\
		struct mpu9250_gyro mpu9250_gyro_##id = { \
			GYRO_INIT_DEV(mpu9250) \
			HAL_NAME("MPU9250 " #id " Gyro") \
			.mpu = &mpu9250_##id, \
			.gyroBasis = {0, 0, 0}, \
		};\
		struct mpu9250 mpu9250_##id = { \
			HAL_NAME("MPU9250 " #id) \
			.gen.init = false, \
			.spi = spi_id, \
			.opt = { \
				.lsb = false, \
				.cpol = false, \
				.cpha = false, \
				.cs = cs_id, \
				.csLowInactive = false, \
				.gpio = gpio_id, \
				.size = 8, \
				.wdelay = 0, \
				.cs_hold = 8, \
				.cs_delay = 500, \
				.bautrate = baut, \
			}, \
			.slave = NULL, \
			.accel = &mpu9250_accel_##id, \
			.gyro = &mpu9250_gyro_##id, \
		}; \
		ACCEL_ADDDEV(mpu9250, mpu9250_accel_##id); \
		GYRO_ADDDEV(mpu9250, mpu9250_gyro_##id); \
		HAL_ADD(mpu9250, mpu9250_##id)
/**\}*/
#endif
