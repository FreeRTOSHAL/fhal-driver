#ifndef HAL_H_
#define HAL_H_
#include <FreeRTOS.h>
#include <semphr.h>
#include <system.h>
#include <stdbool.h>
struct hal {
	bool init;
	SemaphoreHandle_t lock;	
};

inline int32_t hal_init(void *data) {
	struct hal *hal = data;
	hal->lock = xSemaphoreCreateRecursiveMutex();
	if (hal->lock == NULL) {
		return -1;
	}
	return 0;
}

inline int32_t hal_deinit(void *data) {
	struct hal *hal = data;
	vSemaphoreDelete(hal->lock);
	return 0;
}

inline bool hal_isInit(void *data) {
	struct hal *hal = data;
	return hal->init;
}

inline int32_t hal_lock(void *data, TickType_t waittime) {
	struct hal *hal = data;
	return xSemaphoreTakeRecursive(hal->lock, waittime);
}

inline int32_t hal_unlock(void *data) {
	struct hal *hal = data;
	return xSemaphoreGiveRecursive(hal->lock);
}

#define HAL_LOCK(data, waittime, errcode) do { \
	int32_t lock_ret = hal_lock(data, waittime); \
	if (lock_ret != 1) { \
		return errcode; \
	}; \
} while(0) 

#define HAL_UNLOCK(data, errcode) do { \
	int32_t unlock_ret = hal_unlock(data); \
	if (unlock_ret != 1) { \
		return errcode; \
	} \
} while(0)

#define HAL_ADDDEV(gns, ns, p) static struct gns##_generic SECTION(".rodata.dev." #gns) USED const * const ns##_##p = (struct gns##_generic const *) &p

/*
 * Container for Driver without Interface like the MPU9250 Driver
 */
extern void **hals;
#define HAL_ADD(ns, p) static void SECTION(".rodata.dev.hal") USED const * const ns##_##p = (void const *) &p

#endif
