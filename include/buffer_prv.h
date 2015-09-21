#ifndef BUFFER_PRV_
#define BUFFER_PRV_
#ifndef BUFFER_PRV
#error "Never include this file out of a Buffer driver"
#endif
#include <stdbool.h>
struct buffer_prv;

struct buffer {
	struct buffer_prv *prv;
	struct buffer_base *base;
	bool readOnly;
	uint32_t irqnr;
	uint8_t *buffer;
};

struct buffer_base {
	uint32_t magicNr;
	uint32_t len;
	uint32_t sizeOfEntry;
	uint32_t size;

	uint32_t readP;
	uint32_t writeP;
};

int32_t buffer_init_prv(struct buffer *buffer);
int32_t buffer_wfi(struct buffer *buffer, TickType_t waittime);
void buffer_notify(struct buffer *buffer);

#endif
