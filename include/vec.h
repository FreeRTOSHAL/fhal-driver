/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef VEC_H_
#define VEC_H_
#include <stdint.h>
/**
 * Vector
 */
struct vector {
	/**
	 * x in RAW
	 */
	int16_t x;
	/**
	 * y in RAW
	 */
	int16_t y;
	/**
	 * z in RAW
	 */
	int16_t z;
};
#endif
