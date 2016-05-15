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
#include <stdio.h>
#include <system.h>
#include <hal.h>
#include <example.h>
#define EXAMPLE_PRV
#include <example_prv.h>

struct example {
	struct example_generic gen;
	uint32_t index;
};

EXAMPLE_INIT(exDev1, index) {
	int32_t ret;
	struct example *ex = EXAMPLE_GET_DEV(index);
	if (ex == NULL) {
		return NULL;
	}
	ret = example_genericInit(ex);
	if (ret < 0) {
		return NULL;
	}
	if (ret == EXAMPLE_ALREDY_INITED) {
		return ex;
	}
	ex->index = index;
	return ex;
}
EXAMPLE_DEINIT(exDev1, ex) {
	ex->gen.init = false;
	return 0;
}
EXAMPLE_FUNCNAME(exDev1, ex, param) {
	printf("%s: index: %lu param: %lu", __FUNCTION__, ex->index, param);
	return 0;
}

EXAMPLE_OPS(exDev1);
static struct example ex0 = {
	EXAMPLE_INIT_DEV(exDev1)
	HAL_NAME("Example 1 Driver 0")
};
EXAMPLE_ADDDEV(exDev1, ex0);
static struct example ex1 = {
	EXAMPLE_INIT_DEV(exDev1)
	HAL_NAME("Example 1 Driver 0")
};
EXAMPLE_ADDDEV(exDev1, ex1);
