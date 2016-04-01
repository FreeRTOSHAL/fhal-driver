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

size_t strlen(const char *s) {
	size_t n;
	for (n = 0; *s != '\0';s++,n++);
	return n;
}
char *strcpy(char *dest, const char *src) {
	char *olddest = dest;
	for (;*src != '\0';dest++, src++) {
		*dest = *src;
	}
	/* copy  '\0' */
	*dest = *(src);
	return olddest;
}
char *strncpy(char *dest, const char *src, size_t n) {
	size_t i;
	for (i = 0; i < n && src[i] != '\0'; i++) {
		dest[i] = src[i];
	}
	for (; i < n; i++) {
		dest[i] = '\0';
	}
	return dest;
}
char *stpcpy(char *dest, const char *src) {
	while(*src != '\0') {
		*(dest++) = *(src++);
	}
	/* copy  '\0' */
	*dest = *(src);
	return dest;
}

