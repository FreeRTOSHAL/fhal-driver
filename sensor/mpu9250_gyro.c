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
	ret = mpu_recv(mpu, MPU_GYRO_XOUT_H, data, 6, waittime);
	if (ret < 0) {
		goto mpu9259_getAccel_error1;
	}
	mpu9250_unlock(mpu, -1);
	mpu_buildVec(vec, data);
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
