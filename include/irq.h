#ifndef IRQ_H_
#define IRQ_H_
#include <stdint.h>
int32_t irq_init();
int32_t irq_enable(int32_t irqnr);
int32_t irq_disable(int32_t irqnr);
int32_t irq_notify(int32_t cpuid, int32_t irqnr);
int32_t irq_clear(int32_t irqnr);
int32_t irq_getCPUID();
int32_t irq_setPrio(int32_t irqnr, int32_t prio);
int32_t irq_getPrio(int32_t irqnr);
int32_t irq_mask(int32_t irqnr);
int32_t irq_unmask(int32_t irqnr);
#endif
