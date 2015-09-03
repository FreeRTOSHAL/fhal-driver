#include <FreeRTOS.h>
#include <stdint.h>
#include <uart.h>
#define UART_PRV
#include <uart_prv.h>

#include <semphr.h>
int32_t uart_generic_init(struct uart *uart) {
#ifdef CONFIG_UART_THREAD_SAVE
	struct uart_prv *uart_prv = (struct uart_prv *) uart;
	{
		uart_prv->generic.lock = xSemaphoreCreateRecursiveMutex();
		if (uart_prv->generic.lock == NULL) {
			return -1;
		}
	}
#endif
	return 0;
}
int uart_lock(struct uart *uart, TickType_t waittime);
int uart_unlock(struct uart *uart);
#ifdef CONFIG_UART_GENERIC_STRING
int32_t uart_puts(struct uart *uart, char *s, TickType_t waittime) {
	char c;
	int32_t ret;
	ret = uart_lock(uart, waittime);
	if (ret != 1) {
		ret = -1;
		goto uart_writeString_0;
	}
	do {
		c = *(s++);
		if (c != '\0') {
			if (c == '\n') {
				ret = uart_putc(uart, '\r', waittime);
				if (ret < 0) {
					goto uart_writeString_1;
				}
			}
			ret = uart_putc(uart, c, waittime);
			if (ret < 0) {
				goto uart_writeString_1;
			}
		}
	} while (c != '\0');
	ret = uart_unlock(uart);
	if (ret != 1) {
		ret = -1;
		goto uart_writeString_0;
	}
	return 0;
uart_writeString_1:
	uart_unlock(uart);
uart_writeString_0:
	return ret;
}
int32_t uart_gets(struct uart *uart, char *s, TickType_t waittime) {
	/* TODO */
	return -1;
}
#endif
#ifdef CONFIG_UART_GENERIC_BYTE
int32_t uart_read(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime) {
	int ret;
	size_t i = 0;
	ret = uart_lock(uart, waittime);
	if (ret != 1) {
		return -1;
	}
	while (i < length) {
		*data = uart_getc(uart, waittime);
		i++;
		data++;
	}
	if (ret != 1) {
		return -1;
	}
	return i;
}
int32_t uart_write(struct uart *uart, uint8_t *data, size_t length, TickType_t waittime) {
	size_t i = 0;
	int ret;
	int32_t retd;
	ret = uart_lock(uart, waittime);
	if (ret != 1) {
		return -1;
	}
	while (i < length) {
		retd = uart_putc(uart, *(data++), waittime);
		if (retd < 0 ) {
			goto uart_writeBytes_0;
		}
	}
	ret = uart_unlock(uart);
	if (ret != 1) {
		return -1;
	}
uart_writeBytes_0:
	uart_unlock(uart);
	return i;
}
#endif
