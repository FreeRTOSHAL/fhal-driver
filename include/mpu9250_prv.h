#ifndef MPU9250_PRV_H_
#define MPU9250_PRV_H_
#include <stdint.h>
#include <mpu9250.h>
#include <vec.h>
#include <spi.h>
#include <hal.h>

#ifdef CONFIG_MPU9250_DEBUG
# define PRINTF(fmt, ...) printf("MPU: " fmt, ##__VA_ARGS__)
#else
# define PRINTF(fmt, ...)
#endif

#define mpu9250_lock(u, w, e) HAL_LOCK(u, w, e)
#define mpu9250_unlock(u, e) HAL_UNLOCK(u, e)
#ifdef CONFIG_ICM20948
static inline int32_t mpu_switchBank(struct mpu9250 *mpu, uint8_t bank, TickType_t waittime);
#endif

static inline int32_t mpu_send(struct mpu9250 *mpu, uint16_t reg, uint8_t data, TickType_t waittime) {
#ifdef CONFIG_ICM20948
	int ret;
	if ((reg >> 8) != 0 && (reg >> 8) != mpu->bank) {
		/* swtich bank is needed */
		ret = mpu_switchBank(mpu, reg >> 8, waittime);
		if (ret < 0) {
			return reg;
		}
	}
#endif
	{
		uint16_t wdata[] = {
			(reg & 0xFF), 
			(uint16_t) data
		};
		return spiSlave_send(mpu->slave, wdata, 2, waittime);
	}
}

static int32_t mpu_recv(struct mpu9250 *mpu, uint16_t reg, uint8_t *data, uint32_t len, TickType_t waittime) {
	int32_t ret;
	uint32_t size = sizeof(uint16_t) * (len + 1);
	uint16_t *wdata = alloca(size);
	uint16_t *rdata = alloca(size);
	uint32_t i;
#ifdef CONFIG_ICM20948
	if ((reg >> 8) != 0 && (reg >> 8) != mpu->bank) {
		/* swtich bank is needed */
		ret = mpu_switchBank(mpu, reg >> 8, waittime);
		if (ret < 0) {
			return reg;
		}
	}
#endif
	memset(wdata, 0xFF, size);
	memset(rdata, 0x00, size);
	wdata[0] = MPU_READ | (reg & 0xFF);
	ret = spiSlave_transfer(mpu->slave, wdata, rdata, (len + 1), waittime);
	for (i = 1; i < (len + 1); i++) {
		data[i - 1] = (uint8_t) rdata[i];
	}
	return ret;
}
#ifdef CONFIG_ICM20948
static inline int32_t mpu_switchBank(struct mpu9250 *mpu, uint8_t bank, TickType_t waittime) {
	return mpu_send(mpu, MPU_REG_BANK_SEL, MPU_REG_BANK_SEL_USER_BANK(bank - 1), waittime); 
}
#endif

static inline int32_t mpu_clearSetBit(struct mpu9250 *mpu, uint8_t reg, uint8_t clear, uint8_t set, TickType_t waittime) {
	int32_t ret;
	uint8_t val;
	ret = mpu_recv(mpu, reg, &val, 1, waittime);
	if (ret < 0) {
		return -1;
	}
	val &= clear;
	val |= set;
	return mpu_send(mpu, reg, val, waittime);
}
static inline void mpu_buildVec(struct vector *vec, uint8_t *data) {
	vec->x = (((uint16_t) data[0]) << 8) | (((uint16_t) data[1]) << 0);
	vec->y = (((uint16_t) data[2]) << 8) | (((uint16_t) data[3]) << 0);
	vec->z = (((uint16_t) data[4]) << 8) | (((uint16_t) data[5]) << 0);
}
#endif
