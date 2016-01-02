#ifndef GYRO_H_
#define GYRO_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal.h>
#include <vec.h>
struct gyro;
#ifdef CONFIG_GYRO_MULTI
struct gyro_ops {
	struct gyro *(*gyro_init)(uint32_t index);
	int32_t (*gyro_deinit)(struct gyro *gyro);

	int32_t (*gyro_get)(struct gyro *gyro, struct vector *vector, TickType_t waittime);
	int32_t (*gyro_getISR)(struct gyro *gyro, struct vector *vector);
};
#endif
struct gyro_generic {
	bool init;
#ifdef CONFIG_GYRO_THREAD_SAVE
	SemaphoreHandle_t lock;	
#endif
#ifdef CONFIG_GYRO_MULTI
	const struct gyro_ops *ops;
#endif
};
extern struct gyro **gyros;

#ifndef CONFIG_GYRO_MULTI
struct gyro *gyro_init(uint32_t index);
int32_t gyro_deinit(struct gyro *gyro);

int32_t gyro_get(struct gyro *gyro, struct vector *vector, TickType_t waittime);
int32_t gyro_getISR(struct gyro *gyro, struct vector *vector);
#else
inline struct gyro *gyro_init(uint32_t index) {
	struct gyro_generic *a = (struct gyro_generic *) gyros[index];
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

#endif
