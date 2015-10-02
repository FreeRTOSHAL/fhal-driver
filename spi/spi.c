#include <FreeRTOS.h>
#include <task.h>
#include <stdbool.h>
#include <stdint.h>
#include <spi.h>
#define SPI_PRV
#include <spi_prv.h>


int32_t spi_genericInit(struct spi *spi) {
	struct spi_prv *spi_prv = (struct spi_prv *) spi;
	if(spi_prv->generic.init) {
		return SPI_ALREDY_INITED;
	}
#ifdef CONFIG_SPI_THREAD_SAVE
	{
		spi_prv->generic.lock = xSemaphoreCreateRecursiveMutex();
		if (spi_prv->generic.lock == NULL) {
			goto spi_genericInit_error0;
		}
	}
#endif
	spi_prv->generic.init = true;
	return 0;
#ifdef CONFIG_SPI_THREAD_SAVE
spi_genericInit_error0:
	return -1;
#endif
}
#ifdef CONFIG_SPI_THREAD_SAVE
int spi_lock(struct spi *spi, TickType_t waittime);
int spi_unlock(struct spi *spi);
#endif
