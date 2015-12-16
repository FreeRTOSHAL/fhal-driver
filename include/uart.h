#ifndef UART_H_
#define UART_H_
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
struct uart;
#ifdef CONFIG_UART_MULTI
struct uart_ops {
	struct uart *(*uart_init)(uint8_t port, uint32_t bautrate);
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

# ifdef UART_THREAD_SAVE
	char (*uart_getcISR)(struct uart *uart);
	int32_t (*uart_putcISR)(struct uart *uart, char c);
#  ifdef CONFIG_UART_GENERIC_BYTE
	int32_t (*uart_readISR)(struct uart *uart, uint8_t *data, size_t length);
	int32_t (*uart_writeISR)(struct uart *uart, uint8_t *data, size_t length);
#  endif
#  ifndef CONFIG_UART_GENERIC_STRING
	int32_t (*uart_putsISR)(struct uart *uart, char *s);
#  endif
# endif
};
#endif

struct uart_generic {
	bool init;
#ifdef CONFIG_UART_THREAD_SAVE
	SemaphoreHandle_t lock;	
#endif
#ifdef CONFIG_UART_MULTI
	struct uart_ops *ops;
#endif
};
extern struct uart **uarts;
#ifndef CONFIG_UART_MULTI
struct uart *uart_init(uint8_t port, uint32_t bautrate);
int32_t uart_deinit(struct uart *uart);
char uart_getc(struct uart *uart, TickType_t waittime);
int32_t uart_putc(struct uart *uart, char c, TickType_t waittime);
int32_t uart_read(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime);
int32_t uart_write(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime);
/*
 * Platorm independet if CONFIG_UART_GENERIC_STRING
 */
int32_t uart_puts(struct uart *uart, char *s, TickType_t waittime);

# ifdef UART_THREAD_SAVE
char uart_getcISR(struct uart *uart);
int32_t uart_putcISR(struct uart *uart, char c);
int32_t uart_readISR(struct uart *uart, uint8_t *data, size_t length);
int32_t uart_writeISR(struct uart *uart, uint8_t *data, size_t length);
int32_t uart_putsISR(struct uart *uart, char *s);
# endif
#else
/* #ifdef CONFIG_UART_MULTI */
inline struct uart *uart_init(uint8_t port, uint32_t bautrate) {
	struct uart_generic *uart = (struct uart_generic *) uarts[port];
	return uart->ops->uart_init(port, bautrate);
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
	return u->ops->uart_write(uart, data, length);
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
# ifdef UART_THREAD_SAVE
inline char uart_getcISR(struct uart *uart) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_getcISR(uart);
}
inline int32_t uart_putcISR(struct uart *uart, char c) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_putcISR(uart, c);
}
#  ifndef CONFIG_UART_GENERIC_BYTE
inline int32_t uart_readISR(struct uart *uart, uint8_t *data, size_t length) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_readISR(uart, data, length);
}
inline int32_t uart_writeISR(struct uart *uart, uint8_t *data, size_t length) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_writeISR(uart, data, length);
}
#  endif
#  ifndef CONFIG_UART_GENERIC_STRING
inline int32_t uart_putsISR(struct uart *uart, char *s) {
	struct uart_generic *u = (struct uart_generic *) uart;
	return u->ops->uart_putsISR(uart, s);
}
#  endif
# endif
#endif
#ifndef UART_THREAD_SAVE
inline char uart_getcISR(struct uart *uart) {
	return uart_getc(uart, 0);
}
inline int32_t uart_putcISR(struct uart *uart, char c) {
	return uart_putc(uart, c, 0);
}
inline int32_t uart_readISR(struct uart *uart, uint8_t *data, size_t length) {
	return uart_read(uart, data, length, 0);
}
inline int32_t uart_writeISR(struct uart *uart, uint8_t *data, size_t length) {
	return uart_write(uart, data, length, 0);
}
inline int32_t uart_putsISR(struct uart *uart, char *s) {
	return uart_puts(uart, s, 0);
}
#endif
#endif
