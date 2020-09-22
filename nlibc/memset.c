/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#include <stdlib.h>
#include <stdint.h>
void *memset(void *s, int c, size_t n) {
	volatile int8_t *dest = s;
	for (dest = s; n > 0; dest++, n--) {
		*dest = (int8_t) c;
	}
	return s;
}
