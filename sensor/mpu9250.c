#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>
#include <mpu9250.h>
#include <vec.h>
#include <spi.h>
#include <hal.h>



#define mpu9250_lock(u, w, e) HAL_LOCK(u, w, e)
#define mpu9250_unlock(u, e) HAL_UNLOCK(u, e)

static inline int32_t mpu9250_send(struct mpu9250 *mpu, uint8_t reg, uint8_t data, TickType_t waittime) {
	uint16_t wdata[] = {
		(uint16_t) reg, 
		(uint16_t) data
	};
	return spiSlave_send(mpu->slave, wdata, 2, waittime);
}

static int32_t mpu9250_recv(struct mpu9250 *mpu, uint8_t reg, uint8_t *data, uint32_t len, TickType_t waittime) {
	int32_t ret;
	uint32_t size = sizeof(uint16_t) * (len + 1);
	uint16_t *wdata = alloca(size);
	uint16_t *rdata = alloca(size);
	uint32_t i;
	memset(wdata, 0xFF, size);
	memset(rdata, 0x00, size);
	wdata[0] = MPU_READ | ((uint16_t) reg);
	ret = spiSlave_transver(mpu->slave, wdata, rdata, (len + 1), waittime);
	for (i = 1; i < (len + 1); i++) {
		data[i - 1] = (uint8_t) rdata[i];
	}
	return ret;
}

static inline int32_t mpu9250_clearSetBit(struct mpu9250 *mpu, uint8_t reg, uint8_t clear, uint8_t set, TickType_t waittime) {
	int32_t ret;
	uint8_t val;
	ret = mpu9250_recv(mpu, reg, &val, 1, waittime);
	if (ret < 0) {
		return -1;
	}
	val &= clear;
	val |= set;
	return mpu9250_send(mpu, reg, val, waittime);
}

static inline void mpu9250_buildVec(struct vector *vec, uint8_t *data) {
	vec->x = (((uint16_t) data[0]) << 8) | (((uint16_t) data[1]) << 0);
	vec->y = (((uint16_t) data[2]) << 8) | (((uint16_t) data[3]) << 0);
	vec->z = (((uint16_t) data[4]) << 8) | (((uint16_t) data[5]) << 0);
}

static int32_t mpu9250_calibrate(struct mpu9250 *mpu, TickType_t waittime) {
	int32_t i;
	int32_t ret = mpu9250_reset(mpu, waittime);
	if (ret < 0) {
		goto mpu9250_calibrate_error0;
	}
	/* Turn on internal clock source */
	ret = mpu9250_send(mpu, MPU_PWR_MGMT_1, 0x0, waittime);
	if (ret < 0) {
		goto mpu9250_calibrate_error0;
	}
	vTaskDelay(15 / portTICK_PERIOD_MS);
	/* Configure MPU9250 gyro and accelerometer for bias calculati */
	{
		uint16_t count;
		/* set low-pass filter to 188 Hz */
		ret = mpu9250_send(mpu, MPU_CONFIG, 0x01, waittime);
		if (ret < 0) {
			goto mpu9250_calibrate_error0;
		}
		/* Set sample rate to 1 kHz */
		ret = mpu9250_send(mpu, MPU_SMPLRT_DIV, 0x0, waittime);
		if (ret < 0) {
			goto mpu9250_calibrate_error0;
		}
		/* Set gyro full-scale to 250 degrees per second, maximum sensitivity */
		ret = mpu9250_send(mpu, MPU_GYRO_CONFIG, 0x0, waittime);
		if (ret < 0) {
			goto mpu9250_calibrate_error0;
		}
		/* Set accelerometer full-scale to 2 g, maximum sensitivity */
		ret = mpu9250_send(mpu, MPU_ACCEL_CONFIG, 0x0, waittime);
		if (ret < 0) {
			goto mpu9250_calibrate_error0;
		}
		{
			TickType_t lastWakeUpTime;
			/* Configure FIFO to capture accelerometer and gyro data for bias calculation */
			ret = mpu9250_send(mpu, MPU_USER_CTRL, MPU_USER_CTRL_FIFO_EN, waittime);
			if (ret < 0) {
				goto mpu9250_calibrate_error0;
			}
			lastWakeUpTime = xTaskGetTickCount();
			/* Enable gyro and accelerometer sensors for FIFO (max size 512 bytes in MPU-9250) */
			ret = mpu9250_send(
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
		ret = mpu9250_send(mpu, MPU_FIFO_EN, 0x0, waittime);
		if (ret < 0) {
			goto mpu9250_calibrate_error0;
		}
		/* 
		 * Get FIFO Counter
		 */
		{
			uint8_t countRAW[2];
			ret = mpu9250_recv(mpu, MPU_FIFO_COUNTH, countRAW, 2, waittime);
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
				ret = mpu9250_recv(mpu, MPU_FIFO_R_W, data, 12, waittime);
				if (ret < 0) {
					goto mpu9250_calibrate_error0;
				}
				mpu9250_buildVec(&accelVec, data);
				mpu9250_buildVec(&gyroVec, data + 6);
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
			ret = mpu9250_recv(mpu, MPU_FIFO_COUNTH, countRAW, 2, waittime);
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

struct mpu9250 *mpu9250_init(uint32_t index, TickType_t waittime) {
	int32_t ret;
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
	{
		struct spi *spi = spi_init(mpu->spi, SPI_3WIRE_CS, NULL);
		if (spi == NULL) {
			goto mpu9250_init_error1;
		}
		mpu->slave = spiSlave_init(spi, (struct spi_opt *) &mpu->opt);
	}
	ret = mpu9250_reset(mpu, waittime);
	if (ret < 0) {
		goto mpu9250_init_error1;
	}
	/* 
	 * Check MPU is alive
	 */
	{
		uint8_t val;
		ret = mpu9250_recv(mpu, MPU_WHO_AM_I, &val, 1, waittime);
		if (ret < 0) {
			goto mpu9250_init_error1;
		}
		if (val != MPU_WHO_AM_I_VAL) {
			goto mpu9250_init_error1;
		}
	}

	ret = mpu9250_calibrate(mpu, waittime);
	if (ret < 0) {
		goto mpu9250_init_error1;
	}

	ret = mpu9250_reset(mpu, waittime);
	if (ret < 0) {
		goto mpu9250_init_error1;
	}

	{
		ret = mpu9250_clearSetBit(
			mpu, 
			MPU_CONFIG, 
			MPU_CONFIG_DLPF_CFG(0x7), 
			MPU_CONFIG_DLPF_CFG(0x2), /* 98 */
			waittime
		);
		if (ret < 0) {
			goto mpu9250_init_error1;
		}
	}
	/* 
	 * Setup Sample Frequenz
	 */	
	{
		ret = mpu9250_send(mpu, MPU_SMPLRT_DIV, 0x1, waittime);
		if (ret < 0) {
			goto mpu9250_init_error1;
		}
	}
	/* 
	 * Setup Gyro Full Scale to +2000dps
	 */
	{
		ret = mpu9250_clearSetBit(
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
			goto mpu9250_init_error1;
		}
	}
	/* 
	 * Setup Accel Full Scale to +-8g
	 * TODO Confgiuerable over KConfig
	 */
	{
		ret = mpu9250_clearSetBit(
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
			goto mpu9250_init_error1;
		}
	}
	/* 
	 * Setup Accel to 1 kHz and 42 HZ bandwidth
	 */
#if 0
	{
		ret = mpu9250_clearSetBit(
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
			goto mpu9250_init_error1;
		}
	}
#endif
	return mpu;
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
	ret = mpu9250_send(mpu, MPU_PWR_MGMT_1, MPU_PWR_MGMT_1_H_RESET, waittime);
	if (ret < 0) {
		goto mpu9250_reset_error1;
	}
	vTaskDelay(100 / portTICK_PERIOD_MS);

	ret = mpu9250_send(mpu, MPU_PWR_MGMT_1, 0, waittime);
	if (ret < 0) {
		goto mpu9250_reset_error1;
	}
	vTaskDelay(100 / portTICK_PERIOD_MS);
	ret = mpu9250_send(mpu, MPU_PWR_MGMT_1, MPU_PWR_MGMT_1_CLKSEL(1), waittime);
	if (ret < 0) {
		goto mpu9250_reset_error1;
	}
	vTaskDelay(100 / portTICK_PERIOD_MS);
	/*
	 * Enable xyz from Gyro and Accsl
	 */
	ret = mpu9250_send(mpu, MPU_PWR_MGMT_2, 0x0, waittime);
	if (ret < 0) {
		goto mpu9250_reset_error1;
	}
	mpu9250_unlock(mpu, -1);
	return 0;
mpu9250_reset_error1:
	mpu9250_unlock(mpu, -1);
	return -1;
}

ACCEL_INIT(mpu9250, index) {
	int32_t ret;
	struct mpu9250_accel *accel = (struct mpu9250_accel *) ACCEL_GET_DEV(index);
	if (accel == NULL) {
		return NULL;
	}
	if (!accel->mpu->gen.init) {
		return NULL;
	}
	ret = accel_generic_init((struct accel *) accel);
	if (ret < 0) {
		return NULL;
	}
	if (ret > 0) {
		return (struct accel *) accel;
	}
	accel->gen.init = true;
	return (struct accel *) accel;
}
ACCEL_DEINIT(mpu9250, a) {
	struct mpu9250_accel *accel = (struct mpu9250_accel *) a;
	hal_deinit(accel);
	accel->gen.init = false;
	return 0;
}
ACCEL_GET(mpu9250, a, vec, waittime) {
	struct mpu9250_accel *accel = (struct mpu9250_accel *) a;
	struct mpu9250 *mpu = accel->mpu;
	int32_t ret;
	uint8_t data[6];
	accel_lock(accel, waittime, -1);
	mpu9250_lock(mpu, waittime, -1);
	ret = mpu9250_recv(mpu, MPU_ACCEL_XOUT_H, data, 6, waittime);
	if (ret < 0) {
		goto mpu9259_accel_get_error0;
	}
	mpu9250_unlock(mpu, -1);
	mpu9250_buildVec(vec, data);
	return 0;
mpu9259_accel_get_error0:
	mpu9250_unlock(mpu, -1);
	return -1;
}
ACCEL_GET_ISR(mpu9250, accel, vec) {
	/* TODO */
	return -1;
}
ACCEL_OPS(mpu9250);
GYRO_INIT(mpu9250, index) {
	int32_t ret;
	struct mpu9250_gyro *gyro = (struct mpu9250_gyro *) GYRO_GET_DEV(index);
	if (gyro == NULL) {
		return NULL;
	}
	if (!gyro->mpu->gen.init) {
		return NULL;
	}
	ret = gyro_generic_init((struct gyro *) gyro);
	if (ret < 0) {
		return NULL;
	}
	if (ret > 0) {
		return (struct gyro *) gyro;
	}
	gyro->gen.init = true;
	return (struct gyro *) gyro;
}
GYRO_DEINIT(mpu9250, g) {
	struct mpu9250_gyro *gyro = (struct mpu9250_gyro *) g;
	hal_deinit(gyro);
	gyro->gen.init = false;
	return 0;
}
GYRO_GET(mpu9250, g, vec, waittime) {
	struct mpu9250_gyro *gyro = (struct mpu9250_gyro *) g;
	struct mpu9250 *mpu = gyro->mpu;
	int32_t ret;
	uint8_t data[6];
	mpu9250_lock(mpu, waittime, -1);
	ret = mpu9250_recv(mpu, MPU_GYRO_XOUT_H, data, 6, waittime);
	if (ret < 0) {
		goto mpu9259_getAccel_error1;
	}
	mpu9250_unlock(mpu, -1);
	mpu9250_buildVec(vec, data);
	return 0;
mpu9259_getAccel_error1:
	mpu9250_unlock(mpu, -1);
	return -1;
}
GYRO_GET_ISR(mpu9250, gyro, vec) {
	/* TODO */
	return -1;
}
GYRO_OPS(mpu9250);
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
