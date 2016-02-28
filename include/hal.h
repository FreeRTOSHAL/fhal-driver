#ifndef HAL_H_
#define HAL_H_
#include <FreeRTOS.h>
#include <semphr.h>
#include <system.h>
#include <stdbool.h>

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
#ifdef CONFIG_USE_RECURSIVE_MUTEXES
	/**
	 * Driver Access Lock
	 */
	SemaphoreHandle_t lock;	
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
	hal->lock = xSemaphoreCreateRecursiveMutex();
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
 * Add Device Entry to Device Array
 * \param gns Global Namespace of Driver like uart, pwm, ...
 * \param ns Namespace of Driver
 * \param p Pointer to Driver Struct
 */
#define HAL_ADDDEV(gns, ns, p) static struct gns##_generic SECTION(".rodata.dev." #gns) USED const * const ns##_##p = (struct gns##_generic const *) &p

/**
 * Container for Driver without Interface like the MPU9250 Driver
 */
extern void **hals;
/**
 * Add some Devices without Global Namespace like a instances of MPU9250 Driver
 */
#define HAL_ADD(ns, p) static void SECTION(".rodata.dev.hal") USED const * const ns##_##p = (void const *) &p
/**\}*/
#endif
