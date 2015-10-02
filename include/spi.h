#ifndef SPI_H_
#define SPI_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>

#define SPI_OPT_GPIO_DIS 0xFF
#define SPI_OPT_CS_DIS 0x7

struct spi_ops{
	bool lsb;
	bool cpol;
	bool cpha;
	uint8_t cs;
	bool csLowInactive;
	uint16_t gpio;
	uint8_t size;
	uint32_t wdelay;
	uint32_t cs_hold;
	uint32_t cs_delay;
	uint32_t bautrate;
};

struct spi;
struct spi_slave;
struct spi *spi_init(uint32_t index);
int32_t spi_deinit(struct spi *spi);
struct spi_slave *spi_slave(struct spi *spi, struct spi_ops *options);
int32_t spi_sendRecv(struct spi_slave *slave, uint16_t *sendData, uint16_t *recvData, uint32_t len, TickType_t waittime);
int32_t spi_send(struct spi_slave *slave, uint16_t *data, uint32_t len, TickType_t waittime);
int32_t spi_recv(struct spi_slave *slave, uint16_t *data, uint32_t len, TickType_t waittime);
#endif
