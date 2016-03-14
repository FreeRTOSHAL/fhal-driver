#include <stdlib.h>
#include <stdint.h>
void *memset(void *s, int c, size_t n) {
	int8_t *dest = s;
	while (n > 0) {
		*(dest++) = (int8_t) c;
		n--;
	}
	return s;
}
