/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef UART_PRV_
#define UART_PRV_
#ifndef UART_PRV
# error "Never include this file out of a UART driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#define UART_ALREDY_INITED 1
int32_t uart_generic_init(struct uart *uart);
#ifdef CONFIG_UART_THREAD_SAFE
# define uart_lock(u, w, e) HAL_LOCK(u, w, e)
# define uart_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define uart_lock(u, w, e)
# define uart_unlock(u, e)
#endif

#define UART_ADDDEV(ns, p) HAL_ADDDEV(uart, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(uart);
#define UART_GET_DEV(index) HAL_GET_DEV(uart, index)
#ifndef CONFIG_UART_MULTI
# define UART_OPS(ns)
# define UART_INIT_DEV(ns) 
# define UART_INIT(ns, port, baudrate) struct uart *uart_init(uint8_t port, uint32_t baudrate) 
# define UART_DEINIT(ns, u) int32_t uart_deinit(struct uart *u)
# define UART_GETC(ns, u, waittime) char uart_getc(struct uart *u, TickType_t waittime) 
# define UART_PUTC(ns, u, c, waittime) int32_t uart_putc(struct uart *u, char c, TickType_t waittime) 
# define UART_READ(ns, u, data, length, waittime) int32_t uart_read(struct uart *u, uint8_t *data, size_t length, TickType_t waittime) 
# define UART_WRITE(ns, u, data, length, waittime) int32_t uart_write(struct uart *u, uint8_t *data, size_t length, TickType_t waittime) 
# define UART_PUTS(ns, u, s, waitime) int32_t uart_puts(struct uart *u, char *s, TickType_t waittime)
# define UART_GETC_ISR(ns, u) char uart_getcISR(struct uart *u)
# define UART_PUTC_ISR(ns, u, c) int32_t uart_putcISR(struct uart *u, char c)
# define UART_READ_ISR(ns, u, data, length) int32_t uart_readISR(struct uart *u, uint8_t *data, size_t length)
# define UART_WRITE_ISR(ns, u, data, length) int32_t uart_writeISR(struct uart *u, uint8_t *data, size_t length)
# define UART_PUTS_ISR(ns, u, s) int32_t uart_putsISR(struct uart *u, char *s)
#else

# ifndef CONFIG_UART_GENERIC_BYTE
#  define UART_READ_WRITE_OPS(ns) .uart_read = &ns##_uart_read, \
	.uart_write = &ns##_uart_write, \
	.uart_readISR = &ns##_uart_readISR, \
	.uart_writeISR = &ns##_uart_writeISR,
# else
#  define UART_READ_WRITE_OPS(ns)
# endif

# ifndef CONFIG_UART_GENERIC_STRING
#  define UART_STRING_OPS(ns) .uart_puts = &ns##_uart_puts, \
	.uart_putsISR = &ns##_uart_putsISR,
# else
#  define UART_STRING_OPS(ns)
#endif


# define UART_OPS(ns) const struct uart_ops ns##_uart_ops = { \
	.uart_init = &ns##_uart_init, \
	.uart_deinit = &ns##_uart_deinit, \
	.uart_getc = &ns##_uart_getc, \
	.uart_putc = &ns##_uart_putc, \
	.uart_getcISR = &ns##_uart_getcISR, \
	.uart_putcISR = &ns##_uart_putcISR, \
	UART_READ_WRITE_OPS(ns) \
	UART_STRING_OPS(ns) \
}

# define UART_INIT_DEV(ns) .gen.ops = &ns##_uart_ops,

# define UART_INIT(ns, port, baudrate) static struct uart *ns##_uart_init(uint8_t port, uint32_t baudrate) 
# define UART_DEINIT(ns, u) static int32_t ns##_uart_deinit(struct uart *u)
# define UART_GETC(ns, u, waittime) static char ns##_uart_getc(struct uart *u, TickType_t waittime) 
# define UART_PUTC(ns, u, c, waittime) static int32_t ns##_uart_putc(struct uart *u, char c, TickType_t waittime) 
# define UART_READ(ns, u, data, length, waittime) static int32_t ns##_uart_read(struct uart *u, uint8_t *data, size_t length, TickType_t waittime) 
# define UART_WRITE(ns, u, data, length, waittime) static int32_t ns##_uart_write(struct uart *u, uint8_t *data, size_t length, TickType_t waittime) 
# define UART_PUTS(ns, u, s, waitime) static int32_t ns##_uart_puts(struct uart *u, char *s, TickType_t waittime)
# define UART_GETC_ISR(ns, u) static char ns##_uart_getcISR(struct uart *u)
# define UART_PUTC_ISR(ns, u, c) static int32_t ns##_uart_putcISR(struct uart *u, char c)
# define UART_READ_ISR(ns, u, data, length) static int32_t ns##_uart_readISR(struct uart *u, uint8_t *data, size_t length)
# define UART_WRITE_ISR(ns, u, data, length) static int32_t ns##_uart_writeISR(struct uart *u, uint8_t *data, size_t length)
# define UART_PUTS_ISR(ns, u, s) static int32_t ns##_uart_putsISR(struct uart *u, char *s)
#endif
#endif
