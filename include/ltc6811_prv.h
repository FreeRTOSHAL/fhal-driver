#ifndef LTC6811_PRV_H_
#define LTC6811_PRV_H_
#ifndef LTC6811_PRV
# error "Never include this file out of a SPI driver"
#endif
#include <ltc6811.h>
#ifdef CONFIG_LTC6811_DEBUG
# define PRINTF(fmt, ...) printf("LTC: " fmt, ##__VA_ARGS__)
#else
# define PRINTF(fmt, ...)
#endif
#define ltc6811_lock(u, w, e) HAL_LOCK(u, w, e)
#define ltc6811_unlock(u, e) HAL_UNLOCK(u, e)
#endif
