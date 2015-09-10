#ifndef NEWLIB_STUB_H_
#define NEWLIB_STUB_H_
#include <uart.h>
#include <stdint.h>
int32_t newlib_init(struct uart *std_in, struct uart *std_out);
#endif
