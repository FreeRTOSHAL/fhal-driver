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

EXAMPLE_INIT(exDev0, index) {
	int32_t ret;
	struct example *ex = EXAMPLE_GET_DEV(index);
	if  (ex == NULL) {
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
EXAMPLE_DEINIT(exDev0, ex) {
	ex->gen.init = false;
	return 0;
}
EXAMPLE_FUNCNAME(exDev0, ex, param) {
	printf("%s: index: %lu param: %lu", __FUNCTION__, ex->index, param);
	return 0;
}

EXAMPLE_OPS(exDev0);
static struct example ex0 = {
	EXAMPLE_INIT_DEV(exDev0)
};
EXAMPLE_ADDDEV(exDev0, ex0);
static struct example ex1 = {
	EXAMPLE_INIT_DEV(exDev0)
};
EXAMPLE_ADDDEV(exDev0, ex1);
