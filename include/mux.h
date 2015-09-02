#ifndef MUX_H_
#define MUX_H_
#include <stdint.h>
/* 
 * Private Data of mux subsystem
 */
struct mux;
struct mux *mux_init();
int32_t mux_deinit(struct mux *mux);
int32_t mux_pinctl(struct mux *mux, uint32_t pin, uint32_t ctl);
#endif
