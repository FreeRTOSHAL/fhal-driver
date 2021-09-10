/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef SPI_PRV_H_
#define SPI_PRV_H_
#ifndef SPI_PRV
# error "Never include this file out of a SPI driver"
#endif
#include <stdbool.h>
#include <stdint.h>
#include <hal.h>
#define SPI_ALREDY_INITED 1
int32_t spi_genericInit(struct spi *spi);
#ifdef CONFIG_SPI_THREAD_SAFE
# define spi_lock(u, w, e) HAL_LOCK(u, w, e)
# define spi_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define spi_lock(u, w, e)
# define spi_unlock(u, e)
#endif
#define SPI_ADDDEV(ns, p) HAL_ADDDEV(spi, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(spi);
#define SPI_GET_DEV(index) HAL_GET_DEV(spi, index)
#ifndef CONFIG_SPI_MULTI
# define SPI_OPS(ns)
# define SPI_INIT_DEV(ns) 
# define SPI_INIT(ns, index, mode, opt) struct spi *spi_init(uint32_t index, enum spi_mode mode, struct spi_opt *opt)
# define SPI_DEINIT(ns, s) int32_t spi_deinit(struct spi *s)
# define SPI_SET_CALLBACK(ns, s, callback, data) int32_t spi_setCallback(struct spi *s, bool (*callback)(struct spi_slave *slave, void *data), void *data)

# define SPI_SLAVE_INIT(ns, s, options) struct spi_slave *spiSlave_init(struct spi *s, struct spi_opt *options)
# define SPI_SLAVE_DEINIT(ns, s) int32_t spiSlave_deinit(struct spi_slave *s)
# define SPI_SLAVE_TRANSFER(ns, s, sendData, recvData, len, waittime) int32_t spiSlave_transfer(struct spi_slave *s, uint16_t *sendData, uint16_t *recvData, uint32_t len, TickType_t waittime)
# define SPI_SLAVE_SEND(ns, s, data, len, waittime) int32_t spiSlave_send(struct spi_slave *s, uint16_t *data, uint32_t len, TickType_t waittime)
# define SPI_SLAVE_RECV(ns, s, data, len, waittime) int32_t spiSlave_recv(struct spi_slave *s, uint16_t *data, uint32_t len, TickType_t waittime)

# define SPI_SLAVE_TRANSFER_ISR(ns,s, sendData, rescvData, len) int32_t spiSlave_transferISR(struct spi_slave *s, uint16_t *sendData, uint16_t *recvData, uint32_t len)
# define SPI_SLAVE_SEND_ISR(ns,s, data, len) int32_t spiSlave_sendISR(struct spi_slave *s, uint16_t *data, uint32_t len)
# define SPI_SLAVE_RECV_ISR(ns, s, data, len) int32_t spiSlave_recvISR(struct spi_slave *s, uint16_t *data, uint32_t len)
#else
# define SPI_OPS(ns) static const struct spi_ops ns##_ops = { \
	.spi_init = &ns##_spi_init, \
	.spi_deinit = &ns##_spi_deinit, \
	.spi_setCallback = &ns##_spi_setCallback, \
	.spiSlave_init = &ns##_spiSlave_init, \
	.spiSlave_deinit = &ns##_spiSlave_deinit, \
	.spiSlave_transfer = &ns##_spiSlave_transfer, \
	.spiSlave_send = &ns##_spiSlave_send, \
	.spiSlave_recv = &ns##_spiSlave_recv, \
	.spiSlave_transferISR = ns##_spiSlave_transferISR, \
	.spiSlave_sendISR = ns##_spiSlave_sendISR, \
	.spiSlave_recvISR = ns##_spiSlave_recvISR, \
}
# define SPI_INIT_DEV(ns) .gen.ops = &ns##_ops,
# define SPI_INIT(ns, index, mode, opt) static struct spi *ns##_spi_init(uint32_t index, enum spi_mode mode, struct spi_opt *opt)
# define SPI_DEINIT(ns, s) static int32_t ns##_spi_deinit(struct spi *s)
# define SPI_SET_CALLBACK(ns, s, callback, data) static int32_t ns##_spi_setCallback(struct spi *s, bool (*callback)(struct spi_slave *slave, void *data), void *data)

# define SPI_SLAVE_INIT(ns, s, options) static struct spi_slave *ns##_spiSlave_init(struct spi *s, struct spi_opt *options)
# define SPI_SLAVE_DEINIT(ns, s) static int32_t ns##_spiSlave_deinit(struct spi_slave *s)
# define SPI_SLAVE_TRANSFER(ns, s, sendData, recvData, len, waittime) static int32_t ns##_spiSlave_transfer(struct spi_slave *s, uint16_t *sendData, uint16_t *recvData, uint32_t len, TickType_t waittime)
# define SPI_SLAVE_SEND(ns, s, data, len, waittime) static int32_t ns##_spiSlave_send(struct spi_slave *s, uint16_t *data, uint32_t len, TickType_t waittime)
# define SPI_SLAVE_RECV(ns, s, data, len, waittime) static int32_t ns##_spiSlave_recv(struct spi_slave *s, uint16_t *data, uint32_t len, TickType_t waittime)

# define SPI_SLAVE_TRANSFER_ISR(ns,s, sendData, rescvData, len) static int32_t ns##_spiSlave_transferISR(struct spi_slave *s, uint16_t *sendData, uint16_t *recvData, uint32_t len)
# define SPI_SLAVE_SEND_ISR(ns,s, data, len) static int32_t ns##_spiSlave_sendISR(struct spi_slave *s, uint16_t *data, uint32_t len)
# define SPI_SLAVE_RECV_ISR(ns, s, data, len) static int32_t ns##_spiSlave_recvISR(struct spi_slave *s, uint16_t *data, uint32_t len)
#endif
#endif
