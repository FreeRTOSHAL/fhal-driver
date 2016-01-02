#ifndef ACCEL_H_
#define ACCEL_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal.h>
#include <vec.h>
struct accel;
#ifdef CONFIG_ACCEL_MULTI
struct accel_ops {
	struct accel *(*accel_init)(uint32_t index);
	int32_t (*accel_deinit)(struct accel *accel);

	int32_t (*accel_get)(struct accel *accel, struct vector *vector, TickType_t waittime);
	int32_t (*accel_getISR)(struct accel *accel, struct vector *vector);
};
#endif
struct accel_generic {
	bool init;
#ifdef CONFIG_ACCEL_THREAD_SAVE
	SemaphoreHandle_t lock;	
#endif
#ifdef CONFIG_ACCEL_MULTI
	const struct accel_ops *ops;
#endif
};
extern struct accel **accels;

#ifndef CONFIG_ACCEL_MULTI
struct accel *accel_init(uint32_t index);
int32_t accel_deinit(struct accel *accel);

int32_t accel_get(struct accel *accel, struct vector *vector, TickType_t waittime);
int32_t accel_getISR(struct accel *accel, struct vector *vector);
#else
inline struct accel *accel_init(uint32_t index) {
	struct accel_generic *a = (struct accel_generic *) accels[index];
	return a->ops->accel_init(index);
}
inline int32_t accel_deinit(struct accel *accel) {
	struct accel_generic *a = (struct accel_generic *) accel;
	return a->ops->accel_deinit(accel);
}

inline int32_t accel_get(struct accel *accel, struct vector *vector, TickType_t waittime) {
	struct accel_generic *a = (struct accel_generic *) accel;
	return a->ops->accel_get(accel, vector, waittime);
}
inline int32_t accel_getISR(struct accel *accel, struct vector *vector) {
	struct accel_generic *a = (struct accel_generic *) accel;
	return a->ops->accel_getISR(accel, vector);
}
#endif

#endif
