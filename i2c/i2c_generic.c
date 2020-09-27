/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2018
 */
#include <i2c.h>
#define I2C_PRV
#include <i2c_prv.h>

int32_t i2c_generic_init(struct i2c *t) {
	struct i2c_generic *i2c = (struct i2c_generic *) t;
	if (hal_isInit(i2c)) {
		return I2C_ALREDY_INITED;
	}
	i2c->init = true;
	return 0;
}
#ifdef CONFIG_CAPTURE_MULTI
	struct i2c *i2c_init(uint32_t index, enum i2c_mode mode);
	int32_t i2c_deinit(struct i2c *i2c);
	int32_t i2c_send(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len, TickType_t waittime);
	int32_t i2c_recv(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len, TickType_t waittime);
	int32_t i2c_transver(struct i2c *i2c, uint16_t id, uint8_t *sendData, uint32_t sendLen, uint8_t *recvData, uint32_t recvLen, TickType_t waittime);
	int32_t i2c_sendISR(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len);
	int32_t i2c_recvISR(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len);
	int32_t i2c_transverISR(struct i2c *i2c, uint16_t id, uint8_t *sendData, uint32_t sendLen, uint8_t *recvData, uint32_t recvLen);
#endif
