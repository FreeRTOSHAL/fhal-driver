/*
 * Copyright (c) 2016 Andreas Werner <kernel@andy89.org>
 * 
 * Permission is hereby granted, free of charge, to any person 
 * obtaining a copy of this software and associated documentation 
 * files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, 
 * publish, distribute, sublicense, and/or sell copies of the Software, 
 * and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
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
