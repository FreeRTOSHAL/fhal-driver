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
#ifndef MUX_H_
#define MUX_H_
#include <stdint.h>
#include <system.h>
/**
 * \defgroup MUX Mux Subsystem
 * \ingroup HAL
 * 
 * This is Mux Subsytem
 * \{
 */
/**
 * Private Structure of Mux driver
 */
struct mux;
/**
 * Init Mux Subsystem
 * \return Mux Handler 
 */
struct mux *mux_init();
/**
 * Deinit Mux Subsystem
 * \param mux Mux handle
 * \return -1 on error 0 on ok
 */
int32_t mux_deinit(struct mux *mux);

/**
 * Configure pin without pull down or pull down
 */
#define MUX_CTL_OPEN BIT(0)
/**
 * Configure pin with pull down
 */
#define MUX_CTL_PULL_DOWN BIT(1)
/**
 * Configure pin with pull up
 */
#define MUX_CTL_PULL_UP BIT(2)
/**
 * Configure pin with Schmitt Trigger
 */
#define MUX_CTL_SCHMITT BIT(3)
/**
 * Configure mode of pin
 */
#define MUX_CTL_MODE(x) (x << 8)
/**
 * Configure pin
 * 
 * 
 * \param mux Mux handle
 * \param pin Pin Number
 * \param ctl Config Pin can be: {@link MUX_CTL_OPEN} or {@link MUX_CTL_PULL_DOWN} or {@link MUX_CTL_PULL_UP} and {@link MUX_CTL_SCHMITT} and {@link MUX_CTL_MODE} Options can mixed with Or Operator: Example: ({@link MUX_CTL_PULL_DOWN} | {@link MUX_CTL_SCHMITT} | {@link MUX_CTL_MODE(1)})
 * \param extra Extra config for mach specific settings 
 * \return -1 on error and 0 on ok 
 */
int32_t mux_pinctl(struct mux *mux, uint32_t pin, uint32_t ctl, uint32_t extra);
/**\}*/
#endif
