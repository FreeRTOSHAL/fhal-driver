#include <stdint.h>
#include <sys/types.h>
#include "FreeRTOS.h"

/**
 * sbrk - change data segment size
 * 
 * Increase program data space. This function only check Heap is growing to high. The freeing Algorithm is located at malloc and free
 * Implementation of Newlib. Newlib use Malloc Algorithm of Doug Lea (http://g.oswego.edu/dl/html/malloc.html). 
 * 
 * \param incr Size of incrementation is always aligned
 * \return new Pointer address
 */
caddr_t _sbrk(int incr) {
	return pvPortMalloc(incr);
}
