/*
 * Copyright (c) 2016 Andreas Werner <kernel@andy89.org>
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 */
#ifndef BUFFER_PRV_
#define BUFFER_PRV_
#ifndef BUFFER_PRV
#error "Never include this file out of a Buffer driver"
#endif
#include <stdbool.h>
/**\cond INTERNAL*/
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
/**\endcond*/
#endif
