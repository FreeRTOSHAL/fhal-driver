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
