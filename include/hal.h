/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2021
 */
#ifndef HAL_H_
#define HAL_H_
#include <FreeRTOS.h>
#include <semphr.h>
#include <system.h>
#include <stdbool.h>
#include <os.h>

/**
 * \defgroup HAL Hardware Abstraction Layer for FreeRTOS
 * 
 * \code
 * #include <hal.h>
 * \endcode
 * 
 * This is the abstract Hardware Abstraction Layer Interface (HAL) is the main Interface for all Driver. All Driver shall implement the 
 * Interface. 
 * 
 * All Driver Interfaces implemented the {@link hal} struct and have a init Function and a deinit Function. The init function return 
 * a Handler to access the instances of the driver. The deinit function disable the driver instance. 
 * 
 * Each Driver has the possibility to make the instances threadsave. With the function lock and unlock the Driver can lock an unlock the instances.
 * 
 * All Pointer to all Instants stored in a separated Section sored by driver type (like uart, pwm, timer, ..) 
 * 
 * For driver without a specified type a generic Array is created named hal use HAL_ADD() Macro to create a new entry. 
 * 
 * The Linker Script must contain {@link DEV_DEFAULT()}. 
 * 
 * All driver instants are static allocated. 
 * 
 * \{
 */

/**
 * Driver Main Struct
 * All Driver shall implement this struct
 */
struct hal {
	/**
	 * Detect driver is Init or not
	 */
	bool init;
#ifdef CONFIG_INSTANCE_NAME
	/**
	 * Name of Driver Instance for Debugging 
	 */
	const char *name;
#endif
#ifdef CONFIG_USE_RECURSIVE_MUTEXES
	/**
	 * Driver Access Lock
	 */
	OS_DEFINE_MUTEX_RECURSIVE(lock);
#endif
};
/**
 * Init Hal driver
 * \param data Driver Struct like {@link hal}
 * \return -1 on error 0 on ok
 */
inline int32_t hal_init(void *data) {
#ifdef CONFIG_USE_RECURSIVE_MUTEXES
	struct hal *hal = data;
	hal->lock = OS_CREATE_MUTEX_RECURSIVE(hal->lock);
	if (hal->lock == NULL) {
		return -1;
	}
#endif
	return 0;
}
/**
 * Deinit Hal Driver
 * \param data Driver Struct like {@link hal}
 * \return -1 on error 0 on ok
 */
inline int32_t hal_deinit(void *data) {
#ifdef CONFIG_USE_RECURSIVE_MUTEXES
	struct hal *hal = data;
	vSemaphoreDelete(hal->lock);
#endif
	return 0;
}
/**
 * Check Driver is Init
 * \param data Driver Struct like {@link hal}
 * \return true = on is init false on is not init
 */
inline bool hal_isInit(void *data) {
	struct hal *hal = data;
	return hal->init;
}
/**
 * Lock Driver
 * \param data Driver Struct like {@link hal}
 * \param waittime Max Waittime see xSemaphoreTakeRecursive()
 * \return 0 on error 1 on ok like xSemaphoreTakeRecursive()
 */
inline int32_t hal_lock(void *data, TickType_t waittime) {
#ifdef CONFIG_USE_RECURSIVE_MUTEXES
	struct hal *hal = data;
	return xSemaphoreTakeRecursive(hal->lock, waittime);
#else
	return 1;
#endif
}

/**
 * Unlock Driver
 * \param data Driver Struct like {@link hal}
 * \return 0 on error 1 on ok like xSemaphoreTakeRecursive()
 */
inline int32_t hal_unlock(void *data) {
#ifdef CONFIG_USE_RECURSIVE_MUTEXES
	struct hal *hal = data;
	return xSemaphoreGiveRecursive(hal->lock);
#else
	return 1;
#endif
}
#ifdef CONFIG_INSTANCE_NAME
/**
 * Print All Driver Name and her Index
 * printf is needet!!
 * \return -1 on error and 0 on ok
 */
int32_t hal_printNames();
#endif
/**
 * Get Device form global Array
 * \param devs Array
 * \param end Last Position in Array
 * \param index Index in Array
 * \return NULL if index to big else pointer to dev
 */
inline uintptr_t *hal_getDev(uintptr_t **devs, uintptr_t **end, uint32_t index) {
	if ((devs + index) >= end) {
		return NULL;
	}
	return devs[index];
}
/**
 * Define Global Array for Namespace
 * \param gns Namespace like uart
 */
#define HAL_DEFINE_GLOBAL_ARRAY(gns) \
		extern uintptr_t *_devs; \
		extern uintptr_t *_devs##_end
/**
 * Get Device form global Array
 * 
 * Call HAL_DEFINE_GLOBAL_ARRAY() before
 * \param gns Namespace like uart
 * \param index Index in Array
 * \return see hal_getDev()
 */
#define HAL_GET_DEV(gns, index) (void *) hal_getDev(&_devs, &_devs##_end, index)
/**
 * Lock Driver
 * \param data Driver Struct like {@link hal}
 * \param waittime Max Waittime see xSemaphoreTakeRecursive()
 * \param errcode Error Code automatically returned
 */
#define HAL_LOCK(data, waittime, errcode) do { \
	int32_t lock_ret = hal_lock(data, waittime); \
	if (lock_ret != 1) { \
		return errcode; \
	}; \
} while(0) 

/**
 * Unlock Driver
 * \param data Driver Struct like hal
 * \param errcode Error Code automatically returned
 */
#define HAL_UNLOCK(data, errcode) do { \
	int32_t unlock_ret = hal_unlock(data); \
	if (unlock_ret != 1) { \
		return errcode; \
	} \
} while(0)
/**
 * Define Device Entry
 * \param gns Global Namespace of Driver like uart, pwm, ...
 * \param ns Namespace of Driver
 * \param p Pointer to Driver Struct
 */
#define HAL_DEFINE_DEVICE_ENTRY(gns, ns, p) extern struct gns##_generic const * const ns##_##p

/**
 * Add Device Entry to Device Array
 * \param gns Global Namespace of Driver like uart, pwm, ...
 * \param ns Namespace of Driver
 * \param p Pointer to Driver Struct
 */
#define HAL_ADDDEV(gns, ns, p) struct gns##_generic SECTION(".rodata.devs") USED NO_REORDER const * const ns##_##p = (void const *) &p

/**
 * Add some Devices without Global Namespace like a instances of MPU9250 Driver
 */
#define HAL_ADD(ns, p) HAL_ADDDEV(hal, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(hal);
/**
 * Get device form global Namespace
 */
#define HAL_GET_GLOBAL_DEV(index) HAL_GET_DEV(hal, index)
#ifdef CONFIG_INSTANCE_NAME
/**
 * Define Driver Instance Name
 * \param n Name
 */
# define HAL_NAME(n) .gen.name = n,
#else
# define HAL_NAME(n)
#endif
/**
 * Get Unique ID based on driver pointer
 * \param gns Namespace like uart
 * \param ns Namespace of Driver
 * \param p Pointer to Driver Struct
 */
#define HAL_GET_ID(gns, ns, p) ({ \
		HAL_DEFINE_DEVICE_ENTRY(gns, ns, p); \
		uint32_t ret; \
		ret = (uint32_t) ((((uintptr_t) (&ns##_##p)) - ((uintptr_t) (&_devs))) / sizeof(uintptr_t)); \
		ret; \
	})

#ifdef CONFIG_GEN_VERSION
extern const char *versionDriver;
extern const char *versionMach;
extern const char *versionArch;
extern const char *versionKernel;
#endif

/**\}*/
#endif
