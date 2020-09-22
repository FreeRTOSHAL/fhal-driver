/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef VDEV_H_
#define VDEV_H_
#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <semphr.h>
#include <hal.h>
struct vdev;
struct vdev_ops {
	struct vdev *(*vdev_init)(uint32_t index, struct virtio *virtio, uint8_t size, struct vring_config *config);
	int32_t vdev_deinit(struct vdev *vdev);
};
struct vdev_config {
	enum vdev_type id;
	uint32_t notifyid;
	uint32_t features;
	uint32_t configLen;
	void *config;
	uint8_t numOfVrings;
	struct vring_config vrings[0];
};
struct vring_config {
	uint32_t va;
	uint32_t da;
	uint32_t align;
	uint32_t notifyid;
};
/**
 * All driver shall implement this Struct
 * 
 * For Example: 
 * \code{.c}
 * struct vdev {
 * 	struct vdev_generic gen;
 * }
 * \endcode
 */
struct vdev_generic {
	/**
	 * true = is init
	 * false = is not init
	 */
	bool init;
	struct vdev_config config;
	struct virtio *virtio;
	/**
	 * Ops of driver in Multi mode
	 */
	const struct vdev_ops *ops;
#ifdef CONFIG_INSTANCE_NAME
	/**
	 * Name of Driver Instance for Debugging 
	 */
	const char *name;
#endif
#ifdef CONFIG_VDEV_THREAD_SAVE
	/**
	 * Mutex
	 */
	SemaphoreHandle_t lock;	
#endif
};
inline struct vdev *vdev_init(uint32_t index, struct virtio *virtio, uint8_t size, struct vring_config *config) {
	HAL_DEFINE_GLOBAL_ARRAY(vdev);
	struct vdev_generic *gen = HAL_GET_DEV(vdev, index);
	if (gen == NULL) {
		return NULL;
	}
	return gen->ops->vdev_init(index, virtio, size, config);
}
inline int32_t vdev_deinit(struct vdev *vdev) {
	struct vdev_generic *gen = vdev;
	return gen->deinit(vdev);
}
inline int32_t vdev_getID(struct vdev *vdev) {
	struct vdev_generic *gen = vdev;
	return gen->config.id;
}
inline bool vdev_hasFeatures(struct vdev *vdev, uint32_t feature) {
	struct vdev_generic *gen = vdev;
	return (vdev.config->features & BIT(feature)) != 0;
}
#endif
