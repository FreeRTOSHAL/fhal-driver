/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef UART_H_
#define UART_H_
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <hal.h>
/**
 * \defgroup UART UART Subsystem
 * \ingroup HAL
 * \code
 * #include <uart.h>
 * \endcode
 * 
 * This is the UART Subsystem for controlling UART of a SOC. 
 * \{
 */
/**
 * Private Structure of UART driver
 */
struct uart;
#ifdef CONFIG_UART_MULTI
/**
 * Function of UART driver in Multi Driver implementation mode 
 */
struct uart_ops {
	struct uart *(*uart_init)(uint8_t port, uint32_t baudrate);
	int32_t (*uart_deinit)(struct uart *uart);
	char (*uart_getc)(struct uart *uart, TickType_t waittime);
	int32_t (*uart_putc)(struct uart *uart, char c, TickType_t waittime);
# ifndef CONFIG_UART_GENERIC_BYTE
	int32_t (*uart_read)(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime);
	int32_t (*uart_write)(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime);
# endif
# ifndef CONFIG_UART_GENERIC_STRING
	int32_t (*uart_puts)(struct uart *uart, char *s, TickType_t waittime);
# endif

	char (*uart_getcISR)(struct uart *uart);
	int32_t (*uart_putcISR)(struct uart *uart, char c);
# ifndef CONFIG_UART_GENERIC_BYTE
	int32_t (*uart_readISR)(struct uart *uart, uint8_t *data, size_t length);
	int32_t (*uart_writeISR)(struct uart *uart, uint8_t *data, size_t length);
# endif
# ifndef CONFIG_UART_GENERIC_STRING
	int32_t (*uart_putsISR)(struct uart *uart, char *s);
# endif
};
#endif

/**
 * Generic UART Interface  
 */
struct uart_generic {
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
#ifdef CONFIG_UART_THREAD_SAFE
	/**
	 * Mutex
	 */
	OS_DEFINE_MUTEX_RECURSIVE(lock);
#endif
#ifdef CONFIG_UART_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct uart_ops *ops;
#endif
};
#ifndef CONFIG_UART_MULTI
/**
 * Init UART Instance
 * \param port Index of UART
 * \param baudrate UART Baudrate if baudrate == 0 only get Instance
 * \return UART Instance NULL on error
 */
struct uart *uart_init(uint8_t port, uint32_t baudrate);
/**
 * Deinit UART Instance
 * \param uart UART Instance 
 * \return -1 on error 0 on ok
 */
int32_t uart_deinit(struct uart *uart);
/**
 * reads the next character from UART
 * \param uart UART Instance 
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return character
 */
char uart_getc(struct uart *uart, TickType_t waittime);
/**
 * wirte character on UART 
 * \param uart UART Instance 
 * \param c character
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t uart_putc(struct uart *uart, char c, TickType_t waittime);
/**
 * Read Data form UART
 * \param uart UART Instance 
 * \param data Data
 * \param length Length of Data
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error >= 0 length read from UART
 */
int32_t uart_read(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime);
/**
 * Write Data on UART
 * \param uart UART Instance 
 * \param data Data
 * \param length Length of Data
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error >= 0 length write on UART
 */
int32_t uart_write(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime);
/**
 * Write String on UART and a trailing newline
 * \param uart UART Instance 
 * \param s String
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error >= 0 length write on UART
 */
int32_t uart_puts(struct uart *uart, char *s, TickType_t waittime);
/**
 * reads the next character from UART in ISR
 * \param uart UART Instance 
 * \return character
 */
char uart_getcISR(struct uart *uart);
/**
 * wirte character on UART in ISR
 * \param uart UART Instance 
 * \param c character
 * \return -1 on error 0 on ok
 */
int32_t uart_putcISR(struct uart *uart, char c);
/**
 * Read Data form UART in ISR
 * \param uart UART Instance 
 * \param data Data
 * \param length Length of Data
 * \return -1 on error >= 0 length read from UART
 */
int32_t uart_readISR(struct uart *uart, uint8_t *data, size_t length);
/**
 * Write Data on UART in ISR
 * \param uart UART Instance 
 * \param data Data
 * \param length Length of Data
 * \return -1 on error >= 0 length write on UART
 */
int32_t uart_writeISR(struct uart *uart, uint8_t *data, size_t length);
/**
 * Write String on UART and a trailing newline in ISR
 * \param uart UART Instance 
 * \param s String
 * \return -1 on error >= 0 length write on UART
 */
int32_t uart_putsISR(struct uart *uart, char *s);
#else
/* #ifdef CONFIG_UART_MULTI */
inline struct uart *uart_init(uint8_t port, uint32_t baudrate) {
	HAL_DEFINE_GLOBAL_ARRAY(uart);
	struct uart_generic *uart = (struct uart_generic *) HAL_GET_DEV(uart, port);
	if (uart == NULL) {
		return NULL;
	}
	return uart->ops->uart_init(port, baudrate);
}
inline int32_t uart_deinit(struct uart *uart) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_deinit(uart);
}
inline char uart_getc(struct uart *uart, TickType_t waittime) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_getc(uart, waittime);
}
inline int32_t uart_putc(struct uart *uart, char c, TickType_t waittime) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_putc(uart, c, waittime);
}
# ifndef CONFIG_UART_GENERIC_BYTE
inline int32_t uart_read(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_read(uart, data, length, waittime);
}
inline int32_t uart_write(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_write(uart, data, length, waittime);
}
# else 
int32_t uart_read(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime);
int32_t uart_write(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime);
# endif
# ifndef CONFIG_UART_GENERIC_STRING
inline int32_t uart_puts(struct uart *uart, char *s, TickType_t waittime) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_puts(uart, s, waittime);
}
# else
int32_t uart_puts(struct uart *uart, char *s, TickType_t waittime);
# endif
inline char uart_getcISR(struct uart *uart) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_getcISR(uart);
}
inline int32_t uart_putcISR(struct uart *uart, char c) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_putcISR(uart, c);
}
# ifndef CONFIG_UART_GENERIC_BYTE
inline int32_t uart_readISR(struct uart *uart, uint8_t *data, size_t length) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_readISR(uart, data, length);
}
inline int32_t uart_writeISR(struct uart *uart, uint8_t *data, size_t length) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_writeISR(uart, data, length);
}
# endif
# ifndef CONFIG_UART_GENERIC_STRING
inline int32_t uart_putsISR(struct uart *uart, char *s) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_putsISR(uart, s);
}
# endif
#endif
/**\}*/
#endif
