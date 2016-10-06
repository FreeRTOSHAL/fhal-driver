#ifndef REMOTEPROC_TRACE_H_
#define REMOTEPROC_TRACE_H_
#include <uart.h>
struct uart {
	struct uart_generic gen;
	uint8_t *base;
	uint32_t len;
	uint8_t *pos;
};
/**\cond INTERNAL*/
#define UART_PRV
#include <uart_prv.h>
extern const struct uart_ops remoteproc_uart_ops;
/**\endcond*/
#define REMOTEPROC_TRACE_ADDDEV(id, buffer, l) \
	struct uart remoteproc_trace_##id = { \
		UART_INIT_DEV(remoteproc) \
		HAL_NAME("Remoteproc Trace " #id) \
		.base = buffer, \
		.len = l, \
		.pos = buffer, \
	}; \
	UART_ADDDEV(remoteproc, remoteproc_trace_##id)
HAL_DEFINE_GLOBAL_ARRAY(uart);
#define REMOTEPROC_TRACE_ID(id) HAL_GET_ID(uart, remoteproc, remoteproc_trace_##id)
#endif
