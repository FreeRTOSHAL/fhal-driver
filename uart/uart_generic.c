#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
#include <uart.h>
#define UART_PRV
#include <uart_prv.h>

#include <semphr.h>
extern uint32_t _dev_uart;
struct uart **uarts = (struct uart **) &_dev_uart;
int32_t uart_generic_init(struct uart *u) {
	struct uart_generic *uart = (struct uart_generic *) u;
	if (hal_isInit(uart)) {
		return -UART_ALREDY_INITED;
	}
	uart->init = true;
#ifdef CONFIG_UART_THREAD_SAVE
	{
		int32_t ret = hal_init(uart);
		if (ret < 0) {
			return -1;
		}
	}
#endif
	return 0;
}
#ifdef CONFIG_UART_GENERIC_STRING
int32_t uart_puts(struct uart *uart, char *s, TickType_t waittime) {
	char c;
	int32_t ret;
	uart_lock(uart, waittime, -1);
	do {
		c = *(s++);
		if (c != '\0') {
			if (c == '\n') {
				ret = uart_putc(uart, '\r', waittime);
				if (ret < 0) {
					goto uart_puts_0;
				}
			}
			ret = uart_putc(uart, c, waittime);
			if (ret < 0) {
				goto uart_puts_0;
			}
		}
	} while (c != '\0');
	uart_unlock(uart, -1);
	return 0;
uart_puts_0:
	uart_unlock(uart, ret);
	return ret;
}
int32_t uart_putsISR(struct uart *uart, char *s) {
	char c;
	int32_t ret;
	do {
		c = *(s++);
		if (c != '\0') {
			if (c == '\n') {
				ret = uart_putcISR(uart, '\r');
				if (ret < 0) {
					goto uart_putsISR_0;
				}
			}
			ret = uart_putcISR(uart, c);
			if (ret < 0) {
				goto uart_putsISR_0;
			}
		}
	} while (c != '\0');
	return 0;
uart_putsISR_0:
	uart_unlock(uart, ret);
	return ret;
}
#endif
#ifdef CONFIG_UART_GENERIC_BYTE
int32_t uart_read(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime) {
	size_t i = 0;
	uart_lock(uart, waittime, -1);
	while (i < length) {
		*data = uart_getc(uart, waittime);
		i++;
		data++;
	}
	uart_unlock(uart, -1);
	return i;
}
int32_t uart_write(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime) {
	size_t i = 0;
	int32_t retd;
	uart_lock(uart, waittime, -1);
	for (i = 0; i < length; i++, data++) {
		retd = uart_putc(uart, *data, waittime);
		if (retd < 0 ) {
			goto uart_write_0;
		}
	}
	uart_unlock(uart, -1);
	return i;
uart_write_0:
	uart_unlock(uart, i);
	return i;
}
int32_t uart_readISR(struct uart *uart, uint8_t *data, size_t length) {
	size_t i = 0;
	while (i < length) {
		*data = uart_getcISR(uart);
		i++;
		data++;
	}
	return i;
}
int32_t uart_writeISR(struct uart *uart, uint8_t *data, size_t length) {
	size_t i = 0;
	int32_t retd;
	for (i = 0; i < length; i++, data++) {
		retd = uart_putcISR(uart, *data);
		if (retd < 0 ) {
			return i;
		}
	}
	return i;
}
#endif
#ifdef CONFIG_UART_MULTI
struct uart *uart_init(uint8_t port, uint32_t bautrate);
int32_t uart_deinit(struct uart *uart);
char uart_getc(struct uart *uart, TickType_t waittime);
int32_t uart_putc(struct uart *uart, char c, TickType_t waittime);
# ifndef CONFIG_UART_GENERIC_BYTE
int32_t uart_read(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime);
int32_t uart_write(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime);
# endif
# ifndef CONFIG_UART_GENERIC_STRING
int32_t uart_puts(struct uart *uart, char *s, TickType_t waittime);
# endif
char uart_getcISR(struct uart *uart);
int32_t uart_putcISR(struct uart *uart, char c);
# ifndef CONFIG_UART_GENERIC_BYTE
int32_t uart_readISR(struct uart *uart, uint8_t *data, size_t length);
int32_t uart_writeISR(struct uart *uart, uint8_t *data, size_t length);
# endif
# ifndef CONFIG_UART_GENERIC_STRING
int32_t uart_putsISR(struct uart *uart, char *s);
# endif
#endif
