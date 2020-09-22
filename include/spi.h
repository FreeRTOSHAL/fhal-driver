/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef SPI_H_
#define SPI_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal.h>

/**
 * \defgroup SPI SPI Subsystem
 * \ingroup HAL
 * \code
 * #include <spi.h>
 * \endcode
 * 
 * This is the SPI Subsystem.
 * \{\}
 */
/**
 * Disable CS Contoll over GPIO 
 * \ingroup SPI_SLAVE
 */
#define SPI_OPT_GPIO_DIS 0xFF
/**
 * Disable CS Contoll over Contoller
 * \ingroup SPI_SLAVE
 */
#define SPI_OPT_CS_DIS 0x7
/**
 * Settings for SPI Slave
 * \ingroup SPI_SLAVE
 */
struct spi_opt {
	/**
	 * Slave send the data in LSB
	 */
	bool lsb;
	/**
	 * CPOL = false Clock Idle Low
	 * CPOL = true Clock Idel High
	 */
	bool cpol;
	/**
	 * CPHA = false Data on rising 
	 * CPHA = true Data on falling
	 */
	bool cpha;
	/**
	 * CS ID contolled by SPI Contoller
	 * Disable if value is SPI_OPT_CS_DIS
	 */
	uint8_t cs;
	/**
	 * CS is Low Inactive
	 */
	bool csLowInactive;
	/**
	 * CS GPIO Pin Index
	 * CS is Contolled by GPIO Pin
	 * Ignored if cs not SPI_OPT_CS_DIS 
	 * Disabled if value is SPI_OPT_GPIO_DIS
	 */
	uint16_t gpio;
	/**
	 * Size of Frame
	 * 1 - 16
	 * 0 illegal Value
	 */
	uint8_t size;
	/**
	 * Delay in us between Write Cycle
	 */
	uint32_t wdelay;
	/**
	 * Holt Time in us CS after Transfer 
	 */
	uint32_t cs_hold;
	/**
	 * Time in us CS is set and data Transfer start
	 */
	uint32_t cs_delay;
	/**
	 * Baut in HZ
	 */
	uint32_t bautrate;
};
/**
 * SPI Mode
 * \ingroup SPI
 */
enum spi_mode {
	/**
	 * SPI is in 3 Wire mode, MISO, MOSI and CLK
	 */
	SPI_3WIRE,
	/**
	 * SPI is in 3 Wire mode and multiply CS, MISO, MOSI, CLK and CS
	 */
	SPI_3WIRE_CS,
	/**
	 * SPI is in 3 Wire mode, multiply CS and PCS, MISO, MOSI, CLK and CS
	 */
	SPI_3WIRE_CS_PCS,
	/**
	 * SPI is in Slave Mode 
	 * TODO Slave mode
	 */
	SPI_SLAVE
};
/**
 * Private Structure of SPI driver
 * \ingroup SPI
 */
struct spi;
/**
 * Private Structure of SPI Slave
 * \ingroup SPI
 */
struct spi_slave;
#ifdef CONFIG_SPI_MULTI
/**
 * Function of SPI driver in Multi Driver implementation mode
 * \ingroup SPI
 */
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
/**
 * Generic SPI Interface  
 * \ingroup SPI
 */
struct spi_generic {
	/**
	 * true = is init
	 * false = is not init
	 */
	bool init;
#ifdef CONFIG_INSTANCE_NAME
	/**
	 * Name of Driver Instance for Debugging 
	 */
	const char *name;
#endif
#ifdef CONFIG_SPI_THREAD_SAVE
	/**
	 * Mutex
	 */
	OS_DEFINE_MUTEX_RECURSIVE(lock);
#endif
#ifdef CONFIG_SPI_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct spi_ops *ops;
#endif
};

/**
 * Generic SPI Slave Interface  
 * \ingroup SPI_SLAVE
 */
struct spi_slave_generic {
	/**
	 * SPI driver Instance
	 */
	struct spi *spi;
};

#ifndef CONFIG_SPI_MULTI
/**
 * Init SPI 
 * 
 * \ingroup SPI
 * \param index Index of SPI
 * \param mode Mode of Driver
 * \param opt in slave mode
 * \return instance of driver or NULL on error
 */
struct spi *spi_init(uint32_t index, enum spi_mode mode, struct spi_opt *opt);
/**
 * Deinit SPI instance
 * \warning SPI Slave are not deinit automatically 
 * \ingroup SPI
 * \param spi SPI instance
 * \return -1 on error 0 on ok
 */
int32_t spi_deinit(struct spi *spi);
/** 
 * Set Callback for Slave Mode
 * \ingroup SPI
 * \param spi SPI instance
 * \param callback Callback
 * \param data Data parsed to Callback
 * \return -1 on error 0 on ok
 */
int32_t spi_setCallback(struct spi *spi, bool (*callback)(struct spi_slave *slave, void *data), void *data);
/**
 * \defgroup SPI_SLAVE SPI Slave Subsystem
 * \ingroup SPI
 * \code
 * #include <spi.h>
 * \endcode
 * 
 * This is the SPI Slave Subsystem.
 * \{
 */
/**
 * Slave init
 * Init one Slave on a SPI Bus
 * 
 * \warning This Function did not check collision between instances
 * \param spi Spi Instance
 * \param options Option to acces the slave
 * \return SPI Slave Instance or Null on error
 */
struct spi_slave *spiSlave_init(struct spi *spi, struct spi_opt *options);
/**
 * Deinit Slave
 * \param slave Slave instance
 * \return -1 on error 0 on ok
 */
int32_t spiSlave_deinit(struct spi_slave *slave);
/**
 * Transfer Data to slave and recv Data from Slave
 * \warning sendData and recvData shall have the same length!
 * \param slave Slave instance
 * \param sendData Data
 * \param recvData Data
 * \param len Length of Data
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t spiSlave_transver(struct spi_slave *slave, uint16_t *sendData, uint16_t *recvData, uint32_t len, TickType_t waittime);
/**
 * Send Data to Slave
 * \param slave Slave instance
 * \param data Data
 * \param len Length of Data
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t spiSlave_send(struct spi_slave *slave, uint16_t *data, uint32_t len, TickType_t waittime);
/**
 * Recv Data from Slave
 * \param slave Slave instance
 * \param data Data
 * \param len Length of Data
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t spiSlave_recv(struct spi_slave *slave, uint16_t *data, uint32_t len, TickType_t waittime);
/**
 * Transfer Data to slave and recv Data from Slave in ISR
 * \warning sendData and recvData shall have the same length!
 * \param slave Slave instance
 * \param sendData Data
 * \param recvData Data
 * \param len Length of Data
 * \return -1 on error 0 on ok
 */
int32_t spiSlave_transverISR(struct spi_slave *slave, uint16_t *sendData, uint16_t *recvData, uint32_t len);
/**
 * Send Data to Slave in ISR
 * \param slave Slave instance
 * \param data Data
 * \param len Length of Data
 * \return -1 on error 0 on ok
 */
int32_t spiSlave_sendISR(struct spi_slave *slave, uint16_t *data, uint32_t len);
/**
 * Recv Data from Slave in ISR
 * \param slave Slave instance
 * \param data Data
 * \param len Length of Data
 * \return -1 on error 0 on ok
 */
int32_t spiSlave_recvISR(struct spi_slave *slave, uint16_t *data, uint32_t len);
/**\}*/
#else
inline struct spi *spi_init(uint32_t index, enum spi_mode mode, struct spi_opt *opt) {
	HAL_DEFINE_GLOBAL_ARRAY(spi);
	struct spi_generic *s = (struct spi_generic *) HAL_GET_DEV(spi, index);
	if (s == NULL) {
		return NULL;
	}
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
/**\}*/
#endif
