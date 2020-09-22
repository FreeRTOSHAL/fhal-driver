/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef EXAMPLE_PRV_
#define EXAMPLE_PRV_
#ifndef EXAMPLE_PRV
# error "Never include this file out of a EXAMPLE driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
/**
 * \defgroup example_driver_prv Driver View form Example driver 
 * \ingroup example_driver
 * 
 * This is the Driver View form Example Driver. 
 * 
 * This is a implementation of a driver 
 * \include driver/example/example_dev0.c
 * 
 * Driver with board specific like the MPU9247 has special macros to create new Instances. (for example: MPU9250_ADDDEV())
 * \{
 */
/**
 * Error Code Returned if Already inited
 */
#define EXAMPLE_ALREDY_INITED 1
/**
 * Generic Init Function
 * 
 * Check Driver Already inited and init mutex if active
 * \param example Instants of Driver
 * \return EXAMPLE_ALREDY_INITED on alredy init 0 on not init < 0 error
 */
int32_t example_genericInit(struct example *example);
#ifdef CONFIG_EXAMPLE_THREAD_SAVE
/**
 * Lock Driver
 * if THREAD_SAVE is not defined this instruction has no function
 */
# define example_lock(u, w, e) HAL_LOCK(u, w, e)
/**
 * Unlock Driver
 * if THREAD_SAVE is not defined this instruction has no function
 */
# define example_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define example_lock(u, w, e)
# define example_unlock(u, e)
#endif
/**
 * Add Driver Instance
 * \param ns Driver namespace
 * \param p Pointer to driver instance
 */
#define EXAMPLE_ADDDEV(ns, p) HAL_ADDDEV(example, ns, p)
/*
 * Define Global Array for Driver Access
 */
HAL_DEFINE_GLOBAL_ARRAY(example);
/**
 * Simple function for access a dev from driver
 * \param index index
 * \return see HAL_GET_DEV
 */
#define EXAMPLE_GET_DEV(index) HAL_GET_DEV(example, index)
#ifndef CONFIG_EXAMPLE_MULTI
# define EXAMPLE_OPS(ns)
# define EXAMPLE_INIT_DEV(ns) 

# define EXAMPLE_INIT(ns, index) struct example *example_init(uint32_t index)
# define EXAMPLE_DEINIT(ns, p) int32_t example_deinit(struct example *p)

# define EXAMPLE_FUNCNAME(ns, p, param) int32_t example_funcname(struct example *p, uint32_t param)
#else
/**
 * Define Operation
 * \param ns Driver Namespace
 */
# define EXAMPLE_OPS(ns) static const struct example_ops ns##_example_ops = { \
	.example_init = &ns##_example_init,\
	.example_deinit = &ns##_example_deinit,\
	.example_funcname = &ns##_example_funcname, \
}
/**
 * Init Driver Struct 
 * \param ns Driver Namespace
 */
# define EXAMPLE_INIT_DEV(ns) .gen.ops = &ns##_example_ops,

/**
 * Define example_init() Implementation 
 * \param ns Driver namespace Variablenname
 * \param index Index Variablenname
 */
# define EXAMPLE_INIT(ns, index) static struct example *ns##_example_init(uint32_t index)
/**
 * Define example_deinit() Implementation
 * \param ns Driver namespace Variablenname
 * \param p Instance Variablenname
 */
# define EXAMPLE_DEINIT(ns, p) static int32_t ns##_example_deinit(struct example *p)
/**
 * Define example_funcname() Implementation
 * \param ns Driver namespace Variablenname
 * \param p Instance Variablenname
 * \param param Param Variablenname
 */
# define EXAMPLE_FUNCNAME(ns, p, param) static int32_t ns##_example_funcname(struct example *p, uint32_t param)
#endif
/**\}*/
#endif
