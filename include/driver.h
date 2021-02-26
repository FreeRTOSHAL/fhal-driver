/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2021
 */
#ifndef DRIVER_H_
#ifdef LINKER_SCRIPT
/**
 * \defgroup DRIVER Driver specific Linker Macros 
 * \ingroup LINKER
 * \code
 * #include <driver.h>
 * \endcode
 * \{
 */
#include <linker.h>
/**
 * Default Devices
 * \param location Location
 */
#define DEV_DEFAULT(location) \
	SECTION_START(.rodata.devs) \
	SYMBOL(_devs); \
	KEEP(*(.rodata.devs)) \
	SYMBOL(_devs_end); \
	SECTION_STOP(location)
#endif
/**\}*/
#endif
