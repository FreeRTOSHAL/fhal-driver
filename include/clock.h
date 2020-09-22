/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
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
