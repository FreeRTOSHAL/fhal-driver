#ifndef GYRO_H_
#define GYRO_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal.h>
#include <vec.h>
/**
 * \defgroup GYRO Gyroscope Subsystem
 * \ingroup HAL
 * \code
 * #include <accel.h>
 * \endcode
 * 
 * This is the Gyroscope Subsystem.
 * \{
 */
/**
 * Private Structure of Gyroscope driver
 */
struct gyro;
#ifdef CONFIG_GYRO_MULTI
/**
 * Function of Gyro driver in Multi Driver implementation mode 
 */
struct gyro_ops {
	struct gyro *(*gyro_init)(uint32_t index);
	int32_t (*gyro_deinit)(struct gyro *gyro);

	int32_t (*gyro_get)(struct gyro *gyro, struct vector *vector, TickType_t waittime);
	int32_t (*gyro_getISR)(struct gyro *gyro, struct vector *vector);
};
#endif
/**
 * Generic Gyroscope Interface  
 */
struct gyro_generic {
	/**
	 * true = is init
	 * false = is not init
	 */
	bool init;
#ifdef CONFIG_GYRO_THREAD_SAVE
	/**
	 * Mutex
	 */
	SemaphoreHandle_t lock;	
#endif
#ifdef CONFIG_GYRO_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct gyro_ops *ops;
#endif
};

#ifndef CONFIG_GYRO_MULTI
/**
 * Init Gyroscope
 * \param index Index of Gyroscope
 * \return Gyroscope Instance or NULL on error
 */
struct gyro *gyro_init(uint32_t index);
/**
 * Deinit Gyroscope
 * \param gyro Gyroscope Instance
 * \return -1 on error 0 on ok
 */
int32_t gyro_deinit(struct gyro *gyro);
/**
 * Get Gyroscope Value
 * \param gyro Gyroscope Instance
 * \param vector Values
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t gyro_get(struct gyro *gyro, struct vector *vector, TickType_t waittime);
/**
 * Get Gyroscope Value in Interrupt Service Routine
 * \param gyro Gyroscope Instance
 * \param vector Values
 * \return -1 on error 0 on ok
 */
int32_t gyro_getISR(struct gyro *gyro, struct vector *vector);
#else
inline struct gyro *gyro_init(uint32_t index) {
	HAL_DEFINE_GLOBAL_ARRAY(gyro);
	struct gyro_generic *a = (struct gyro_generic *) HAL_GET_DEV(gyro, index);
	if (a == NULL) {
		return NULL;
	}
	return a->ops->gyro_init(index);
}
inline int32_t gyro_deinit(struct gyro *gyro) {
	struct gyro_generic *a = (struct gyro_generic *) gyro;
	return a->ops->gyro_deinit(gyro);
}

inline int32_t gyro_get(struct gyro *gyro, struct vector *vector, TickType_t waittime) {
	struct gyro_generic *a = (struct gyro_generic *) gyro;
	return a->ops->gyro_get(gyro, vector, waittime);
}
inline int32_t gyro_getISR(struct gyro *gyro, struct vector *vector) {
	struct gyro_generic *a = (struct gyro_generic *) gyro;
	return a->ops->gyro_getISR(gyro, vector);
}
#endif
/**\}*/
#endif
