#include <string.h>
#include <stdint.h>

size_t strlen(const char *s) {
	size_t n = 0;
	while(*s != '\0') {
		s++;
		n++;
	}
	return n;
}
char *strcpy(char *dest, const char *src) {
	char *olddest = dest;
	while(*src != '\0') {
		*(dest++) = *(src++);
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

