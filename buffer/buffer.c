#include <FreeRTOS.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "buffer.h"
#include "cache.h"
#define BUFFER_PRV
#include <buffer_prv.h>

static uint32_t getWriteP(struct buffer *buffer) {
#ifdef CONFIG_CACHE
	if (buffer->readOnly) {
		/*
		 * May Change by A5 invalid Cache
		 */
		cache_invalidData(&buffer->base->writeP, 1);
	}
#endif
	return buffer->base->writeP;
}

static uint32_t getReadP(struct buffer *buffer) {
#ifdef CONFIG_CACHE
	if (!buffer->readOnly) {
		/*
		 * May Change by A5 invalid Cache
		 */
		cache_invalidData(&buffer->base->readP, 1);
	}
#endif
	return buffer->base->readP;
}

static void incrementWriteP(struct buffer *buffer) {
	struct buffer_base *base = buffer->base;
	/*base->writeP= (getWriteP(buffer) + base->sizeOfEntry) % base->size;*/
	base->writeP = (getWriteP(buffer) + base->sizeOfEntry) & (base->size - 1);
	/* 
	 * Write Back To SRAM
	 */
#ifdef CONFIG_CACHE
	cache_flushData(&base->writeP, 1);
#endif
}
static void incrementReadP(struct buffer *buffer) {
	struct buffer_base *base = buffer->base;
	/*base->readP= (getReadP(buffer) + base->sizeOfEntry) % base->size;*/
	base->readP = (getReadP(buffer) + base->sizeOfEntry) & (base->size - 1);
	/* 
	 * Write Back To SRAM
	 */
#ifdef CONFIG_CACHE
	cache_flushData(&base->readP, 1);
#endif
}

static bool isPowerOf2(uint32_t n) {
	/* 
	 * Check size is Power of Basis 2
	 * (1, 2, 4, 8, 16, 32, ...)
	 */
	return (n != 0 && ((n & (n - 1)) == 0));
}

struct buffer *buffer_init(struct buffer_base *base, uint32_t len, uint32_t sizeOfEntry, bool readOnly, uint32_t irqnr) {
	int32_t ret;
	struct buffer *buffer = pvPortMalloc(sizeof(struct buffer));
	if (buffer == NULL) {
		goto buffer_init_error_0;
	}
	buffer->base = base;
	buffer->readOnly = readOnly;
	buffer->irqnr = irqnr;
	buffer->buffer = ((uint8_t *) base) + sizeof(struct buffer_base);
	/*
	 * Init Shared Memmory
	 */
	if (base->magicNr != 0x42424343) {
		base->len = len;
		base->sizeOfEntry = sizeOfEntry;
		base->size = base->len * base->sizeOfEntry;
		{
			bool check = isPowerOf2(base->size);
			if (!check) {
				goto buffer_init_error_1;
			}
		}

		base->readP = 0;
		base->writeP = 0;
		base->magicNr = 0x42424343;
#ifdef CONFIG_CACHE
		cache_flushData((uint32_t *) base, sizeof(struct buffer) / 4);
#endif
	} else {
		if (
			base->len != len ||
			base->sizeOfEntry != sizeOfEntry
		) {
			goto buffer_init_error_1;
		}
	}
	ret = buffer_init_prv(buffer);
	if (ret < 0) {
		goto buffer_init_error_1;
	}
	return buffer;
buffer_init_error_1:
	vPortFree(buffer);
buffer_init_error_0: 
	return NULL;
}

/* 
 * ---------------------------------------------------------
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | 
 * ---------------------------------------------------------
 *      free Space <--- ^ filed Space  ^ ---> free Space  ^ First Entry
 *                    readP          writeP
 * ---------------------------------------------------------
 * |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | 
 * ---------------------------------------------------------
 *     filed Space <--- ^ free Space   ^ ---> filed Space ^ First Entry
 *                    writeP         readP
 */

int32_t buffer_getFreeSpace(struct buffer *buffer) {
	int32_t writeP = getWriteP(buffer);
	int32_t readP = getReadP(buffer);
	if (writeP >= readP) {
		/* 
		 * For Overflow Dedection one Slot need to be free
		 */
		return (buffer->base->size - buffer->base->sizeOfEntry) - (writeP - readP);
	} else {
		return (readP - writeP) - buffer->base->sizeOfEntry;
	}
}
int32_t buffer_deinit(struct buffer *buffer) {
	return 0;
}
int32_t buffer_is_full(struct buffer *buffer) {
	struct buffer_base *base = buffer->base;
	return (getWriteP(buffer) + base->sizeOfEntry) % (base->len * base->sizeOfEntry) == getReadP(buffer);
}
bool buffer_empty(struct buffer *buffer) {
	return getWriteP(buffer) == getReadP(buffer);
}

int32_t buffer_write(struct buffer *buffer, uint8_t *data, int32_t size) {
	struct buffer_base *base = buffer->base;
	uint32_t tmp;
	int32_t wsize;
	uint32_t writeP;
	uint32_t writeSize = 0;
	if (size == 0) {
		return BUFFER_EINVAL;
	}
	/* 
	 * Scale size to bytes
	 */
	size *= buffer->base->sizeOfEntry;
	/* 
	 * Error if ReadOnly
	 */
	if (buffer->readOnly) {
		return BUFFER_OPERATION_NOT_SUPPORTED;
	}
	do {
		tmp = buffer_getFreeSpace(buffer);
		if (tmp == 0) {
			/* mh no space left in buffer error;) */
			return BUFFER_NO_SPACE_LEFT;
		} else {
			/* 
			 * TODO Optimization. Copy more then one entry
			 */
			wsize = base->sizeOfEntry;
		}
		writeP = getWriteP(buffer);
		memcpy(buffer->buffer + writeP, data, wsize);
#ifdef CONFIG_CACHE
		/* 
		 * Write Entry to SRAM
		 */
		cache_flushData((uint32_t *) (buffer->buffer + writeP), wsize);
#endif
		data += wsize;
		size -= wsize;
		writeSize += wsize;
		/* 
		 * All data write in Buffer increment WriteP
		 */
		incrementWriteP(buffer);
	} while(size > 0);
	buffer_notify(buffer);
	return writeSize / base->sizeOfEntry;
}
int32_t buffer_read(struct buffer *buffer, uint8_t *data, int32_t size, TickType_t waittime) {
	struct buffer_base *base = buffer->base;
	uint32_t readSize = 0;
	uint32_t readP;
	size *= base->sizeOfEntry;
	/* 
	 * If Not ReadOnly is WriteOnly
	 */
	if (!buffer->readOnly) {
		return BUFFER_OPERATION_NOT_SUPPORTED;
	}
	if (buffer_empty(buffer)) {
		buffer_wfi(buffer, waittime);
	}
	while(!buffer_empty(buffer) && readSize < size) {
		readP = getReadP(buffer);
#ifdef CONFIG_CACHE
		/* 
		 * Read Entry to Cache
		 */
		cache_invalidData((uint32_t *) (buffer->buffer + readP), base->sizeOfEntry);
#endif
		memcpy(data, buffer->buffer + readP, base->sizeOfEntry);
		data+=base->sizeOfEntry;
		readSize+=base->sizeOfEntry;
		incrementReadP(buffer);
	}
	return readSize / base->sizeOfEntry;
}
