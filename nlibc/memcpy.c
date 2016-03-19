/*
 * Copyright (c) 2016 Andreas Werner <kernel@andy89.org>
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 *  files (the "Software"), to deal in the Software without restriction, 
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
#include <string.h>
#include <stdint.h>

void *memcpy(void *d, const void *s, size_t n) {
	/* Speed Up Copy if d and s is aliged */
	/*
	 * If n,s and d power of 4
	 * copy with 32 Bit
	 */
	if (((n & 0x3) == 0) && ((((uintptr_t) d) & 0x3) == 0) && ((((uintptr_t) s) & 0x3) == 0)) {
		const uint32_t *src = s;
		uint32_t *dest = d;
		n >>= 2;
		while(n > 0) {
			*(dest++) = *(src++);
			n--;
		}
	/*
	 * If n,s and d power of 2
	 * copy with 16 Bit
	 */
	} else if (((n & 0x1) == 0) && ((((uintptr_t) d) & 0x1) == 0) && ((((uintptr_t) s) & 0x1) == 0)) {
		const uint16_t *src = s;
		uint16_t *dest = d;
		n >>= 1;
		while(n > 0) {
			*(dest++) = *(src++);
			n--;
		}
	/*
	 * Else 
	 * copy with 8 Bit
	 */
	} else {
		const uint8_t *src = s;
		uint8_t *dest = d;
		while(n > 0) {
			*(dest++) = *(src++);
			n--;
		}

	}
	return d;
}
