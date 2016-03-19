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
#ifdef CONFIG_SPI_THREAD_SAVE
	{
		int32_t ret = hal_init(s);
		if (ret < 0) {
			goto spi_genericInit_error0;
		}
	}
#endif
	s->init = true;
	return 0;
#ifdef CONFIG_SPI_THREAD_SAVE
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
int32_t spiSlave_transver(struct spi_slave *slave, uint16_t *sendData, uint16_t *recvData, uint32_t len, TickType_t waittime);
int32_t spiSlave_send(struct spi_slave *slave, uint16_t *data, uint32_t len, TickType_t waittime);
int32_t spiSlave_recv(struct spi_slave *slave, uint16_t *data, uint32_t len, TickType_t waittime);

int32_t spiSlave_transverISR(struct spi_slave *slave, uint16_t *sendData, uint16_t *recvData, uint32_t len);
int32_t spiSlave_sendISR(struct spi_slave *slave, uint16_t *data, uint32_t len);
int32_t spiSlave_recvISR(struct spi_slave *slave, uint16_t *data, uint32_t len);
#endif
