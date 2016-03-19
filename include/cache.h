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
#ifndef CACHE_H_
#define CACHE_H_
#include <stdint.h>
/**
 * \defgroup CACHE Cache Subsystem
 * \ingroup HAL
 * \code
 * #include <cache.h>
 * \endcode
 * 
 * This is the Cache Subsystem.
 * \{
 */
/**
 * Cache Init 
 * \return -1 on error 0 on ok
 */
int32_t cache_init();
/**
 * Flash All data in Data Cache
 * \return -1 on error 0 on ok
 */
int32_t cache_flushDataAll();
/**
 * Flush Data 
 * \param addr Address
 * \param size Size
 * \return -1 on error 0 on ok
 */
int32_t cache_flushData(uint32_t *addr, uint32_t size);
/**
 * INvalid all data in Data Cache
 * \return -1 on error 0 on ok
 */
int32_t cache_invalidDataAll();
/**
 * Invalid Data 
 * \param addr Address
 * \param size Size
 * \return -1 on error 0 on ok
 */
int32_t cache_invalidData(uint32_t *addr, uint32_t size);
/**\}*/
#endif
