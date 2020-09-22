/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
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

