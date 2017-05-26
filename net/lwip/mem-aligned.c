#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/err.h"
#include <stdlib.h>
#include <malloc.h>
void mem_init(void) {
}

void *mem_trim(void *mem, mem_size_t size) {
	(void) size;
	return mem;
}

void *mem_malloc(mem_size_t size) {
	return memalign(MEM_ALIGNMENT, size);
}
void mem_free(void *mem) {
	free(mem);
}

