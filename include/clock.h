#ifndef CLOCK_H_
#define CLOCK_H_
#include <stdint.h>
struct clock *clock_init();
int32_t clock_deinit(struct clock *clk);
#endif
