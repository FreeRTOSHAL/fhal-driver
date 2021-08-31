/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <FreeRTOS.h>
#include <task.h>
#include <stdbool.h>
#include <stdint.h>
#include <spi.h>
#define SPI_PRV
#include <spi_prv.h>

int32_t spi_genericInit(struct spi *spi) {
	struct spi_generic * s = (struct spi_generic *) spi;
	if(hal_isInit(s)) {
		return SPI_ALREDY_INITED;
	}
#ifdef CONFIG_SPI_THREAD_SAFE
	{
		int32_t ret = hal_init(s);
		if (ret < 0) {
			goto spi_genericInit_error0;
		}
	}
#endif
	s->init = true;
	return 0;
#ifdef CONFIG_SPI_THREAD_SAFE
spi_genericInit_error0:
	return -1;
#endif
}
#ifdef CONFIG_SPI_MULTI
struct spi *spi_init(uint32_t index, enum spi_mode mode, struct spi_opt *opt);
int32_t spi_deinit(struct spi *spi);
int32_t spi_setCallback(struct spi *spi, bool (*callback)(struct spi_slave *slave, void *data), void *data);

struct spi_slave *spiSlave_init(struct spi *spi, struct spi_opt *options);
int32_t spiSlave_deinit(struct spi_slave *slave);
int32_t spiSlave_transfer(struct spi_slave *slave, uint16_t *sendData, uint16_t *recvData, uint32_t len, TickType_t waittime);
int32_t spiSlave_send(struct spi_slave *slave, uint16_t *data, uint32_t len, TickType_t waittime);
int32_t spiSlave_recv(struct spi_slave *slave, uint16_t *data, uint32_t len, TickType_t waittime);

int32_t spiSlave_transferISR(struct spi_slave *slave, uint16_t *sendData, uint16_t *recvData, uint32_t len);
int32_t spiSlave_sendISR(struct spi_slave *slave, uint16_t *data, uint32_t len);
int32_t spiSlave_recvISR(struct spi_slave *slave, uint16_t *data, uint32_t len);
#endif
