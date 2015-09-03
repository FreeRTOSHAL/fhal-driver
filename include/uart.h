#ifndef UART_H_
#define UART_H_
#include <FreeRTOS.h>
#include <stdint.h>
struct uart;
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
int32_t uart_gets(struct uart *uart, char *s, TickType_t waittime);
#endif
