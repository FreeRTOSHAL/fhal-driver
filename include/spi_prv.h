#ifndef SPI_PRV_H_
#define SPI_PRV_H_
#ifndef SPI_PRV
# error "Never include this file out of a SPI driver"
#endif
#include <stdbool.h>
#include <stdint.h>
#include <FreeRTOS.h>
#include <semphr.h>
#define SPI_ALREDY_INITED 1
struct spi_generic {
	bool init;
#ifndef CONFIG_SPI_THREAD_SAVE
	SemaphoreHandle_t lock;	
#endif
};
struct spi_prv {
	struct spi_generic generic;
};
int32_t spi_genericInit(struct spi *spi);
#ifdef CONFIG_SPI_THREAD_SAVE
inline int spi_lock(struct spi *spi, TickType_t waittime) {
	struct spi_prv *spi_prv = (struct spi_prv *) spi;
	return xSemaphoreTakeRecursive(spi_prv->generic.lock, waittime);
}
inline int spi_unlock(struct spi *spi) {
	struct spi_prv *spi_prv = (struct spi_prv *) spi;
	return xSemaphoreGiveRecursive(spi_prv->generic.lock);
}
#else
# define spi_lock(u, w) 1
# define spi_unlock(u) 1
#endif
#endif
