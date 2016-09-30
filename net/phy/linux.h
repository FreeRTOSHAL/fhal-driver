#ifndef LINUX_H_
#define LINUX_H_
#include <stdint.h>
#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t
#define int int32_t
#define phy_device phy
#define phy_read(dev, reg) ({ \
	uint16_t val; \
	int32_t ret; \
	ret = phy_read(dev, reg, &val); \
	((ret < 0)? ret : (int32_t) val);\
})
#define mutex_lock(l)
#define mutex_unlock(l)
#endif
