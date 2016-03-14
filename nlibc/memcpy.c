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
