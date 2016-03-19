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
