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
#ifndef CLOCK_H_
#define CLOCK_H_
#include <stdint.h>
#include <stdbool.h>
/**
 * \defgroup CLOCK Clock Subsystem
 * \ingroup HAL
 * \code
 * #include <accel.h>
 * \endcode
 * 
 * This is the Clock Subsystem.
 * \todo Implement Clock Subsystem for better support for Power Mangement
 * \{
 */
struct clock;
/**
 * Generic Clock Interface
 */
struct clock_generic {
	bool init;
};
/**
 * Clock Init
 * \return Clock Instance or Null on error
 */
struct clock *clock_init();
/**
 * Get CPU Clock Speed
 * \param clk CLock Instance
 * \return -1 on error >= 0 CPU Speed in Hz
 */
int64_t clock_getCPUSpeed(struct clock *clk);
/**
 * Get Periphery Clock Speed
 * \param clk CLock Instance
 * \param id Periphery Clock ID
 * \return -1 on error >= 0 Periphery Speed in Hz
 */
int64_t clock_getPeripherySpeed(struct clock *clk, uint32_t id);
/**
 * Deinit Clock
 * \param clk CLock Instance
 * \return -1 on error 0 on ok
 */
int32_t clock_deinit(struct clock *clk);
/**\}*/
#endif
