#include <uart.h>
#define UART_PRV
#include <uart_prv.h>
#include <remoteproc_trace.h>
UART_INIT(remoteproc, port, bautrate) {
	struct uart *uart = (struct uart *) UART_GET_DEV(port);
	int32_t ret;
	if (uart == NULL) {
		return NULL;
	}
	ret = uart_generic_init(uart);
	if (ret < 0) {
		return NULL;
	}
	/*
	 * Already Init
	 */
	if (ret > 0) {
		return uart;
	}
	/* ignored */
	(void) bautrate;
	if (!uart->base || !uart->len) {
		uart->gen.init = false;
		return NULL;
	}
	/* Terminate trace buffer */
	*uart->base = '\0';
	uart->pos = uart->base;
	return uart;
}
UART_DEINIT(remoteproc, uart) {
	return 0;
}
UART_GETC(remoteproc, uart, waittime) {
	return '\0';
}
UART_PUTC(remoteproc, uart, c, waittime) {
	int32_t ret;
	uart_lock(uart, waittime, -1);
	ret = uart_putcISR(uart, c);
	uart_unlock(uart, -1);
	return ret;
}
UART_GETC_ISR(remoteproc, uart) {
	return '\0';
}
UART_PUTC_ISR(remoteproc, uart, c) {
	uint8_t *newpos;
	newpos = uart->pos;
	if ((newpos + 1) > uart->base + uart->len) {
		/* TODO overflow */
		return -1;
	}
	/* terminate String */
	*(newpos + 1) = '\0';
	/* Set Char */
	*newpos = c;
	uart->pos = newpos + 1;
	return 0;
}
UART_OPS(remoteproc);
