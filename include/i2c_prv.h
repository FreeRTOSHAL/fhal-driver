/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2018
 */
#ifndef I2C_PRV_
#define I2C_PRV_
#ifndef I2C_PRV
# error "Never include this file out of a I2C driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#define I2C_ALREDY_INITED 1
int32_t i2c_genericInit(struct i2c *i2c);
#ifdef CONFIG_I2C_THREAD_SAVE
# define i2c_lock(u, w, e) HAL_LOCK(u, w, e)
# define i2c_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define i2c_lock(u, w, e)
# define i2c_unlock(u, e)
#endif
#define I2C_ADDDEV(ns, p) HAL_ADDDEV(i2c, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(i2c);
#define I2C_GET_DEV(index) HAL_GET_DEV(i2c, index)
#ifndef CONFIG_I2C_MULTI
# define I2C_OPS(ns)
# define I2C_INIT_DEV(ns) 
# define I2C_INIT(ns, index, mode) struct i2c *i2c_init(uint32_t index, enum i2c_mode mode)
# define I2C_DEINIT(ns, i) int32_t  i2c_deinit(struct i2c *i)
# define I2C_SEND(ns, i, id, data, len, waittime) int32_t i2c_send(struct i2c *i, uint16_t id, uint8_t *data, uint32_t len, TickType_t waittime)
# define I2C_RECV(ns, i, id, data, len, waittime) int32_t i2c_recv(struct i2c *i, uint16_t id, uint8_t *data, uint32_t len, TickType_t waittime)
# define I2C_TRANSVER(ns, i, id, sendData, sendLen, recvData, recvLen, waittime) int32_t i2c_transfer(struct i2c *i, uint16_t id, uint8_t *sendData, uint32_t sendLen, uint8_t *recvData, uint32_t recvLen, TickType_t waittime)
# define I2C_SEND_ISR(ns, i, id, data, len) int32_t i2c_sendISR(struct i2c *i, uint16_t id, uint8_t *data, uint32_t len);
# define I2C_RECV_ISR(ns, i, id, data, len) int32_t i2c_recvISR(struct i2c *i, uint16_t id, uint8_t *data, uint32_t len);
# define I2C_TRANSVER_ISR(ns, i, id, sendData, sendLen, recvData, recvLen) int32_t i2c_transferISR(struct i2c *i, uint16_t id, uint8_t *sendData, uint32_t sendLen, uint8_t *recvData, uint32_t recvLen);
#else
# define I2C_OPS(ns) static const struct i2c_ops ns##_i2c_ops = { \
	.i2c_init = &ns##_i2c_init,\
	.i2c_deinit = &ns##_i2c_deinit,\
	.i2c_send = &ns##_i2c_send, \
	.i2c_recv = &ns##_i2c_recv, \
	.i2c_transfer = &ns##_i2c_transfer, \
	.i2c_sendISR = &ns##_i2c_sendISR, \
	.i2c_recvISR = &ns##_i2c_recvISR, \
	.i2c_transferISR = &ns##_i2c_transferISR, \
}
# define I2C_INIT_DEV(ns) .gen.ops = &ns##_i2c_ops,
# define I2C_INIT(ns, index, mode) struct i2c *ns##_i2c_init(uint32_t index, enum i2c_mode mode)
# define I2C_DEINIT(ns, i) int32_t  ns##_i2c_deinit(struct i2c *i)
# define I2C_SEND(ns, i, id, data, len, waittime) int32_t ns##_i2c_send(struct i2c *i, uint16_t id, uint8_t *data, uint32_t len, TickType_t waittime)
# define I2C_RECV(ns, i, id, data, len, waittime) int32_t ns##_i2c_recv(struct i2c *i, uint16_t id, uint8_t *data, uint32_t len, TickType_t waittime)
# define I2C_TRANSVER(ns, i, id, sendData, sendLen, recvData, recvLen, waittime) int32_t i2c_transfer(struct i2c *i, uint16_t id, uint8_t *sendData, uint32_t sendLen, uint8_t *recvData, uint32_t recvLen, TickType_t waittime)
# define I2C_SEND_ISR(ns, i, id, data, len) int32_t ns##_i2c_sendISR(struct i2c *i, uint16_t id, uint8_t *data, uint32_t len);
# define I2C_RECV_ISR(ns, i, id, data, len) int32_t i2c_recvISR(struct i2c *i, uint16_t id, uint8_t *data, uint32_t len)
# define I2C_TRANSVER_ISR(ns, i, id, sendData, sendLen, recvData, recvLen) int32_t ns##_i2c_transferISR(struct i2c *i, uint16_t id, uint8_t *sendData, uint32_t sendLen, uint8_t *recvData, uint32_t recvLen)
#endif
#endif

