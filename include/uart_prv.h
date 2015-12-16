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
#ifdef CONFIG_UART_THREAD_SAVE
# define uart_lock(u, w, e) HAL_LOCK(u, w, e)
# define uart_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define uart_lock(u, w, e)
# define uart_unlock(u, e)
#endif

#define UART_ADDDEV(ns, p) static struct uart_generic SECTION(".rodata.dev.uart") USED const * ns##_##p = (struct uart_generic const *) &p
#ifndef CONFIG_UART_MULTI
# define UART_OPS(ns)
# define UART_INIT(ns, port, bautrate) struct uart *uart_init(uint8_t port, uint32_t bautrate) 
# define UART_DEINIT(ns, uart) int32_t uart_deinit(struct uart *uart)
# define UART_GETC(ns, uart, waittime) char uart_getc(struct uart *uart, TickType_t waittime) 
# define UART_PUTC(ns, uart, c, waittime) int32_t uart_putc(struct uart *uart, char c, TickType_t waittime) 
# define UART_READ(ns, uart, data, length, waittime) int32_t uart_read(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime) 
# define UART_WRITE(ns, uart, data, length, waittime) int32_t uart_write(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime) 
# define UART_PUTS(ns, uart, s, waitime) int32_t uart_puts(struct uart *uart, char *s, TickType_t waittime)
# define UART_GETC_ISR(ns, uart) char uart_getcISR(struct uart *uart)
# define UART_PUTC_ISR(ns, uart, c) int32_t uart_putcISR(struct uart *uart, char c)
# define UART_READ_ISR(ns, uart, data, length) int32_t uart_readISR(struct uart *uart, uint8_t *data, size_t length)
# define UART_WRITE_ISR(ns, uart, data, length) int32_t uart_writeISR(struct uart *uart, uint8_t *data, size_t length)
# define UART_PUTS_ISR(ns, uart, s) int32_t uart_putsISR(struct uart *uart, char *s)
#else

# ifndef CONFIG_UART_GENERIC_BYTE
#  define UART_READ_WRITE_OPS(ns) .uart_read = &ns##_uart_read, \
	.uart_write = &ns##_uart_write, \
# else
#  define UART_READ_WRITE_OPS(ns)
# endif

# ifndef CONFIG_UART_GENERIC_STRING
#  define UART_STRING_OPS(ns) .uart_puts = &ns##_uart_puts,
# else
#  define UART_STRING_OPS(ns)
#endif

# ifdef CONFIG_UART_THREAD_SAVE
#  ifndef CONFIG_UART_GENERIC_BYTE
#   define UART_ISR_READ_WRITE_OPS(ns) .uart_readISR = &ns##_uart_readISR, \
	.uart_writeISR = &ns##_uart_writeISR,
#  else
#   define UART_ISR_READ_WRITE_OPS(ns)
#  endif
#  ifndef CONFIG_UART_GENERIC_STRING
#   define UART_ISR_STRING_OPS(ns) .uart_putsISR = &ns##_uart_putsISR,
#  else
#   define UART_ISR_STRING_OPS(ns)
#  endif
#  define UART_ISR_OPS(ns) .uart_getcISR = &ns##_uart_getcISR, \
	.uart_putcISR = &ns##_uart_putcISR, \
	UART_ISR_READ_WRITE_OPS(ns) \
	UART_ISR_STRING_OPS(ns)
# else
#  define UART_ISR_OPS(ns)
# endif

# define UART_OPS(ns) static struct uart_ops ops = { \
	.uart_init = &ns##_uart_init, \
	.uart_deinit = &ns##_uart_deinit, \
	.uart_getc = &ns##_uart_getc, \
	.uart_putc = &ns##_uart_putc, \
	/*UART_READ_WRITE_OPS(ns) \*/ /* TODO */ \
	UART_STRING_OPS(ns) \
	UART_ISR_OPS(ns) \
}
# define UART_INIT(ns, port, bautrate) static struct uart *ns##_uart_init(uint8_t port, uint32_t bautrate) 
# define UART_DEINIT(ns, uart) static int32_t ns##_uart_deinit(struct uart *uart)
# define UART_GETC(ns, uart, waittime) static char ns##_uart_getc(struct uart *uart, TickType_t waittime) 
# define UART_PUTC(ns, uart, c, waittime) static int32_t ns##_uart_putc(struct uart *uart, char c, TickType_t waittime) 
# define UART_READ(ns, uart, data, length, waittime) static int32_t ns##_uart_read(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime) 
# define UART_WRITE(ns, uart, data, length, waittime) static int32_t ns##_uart_write(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime) 
# define UART_PUTS(ns, uart, s, waitime) static int32_t ns##_uart_puts(struct uart *uart, char *s, TickType_t waittime)
# define UART_GETC_ISR(ns, uart) static char ns##_uart_getcISR(struct uart *uart)
# define UART_PUTC_ISR(ns, uart, c) static int32_t ns##_uart_putcISR(struct uart *uart, char c)
# define UART_READ_ISR(ns, uart, data, length) static int32_t ns##_uart_readISR(struct uart *uart, uint8_t *data, size_t length)
# define UART_WRITE_ISR(ns, uart, data, length) static int32_t ns##_uart_writeISR(struct uart *uart, uint8_t *data, size_t length)
# define UART_PUTS_ISR(ns, uart, s) static int32_t ns##_uart_putsISR(struct uart *uart, char *s)
#endif
#endif
