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
	ret = mpu_recv(mpu, MPU_ACCEL_XOUT_H, data, 6, waittime);
	if (ret < 0) {
		goto mpu9259_accel_get_error0;
	}
	mpu9250_unlock(mpu, -1);
	mpu_buildVec(vec, data);
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
