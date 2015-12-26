#ifndef MUX_H_
#define MUX_H_
#include <stdint.h>
#include <system.h>
/* 
 * Private Data of mux subsystem
 */
struct mux;
struct mux *mux_init();
int32_t mux_deinit(struct mux *mux);

#define MUX_CTL_OPEN BIT(0)
#define MUX_CTL_PULL_DOWN BIT(1)
#define MUX_CTL_PULL_UP BIT (2)
#define MUX_CTL_SCHMITT BIT(3)
#define MUX_CTL_MODE(x) (x << 8)

int32_t mux_pinctl(struct mux *mux, uint32_t pin, uint32_t ctl, uint32_t extra);
#endif
