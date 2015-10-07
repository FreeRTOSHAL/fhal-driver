#ifndef UART_PRV_
#define UART_PRV_
#ifndef UART_PRV
# error "Never include this file out of a UART driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#define UART_ALREDY_INITED 1
struct uart_generic {
	bool init;
#ifdef CONFIG_UART_THREAD_SAVE
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
	return xSemaphoreTakeRecursive(uart_prv->generic.lock, waittime);
}
inline int uart_unlock(struct uart *uart) {
	struct uart_prv *uart_prv = (struct uart_prv *) uart;
	return xSemaphoreGiveRecursive(uart_prv->generic.lock);
}
#else
# define uart_lock(u, w) 1
# define uart_unlock(u) 1
#endif
#endif
