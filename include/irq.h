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
#ifndef IRQ_H_
#define IRQ_H_
#include <stdint.h>
/**
 * \defgroup IRQ IRQ Subsystem
 * \ingroup HAL
 * \code
 * #include <irq.h>
 * \endcode
 * 
 * This is IRQ Subsystem
 * \{
 */
/**
 * IRQ init Subsystem
 *
 * \return -1 on error 0 on ok
 */
int32_t irq_init();
/**
 * Enable IRQ
 * 
 * \param irqnr IRQ Number
 * \return -1 on error 0 on ok
 */
int32_t irq_enable(int32_t irqnr);
/**
 * Disable IRQ
 * 
 * \param irqnr IRQ Number
 * \return -1 on error 0 on ok
 */
int32_t irq_disable(int32_t irqnr);
/**
 * Notify CPU
 * 
 * \param cpuid CPU ID
 * \param irqnr IRQ Number
 * \return -1 on error 0 on ok
 */
int32_t irq_notify(int32_t cpuid, int32_t irqnr);
/**
 * Clear IRQ
 * 
 * \param irqnr IRQ Number
 * \return -1 on error 0 on ok
 */
int32_t irq_clear(int32_t irqnr);
/**
 * Get CPU ID
 * 
 * \return CPU ID
 */
int32_t irq_getCPUID();
/**
 * Set IRQ Prio
 * 
 * \param irqnr IRQ Number
 * \param prio Prio
 * \return -1 on error 0 on ok
 */
int32_t irq_setPrio(int32_t irqnr, int32_t prio);
/**
 * Get Prio of IRQ
 * 
 * \param irqnr IRQ Number
 * \return Prio of IRQ
 */
int32_t irq_getPrio(int32_t irqnr);
/**
 * Mask IRQ
 * 
 * \param irqnr IRQ Number
 * \return -1 on error 0 on ok
 */
int32_t irq_mask(int32_t irqnr);
/**
 * Unmask IRQ
 * 
 * \param irqnr IRQ Number
 * \return -1 on error 0 on ok
 */
int32_t irq_unmask(int32_t irqnr);
/**\}*/
#endif
