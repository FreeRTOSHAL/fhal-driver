/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2020
 */
#ifndef I2C_H_
#define I2C_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal.h>
/**
 * \defgroup I2C I2C Subsystem
 * \ingroup HAL
 * \code
 * #include <i2c.h>
 * \endcode
 *
 * This is the I2C Subsystem for controlling I2C of a SOC. 
 * \{
 */
/**
 * Private Structure of I2C driver
 */
struct i2c;
/**
 * I2C Mode / Speed Config
 */
enum i2c_mode {
	/**
	 * Standard Mode (100 Kbit/s)
	 */
	I2C_SM,
	/**
	 * Fast Mode (400 Kbit/s)
	 */
	I2C_FM,
	/**
	 *Fast Mode Plus (1 Mbit/s)
	 */
	I2C_FMP,
	/**
	 * High Speed Mode (3.4 Mbit/s)
	 */
	I2C_HSM,
	/**
	 * Ultra Fast-mode (5.0 Mbit/s)
	 */
	i2C_UFM,
};
#ifdef CONFIG_I2C_MULTI
/**
 * Function of I2C driver in Multi Driver implementation mode 
 */
struct i2c_ops {
	struct i2c *(*i2c_init)(uint32_t index, enum i2c_mode mode);
	int32_t (*i2c_deinit)(struct i2c *i2c);
	int32_t (*i2c_send)(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len, TickType_t waittime);
	int32_t (*i2c_recv)(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len, TickType_t waittime);
	int32_t (*i2c_transfer)(struct i2c *i2c, uint16_t id, uint8_t *sendData, uint32_t sendLen, uint8_t *recvData, uint32_t recvLen, TickType_t waittime);
	int32_t (*i2c_sendISR)(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len);
	int32_t (*i2c_recvISR)(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len);
	int32_t (*i2c_transferISR)(struct i2c *i2c, uint16_t id, uint8_t *sendData, uint32_t sendLen, uint8_t *recvData, uint32_t recvLen);
};
#endif
/**
 * Generic I2C Interface  
 */
struct i2c_generic {
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
#ifdef CONFIG_I2C_THREAD_SAVE
	/**
	 * Mutex
	 */
	OS_DEFINE_MUTEX_RECURSIVE(lock);
#endif
#ifdef CONFIG_I2C_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct i2c_ops *ops;
#endif
};
#ifndef CONFIG_I2C_MULTI
/**
 * init I2C instance
 * \param index Index of I2C
 * \param mode I2C Mode
 * \return I2C Instance NULL on Error 
 */
struct i2c *i2c_init(uint32_t index, enum i2c_mode mode);

/**
 * Deinit I2C
 * \param i2c I2C instance
 * \return -1 on error 0 on ok
 */
int32_t i2c_deinit(struct i2c *i2c);
/**
 * Send Bytes
 * \param i2c I2C instance
 * \param id ID if id > 7-Bit Exended IDs is used
 * \param data Data
 * \param len
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t i2c_send(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len, TickType_t waittime);
/**
 * Recv Bytes
 * \param i2c I2C instance
 * \param id ID if id > 7-Bit Exended IDs is used
 * \param data Data
 * \param len len
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t i2c_recv(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len, TickType_t waittime);
/**
 * Transver Send and Recv
 *
 * \param i2c I2C instance
 * \param id ID if id > 7-Bit Exended IDs is used
 * \param sendData Send Data
 * \param sendLen Send len
 * \param recvdData Recv Data
 * \param recvLen Recv len
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t i2c_transfer(struct i2c *i2c, uint16_t id, uint8_t *sendData, uint32_t sendLen, uint8_t *recvData, uint32_t recvLen, TickType_t waittime);
/**
 * Send Bytes in Interrupt
 * \param i2c I2C instance
 * \param id ID if id > 7-Bit Exended IDs is used
 * \param data Data
 * \param len
 * \return -1 on error 0 on ok
 */
int32_t i2c_sendISR(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len);
/**
 * Recv Bytes in Interrupt
 * \param i2c I2C instance
 * \param id ID if id > 7-Bit Exended IDs is used
 * \param data Data
 * \param len len
 * \return -1 on error 0 on ok
 */
int32_t i2c_recvISR(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len);
/**
 * Transver Send and Recv in Interrupt
 *
 * \param i2c I2C instance
 * \param id ID if id > 7-Bit Exended IDs is used
 * \param sendData Send Data
 * \param sendLen Send len
 * \param recvdData Recv Data
 * \param recvLen Recv len
 * \return -1 on error 0 on ok
 */
int32_t i2c_transferISR(struct i2c *i2c, uint16_t id, uint8_t *sendData, uint32_t sendLen, uint8_t *recvData, uint32_t recvLen);

#else
inline struct i2c *i2c_init(uint32_t index, enum i2c_mode mode) {
	HAL_DEFINE_GLOBAL_ARRAY(i2c);
	struct i2c_generic *a = (struct i2c_generic *) HAL_GET_DEV(i2c, index);
	if (a == NULL) {
		return NULL;
	}
	return a->ops->i2c_init(index, mode);
}
inline int32_t i2c_deinit(struct i2c *i2c) {
	struct i2c_generic *a = (struct i2c_generic *) i2c;
	return a->ops->i2c_deinit(i2c);
}
inline int32_t i2c_send(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len, TickType_t waittime) {
	struct i2c_generic *a = (struct i2c_generic *) i2c;
	return a->ops->i2c_send(i2c, id, data, len, waittime);
}
inline int32_t i2c_recv(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len, TickType_t waittime) {
	struct i2c_generic *a = (struct i2c_generic *) i2c;
	return a->ops->i2c_recv(i2c, id, data, len, waittime);
}
inline int32_t i2c_transfer(struct i2c *i2c, uint16_t id, uint8_t *sendData, uint32_t sendLen, uint8_t *recvData, uint32_t recvLen, TickType_t waittime) {
	struct i2c_generic *a = (struct i2c_generic *) i2c;
	return a->ops->i2c_transfer(i2c, id, sendData, sendLen, recvData, recvLen, waittime);
}
inline int32_t i2c_sendISR(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len) {
	struct i2c_generic *a = (struct i2c_generic *) i2c;
	return a->ops->i2c_sendISR(i2c, id, data, len);
}
inline int32_t i2c_recvISR(struct i2c *i2c, uint16_t id, uint8_t *data, uint32_t len) {
	struct i2c_generic *a = (struct i2c_generic *) i2c;
	return a->ops->i2c_recvISR(i2c, id, data, len);
}
inline int32_t i2c_transferISR(struct i2c *i2c, uint16_t id, uint8_t *sendData, uint32_t sendLen, uint8_t *recvData, uint32_t recvLen) {
	struct i2c_generic *a = (struct i2c_generic *) i2c;
	return a->ops->i2c_transferISR(i2c, id, sendData, sendLen, recvData, recvLen);
}
#endif
/**\}*/
#endif

