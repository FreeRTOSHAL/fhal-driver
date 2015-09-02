#ifndef UART_PRV_
#define UART_PRV_
#ifndef UART_PRV
# error "Never include this file out of a UART driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
struct uart_generic {
#ifndef UART_THREAD_SAVE
	SemaphoreHandle_t lock;	
#endif
};
struct uart_prv {
	struct uart_generic generic;
};
int32_t uart_generic_init(struct uart *uart);
#ifdef CONFIG_UART_THREAD_SAVE
inline int uart_lock(struct uart *uart, TickType_t waittime) {
	struct uart_prv *uart_prv = (struct uart_prv *) uart;
	return xSemaphoreTake(uart_prv->generic.lock, waittime);
}
inline int uart_unlock(struct uart *uart) {
	struct uart_prv *uart_prv = (struct uart_prv *) uart;
	return xSemaphoreGive(uart_prv->generic.lock);
}
#else
inline int uart_lock(struct uart *uart, TickType_t waittime) {
	(void) uart;
	(void) waittime;
	return 1;
}
inline int uart_unlock(struct uart *uart) {
	(void) uart;
	return 1;
}
#endif
#endif
