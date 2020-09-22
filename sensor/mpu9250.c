/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <mpu9250.h>
#include <mpu9250_prv.h>
#include <vec.h>
#include <spi.h>
#include <hal.h>

static int32_t mpu9250_calibrate(struct mpu9250 *mpu, TickType_t waittime) {
	int32_t i;
	int32_t ret = mpu9250_reset(mpu, waittime);
	if (ret < 0) {
		goto mpu9250_calibrate_error0;
	}
	/* Turn on internal clock source */
	ret = mpu_send(mpu, MPU_PWR_MGMT_1, 0x0, waittime);
	if (ret < 0) {
		goto mpu9250_calibrate_error0;
	}
	vTaskDelay(15 / portTICK_PERIOD_MS);
	/* Configure MPU9250 gyro and accelerometer for bias calculati */
	{
		uint16_t count;
		/* set low-pass filter to 188 Hz */
		ret = mpu_send(mpu, MPU_CONFIG, 0x01, waittime);
		if (ret < 0) {
			goto mpu9250_calibrate_error0;
		}
		/* Set sample rate to 1 kHz */
		ret = mpu_send(mpu, MPU_SMPLRT_DIV, 0x0, waittime);
		if (ret < 0) {
			goto mpu9250_calibrate_error0;
		}
		/* Set gyro full-scale to 250 degrees per second, maximum sensitivity */
		ret = mpu_send(mpu, MPU_GYRO_CONFIG, 0x0, waittime);
		if (ret < 0) {
			goto mpu9250_calibrate_error0;
		}
		/* Set accelerometer full-scale to 2 g, maximum sensitivity */
		ret = mpu_send(mpu, MPU_ACCEL_CONFIG, 0x0, waittime);
		if (ret < 0) {
			goto mpu9250_calibrate_error0;
		}
		{
			TickType_t lastWakeUpTime;
			/* Configure FIFO to capture accelerometer and gyro data for bias calculation */
			ret = mpu_send(mpu, MPU_USER_CTRL, MPU_USER_CTRL_FIFO_EN, waittime);
			if (ret < 0) {
				goto mpu9250_calibrate_error0;
			}
			lastWakeUpTime = xTaskGetTickCount();
			/* Enable gyro and accelerometer sensors for FIFO (max size 512 bytes in MPU-9250) */
			ret = mpu_send(
				mpu, 
				MPU_FIFO_EN, 
				(
					MPU_FIFO_EN_ACCEL | 
					MPU_FIFO_EN_GYRO_ZOUT | 
					MPU_FIFO_EN_GYRO_YOUT | 
					MPU_FIFO_EN_GYRO_GYRO_XOUT
				), 
				waittime
			);
			if (ret < 0) {
				goto mpu9250_calibrate_error0;
			}
			/* accumulate 40 samples in 40 milliseconds = 480 bytes */
			vTaskDelayUntil(&lastWakeUpTime, 40 / portTICK_PERIOD_MS);
		}
		/* At end of sample accumulation, turn off FIFO sensor read */
		/* Disable gyro and accelerometer sensors for FIFO */
		ret = mpu_send(mpu, MPU_FIFO_EN, 0x0, waittime);
		if (ret < 0) {
			goto mpu9250_calibrate_error0;
		}
		/* 
		 * Get FIFO Counter
		 */
		{
			uint8_t countRAW[2];
			ret = mpu_recv(mpu, MPU_FIFO_COUNTH, countRAW, 2, waittime);
			if (ret < 0) {
				goto mpu9250_calibrate_error0;
			}
			count = (((uint16_t) countRAW[0]) << 8) | ((uint16_t) countRAW[1]);
		}
		{
			int32_t accelVec32[3];
			int32_t gyroVec32[3];
			memset(accelVec32, 0, sizeof(accelVec32));
			memset(gyroVec32, 0, sizeof(gyroVec32));
			for (i = 0; i < count; i+=12) {
				uint8_t data[12];
				struct vector accelVec;
				struct vector gyroVec;
				ret = mpu_recv(mpu, MPU_FIFO_R_W, data, 12, waittime);
				if (ret < 0) {
					goto mpu9250_calibrate_error0;
				}
				mpu_buildVec(&accelVec, data);
				mpu_buildVec(&gyroVec, data + 6);
				accelVec32[0] += accelVec.x;
				accelVec32[1] += accelVec.y;
				accelVec32[2] += accelVec.z;
				gyroVec32[0] += gyroVec.x;
				gyroVec32[1] += gyroVec.y;
				gyroVec32[2] += gyroVec.z;
			}
			mpu->accel->accelBasis.x = (int16_t) (accelVec32[0] / (count / 12));
			mpu->accel->accelBasis.y = (int16_t) (accelVec32[1] / (count / 12));
			mpu->accel->accelBasis.z = (int16_t) (accelVec32[2] / (count / 12));
			mpu->gyro->gyroBasis.x = (int16_t) (gyroVec32[0] / (count / 12));
			mpu->gyro->gyroBasis.y = (int16_t) (gyroVec32[1] / (count / 12));
			mpu->gyro->gyroBasis.z = (int16_t) (gyroVec32[2] / (count / 12));

			/* Remove Gravity */
			if (mpu->accel->accelBasis.z > 0) {
				mpu->accel->accelBasis.z -= MPU_GRAVITY;
			} else {
				mpu->accel->accelBasis.z += MPU_GRAVITY;
			}
		}

		{
			uint8_t countRAW[2];
			ret = mpu_recv(mpu, MPU_FIFO_COUNTH, countRAW, 2, waittime);
			if (ret < 0) {
				goto mpu9250_calibrate_error0;
			}
			count = (((uint16_t) countRAW[0]) << 8) | ((uint16_t) countRAW[1]);
			/* 
			 * FIFO should be empty
			 */
			if (count != 0) {
				goto mpu9250_calibrate_error0;
			}
		}


	}
	return 0;
mpu9250_calibrate_error0:
	return -1;
}

static int32_t mpu9250_isAlive(struct mpu9250 *mpu, TickType_t waittime) {
	int32_t ret;
	uint8_t val;
	PRINTF("Check MPU is alive\n");
	ret = mpu_recv(mpu, MPU_WHO_AM_I, &val, 1, waittime);
	if (ret < 0) {
		return -1;
	}
	if (val != MPU_WHO_AM_I_VAL) {
		PRINTF("MPU is not alive who am i register: 0x%x", val);
		return -1;
	}
	PRINTF("MPU is alive\n");
	return 0;
}

struct mpu9250 *mpu9250_init(uint32_t index, struct spi *spi, uint8_t cs, uint16_t gpio, TickType_t waittime) {
	int32_t ret;
	const struct spi_opt opt = {
		.lsb = false,
		.cpol = false,
		.cpha = false,
		.cs = cs,
		.csLowInactive = false,
		.gpio = gpio,
		.size = 8,
		.wdelay = 0,
		.cs_hold = 8,
		.cs_delay = 500,
		.bautrate = 5000000,
	};
	struct mpu9250 *mpu = (struct mpu9250 *) HAL_GET_GLOBAL_DEV(index);
	if (mpu->gen.init) {
		return mpu;
	}
	ret = hal_init(mpu);
	if (ret < 0) {
		goto mpu9250_init_error0;
	}
	mpu->index = index;
	mpu->gen.init = true;
	PRINTF("SPI Init\n");
	mpu->slave = spiSlave_init(spi, (struct spi_opt *) &opt);
	if (mpu->slave == NULL) {
		goto mpu9250_init_error1;
	}
	/* 
	 * Check MPU is alive
	 */
	ret = mpu9250_isAlive(mpu, waittime);;
	if (ret < 0) {
		goto mpu9250_init_error2;
	}
	PRINTF("MPU9250 Reset\n");
	ret = mpu9250_reset(mpu, waittime);
	if (ret < 0) {
		goto mpu9250_init_error2;
	}
	/* 
	 * Check MPU is alive after Reset
	 */
	ret = mpu9250_isAlive(mpu, waittime);;
	if (ret < 0) {
		goto mpu9250_init_error2;
	}
	PRINTF("MPU Calibrate");
	ret = mpu9250_calibrate(mpu, waittime);
	if (ret < 0) {
		goto mpu9250_init_error2;
	}

	PRINTF("MPU9250 Reset\n");
	ret = mpu9250_reset(mpu, waittime);
	if (ret < 0) {
		goto mpu9250_init_error2;
	}

	PRINTF("MPU Config");
	{
		ret = mpu_clearSetBit(
			mpu, 
			MPU_CONFIG, 
			MPU_CONFIG_DLPF_CFG(0x7), 
			MPU_CONFIG_DLPF_CFG(0x2), /* 98 */
			waittime
		);
		if (ret < 0) {
			goto mpu9250_init_error2;
		}
	}
	/* 
	 * Setup Sample Frequenz
	 */	
	{
		ret = mpu_send(mpu, MPU_SMPLRT_DIV, 0x1, waittime);
		if (ret < 0) {
			goto mpu9250_init_error2;
		}
	}
	/* 
	 * Setup Gyro Full Scale to +2000dps
	 */
	{
		ret = mpu_clearSetBit(
			mpu, 
			MPU_GYRO_CONFIG, 
			(
				MPU_GYRO_CONFIG_ZGYRO_CT_EN | 
				MPU_GYRO_CONFIG_YGYRO_CT_EN | 
				MPU_GYRO_CONFIG_XGYRO_CT_EN |
				MPU_GYRO_CONFIG_GYRO_FS_SEL(0x3) |
				MPU_GYRO_CONFIG_FCHOICE_B(0x3)
			), 
			MPU_ACCEL_CONFIG_ACCEL_FS_SEL(0x3),
			waittime
		);
		if (ret < 0) {
			goto mpu9250_init_error2;
		}
	}
	/* 
	 * Setup Accel Full Scale to +-8g
	 * TODO Confgiuerable over KConfig
	 */
	{
		ret = mpu_clearSetBit(
			mpu, 
			MPU_ACCEL_CONFIG, 
			(
				MPU_ACCEL_CONFIG_AZ_ST_EN | 
				MPU_ACCEL_CONFIG_AY_ST_EN | 
				MPU_ACCEL_CONFIG_AX_ST_EN | 
				MPU_ACCEL_CONFIG_ACCEL_FS_SEL(0x3)
			), 
			MPU_ACCEL_CONFIG_ACCEL_FS_SEL(0x2),
			waittime
		);
		if (ret < 0) {
			goto mpu9250_init_error2;
		}
	}
	/* 
	 * Setup Accel to 1 kHz and 42 HZ bandwidth
	 */
#if 0
	{
		ret = mpu_clearSetBit(
			mpu, 
			MPU_ACCEL_CONFIG_2, 
			(
				MPU_ACCEL_CONFIG_2_ACCEL_FCHOICE_B(0x3) | 
				MPU_ACCEL_CONFIG_2_A_DLPF_CFG(0x3)
			), 
			MPU_ACCEL_CONFIG_2_A_DLPF_CFG(0x3),
			0,
			waittime
		);
		if (ret < 0) {
			goto mpu9250_init_error2;
		}
	}
#endif
	return mpu;
mpu9250_init_error2:
	spiSlave_deinit(mpu->slave);
mpu9250_init_error1:
	hal_deinit(mpu);	
mpu9250_init_error0:
	mpu->gen.init = false;
	return NULL;
}
int32_t mpu9250_deinit(struct mpu9250 *mpu) {
	hal_deinit(mpu);	
	return 0;
}
int32_t mpu9250_reset(struct mpu9250 *mpu, TickType_t waittime) {
	int32_t ret;
	mpu9250_lock(mpu, waittime, -1);
	ret = mpu_send(mpu, MPU_PWR_MGMT_1, MPU_PWR_MGMT_1_H_RESET, waittime);
	if (ret < 0) {
		goto mpu9250_reset_error1;
	}
	vTaskDelay(100 / portTICK_PERIOD_MS);

	ret = mpu_send(mpu, MPU_PWR_MGMT_1, 0, waittime);
	if (ret < 0) {
		goto mpu9250_reset_error1;
	}
	vTaskDelay(100 / portTICK_PERIOD_MS);
	ret = mpu_send(mpu, MPU_PWR_MGMT_1, MPU_PWR_MGMT_1_CLKSEL(1), waittime);
	if (ret < 0) {
		goto mpu9250_reset_error1;
	}
	vTaskDelay(100 / portTICK_PERIOD_MS);
	/*
	 * Enable xyz from Gyro and Accsl
	 */
	ret = mpu_send(mpu, MPU_PWR_MGMT_2, 0x0, waittime);
	if (ret < 0) {
		goto mpu9250_reset_error1;
	}
	mpu9250_unlock(mpu, -1);
	return 0;
mpu9250_reset_error1:
	mpu9250_unlock(mpu, -1);
	return -1;
}

int32_t mpu9250_getAccel(struct mpu9250 *mpu, struct mpu9250_vector *vec, TickType_t waittime) {
	struct vector vecRAW;
	int32_t ret = accel_get((struct accel *) mpu->accel, &vecRAW, waittime);
	if (ret < 0) {
		return ret;
	}
	vec->x = (((float) vecRAW.x) * (8.0/32768.0)) - (((float) mpu->accel->accelBasis.x) / (float) MPU_GRAVITY);
	vec->y = (((float) vecRAW.y) * (8.0/32768.0)) - (((float) mpu->accel->accelBasis.y) / (float) MPU_GRAVITY);
	vec->z = (((float) vecRAW.z) * (8.0/32768.0)) - (((float) mpu->accel->accelBasis.z) / (float) MPU_GRAVITY);
	return 0;
}
int32_t mpu9250_getGyro(struct mpu9250 *mpu, struct mpu9250_vector *vec, TickType_t waittime) {
	struct vector vecRAW;
	int32_t ret = gyro_get((struct gyro *) mpu->gyro, &vecRAW, waittime);
	if (ret < 0) {
		return ret;
	}
	vec->x = (((float) vecRAW.x) * (2000.0/32768.0)) - (((float) mpu->gyro->gyroBasis.x) / 131.);
	vec->y = (((float) vecRAW.y) * (2500.0/32768.0)) - (((float) mpu->gyro->gyroBasis.y) / 131.);
	vec->z = (((float) vecRAW.z) * (2500.0/32768.0)) - (((float) mpu->gyro->gyroBasis.z) / 131.);
	return 0;
}
