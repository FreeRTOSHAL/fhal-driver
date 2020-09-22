/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <stdint.h>
#include <uart.h>

struct uart *out = NULL;
struct uart *in = NULL;

int32_t nlibc_init(struct uart *std_in, struct uart *std_out) {
	in = std_in;
	out = std_out;
	return 0;
}

int puts(const char *s) {
	return uart_puts(out, (char *) s, portMAX_DELAY);
}

int putchar(int c) {
	return uart_putc(out, c, portMAX_DELAY);
}
