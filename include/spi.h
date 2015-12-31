#ifndef SPI_H_
#define SPI_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal.h>

#define SPI_OPT_GPIO_DIS 0xFF
#define SPI_OPT_CS_DIS 0x7

struct spi_opt {
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

enum spi_mode {
	SPI_3WIRE,
	SPI_3WIRE_CS,
	SPI_3WIRE_CS_PCS,
	SPI_SLAVE
};

struct spi;
struct spi_slave;
#ifdef CONFIG_SPI_MULTI
struct spi_ops {
	struct spi *(*spi_init)(uint32_t index, enum spi_mode mode, struct spi_opt *opt);
	int32_t (*spi_deinit)(struct spi *spi);
	int32_t (*spi_setCallback)(struct spi *spi, bool (*callback)(struct spi_slave *slave, void *data), void *data);

	struct spi_slave *(*spiSlave_init)(struct spi *spi, struct spi_opt *options);
	int32_t (*spiSlave_deinit)(struct spi_slave *slave);
	int32_t (*spiSlave_transver)(struct spi_slave *slave, uint16_t *sendData, uint16_t *recvData, uint32_t len, TickType_t waittime);
	int32_t (*spiSlave_send)(struct spi_slave *slave, uint16_t *data, uint32_t len, TickType_t waittime);
	int32_t (*spiSlave_recv)(struct spi_slave *slave, uint16_t *data, uint32_t len, TickType_t waittime);

	int32_t (*spiSlave_transverISR)(struct spi_slave *slave, uint16_t *sendData, uint16_t *recvData, uint32_t len);
	int32_t (*spiSlave_sendISR)(struct spi_slave *slave, uint16_t *data, uint32_t len);
	int32_t (*spiSlave_recvISR)(struct spi_slave *slave, uint16_t *data, uint32_t len);
	};
#endif

struct spi_generic {
	bool init;
#ifdef CONFIG_SPI_THREAD_SAVE
	SemaphoreHandle_t lock;	
#endif
#ifdef CONFIG_SPI_MULTI
	const struct spi_ops *ops;
#endif
};

struct spi_slave_generic {
	struct spi *spi;
};


extern struct spi **spis;

#ifndef CONFIG_SPI_MULTI
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
#else
inline struct spi *spi_init(uint32_t index, enum spi_mode mode, struct spi_opt *opt) {
	struct spi_generic *s = (struct spi_generic *) spis[index];
	return s->ops->spi_init(index, mode, opt);
}
inline int32_t spi_deinit(struct spi *spi) {
	struct spi_generic *s = (struct spi_generic *) spi;
	return s->ops->spi_deinit(spi);
}
inline int32_t spi_setCallback(struct spi *spi, bool (*callback)(struct spi_slave *slave, void *data), void *data) {
	struct spi_generic *s = (struct spi_generic *) spi;
	return s->ops->spi_setCallback(spi, callback, data);
}

inline struct spi_slave *spiSlave_init(struct spi *spi, struct spi_opt *options) {
	struct spi_generic *s = (struct spi_generic *) spi;
	return s->ops->spiSlave_init(spi, options);
}
inline int32_t spiSlave_deinit(struct spi_slave *slave) {
	struct spi_slave_generic *sp = (struct spi_slave_generic *) slave;
	struct spi_generic *s = (struct spi_generic *) sp->spi;
	return s->ops->spiSlave_deinit(slave);
}
inline int32_t spiSlave_transver(struct spi_slave *slave, uint16_t *sendData, uint16_t *recvData, uint32_t len, TickType_t waittime) {
	struct spi_slave_generic *sp = (struct spi_slave_generic *) slave;
	struct spi_generic *s = (struct spi_generic *) sp->spi;
	return s->ops->spiSlave_transver(slave, sendData, recvData, len, waittime);
}
inline int32_t spiSlave_send(struct spi_slave *slave, uint16_t *data, uint32_t len, TickType_t waittime) {
	struct spi_slave_generic *sp = (struct spi_slave_generic *) slave;
	struct spi_generic *s = (struct spi_generic *) sp->spi;
	return s->ops->spiSlave_send(slave, data, len, waittime);
}
inline int32_t spiSlave_recv(struct spi_slave *slave, uint16_t *data, uint32_t len, TickType_t waittime) {
	struct spi_slave_generic *sp = (struct spi_slave_generic *) slave;
	struct spi_generic *s = (struct spi_generic *) sp->spi;
	return s->ops->spiSlave_recv(slave, data, len, waittime);
}

inline int32_t spiSlave_transverISR(struct spi_slave *slave, uint16_t *sendData, uint16_t *recvData, uint32_t len) {
	struct spi_slave_generic *sp = (struct spi_slave_generic *) slave;
	struct spi_generic *s = (struct spi_generic *) sp->spi;
	return s->ops->spiSlave_transverISR(slave, sendData, recvData, len);
}
inline int32_t spiSlave_sendISR(struct spi_slave *slave, uint16_t *data, uint32_t len) {
	struct spi_slave_generic *sp = (struct spi_slave_generic *) slave;
	struct spi_generic *s = (struct spi_generic *) sp->spi;
	return s->ops->spiSlave_sendISR(slave, data, len);
}
inline int32_t spiSlave_recvISR(struct spi_slave *slave, uint16_t *data, uint32_t len) {
	struct spi_slave_generic *sp = (struct spi_slave_generic *) slave;
	struct spi_generic *s = (struct spi_generic *) sp->spi;
	return s->ops->spiSlave_recvISR(slave, data, len);
}
#endif
#endif
