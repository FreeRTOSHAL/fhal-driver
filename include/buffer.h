#ifndef SHARED_H_
#define SHARED_H_
#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#define SHARED_MAGIC 0x42424343

struct buffer_base;
struct buffer;

#define BUFFER_OPERATION_NOT_SUPPORTED -1
#define BUFFER_NO_SPACE_LEFT -2
#define BUFFER_EINVAL -3 

struct buffer *buffer_init(struct buffer_base *base, uint32_t len, uint32_t sizeOfEntry, bool readOnly, uint32_t irqnr);
int32_t buffer_deinit(struct buffer *buffer);
int32_t buffer_is_full(struct buffer *buffer);
int32_t buffer_empty(struct buffer *buffer);
int32_t buffer_write(struct buffer *buffer, uint8_t *data, int32_t size);
int32_t buffer_read(struct buffer *buffer, uint8_t *data, int32_t size, TickType_t waittime);
#endif
