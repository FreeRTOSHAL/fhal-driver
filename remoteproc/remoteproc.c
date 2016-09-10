#include <stdio.h>
#include <mailbox.h>
#include <remoteproc.h>
struct rproc {
	const struct rproc_ops *ops;
	struct resource_table *rsc;
	bool master;
	uint32_t cpuID;
	void *priv;
};
static int32_t handleCarveout(struct fw_rsc_carveout *carveout) {
	printf("%s\n", __FUNCTION__);
	printf("carveout: da: 0x%08lx pa: 0x%08lx len: 0x%08lx flags: 0x%08lx name: %s\n", carveout->da, carveout->pa, carveout->len, carveout->flags, carveout->name);
	return 0;
}
static int32_t handleDevMem(struct fw_rsc_devmem *devmem) {
	printf("%s\n", __FUNCTION__);
	printf("dev mem: da: 0x%08lx pa: 0x%08lx len: 0x%08lx flags: 0x%08lx name: %s\n", devmem->da, devmem->pa, devmem->len, devmem->flags, devmem->name);
	return 0;
}
static int32_t handleTrace(struct fw_rsc_trace *trace) {
	printf("%s\n", __FUNCTION__);
	printf("trace buffer: da: 0x%08lx len: 0x%08lx name: %s\n", trace->da, trace->len, trace->name);
	return 0;
}
static int32_t handleVdev(struct fw_rsc_vdev *vdev) {
	uint8_t i;
	printf("%s\n", __FUNCTION__);
	printf("vdev: id: %ld notifyid: %ld dfeatures: 0x%08lx gfeatures: 0x%08lx config_len: 0x%08lx status: 0x%08x num of vrings: %d\n", vdev->id, vdev->notifyid, vdev->dfeatures, vdev->gfeatures, vdev->config_len, vdev->status, vdev->num_of_vrings);
	for (i = 0; i < vdev->num_of_vrings; i++) {
		struct fw_rsc_vdev_vring *vring = &vdev->vring[i];
		printf("\tvring: da: 0x%08lx align: 0x%08lx num: 0x%08lx notifyid: 0x%08lx\n", vring->da, vring->align, vring->num, vring->notifyid);
	}
	return 0;
}
static int32_t parseRSC(struct rproc *rproc) {
	struct resource_table *rsc = rproc->rsc;
	uint32_t i;
	struct fw_rsc_hdr *hdr;
	if (rsc == NULL) {
		return -1;
	}
	if (rsc->ver != 1) {
		return -1;
	}
	for (i = 0; i < rsc->num; i++) {
		hdr = (struct fw_rsc_hdr *) (((uintptr_t) rsc) + (uintptr_t) rsc->offset[i]);
		if (hdr->type > RSC_LAST) {
		}
		switch (hdr->type) {
			case RSC_CARVEOUT:
				handleCarveout((struct fw_rsc_carveout *) hdr);
				break;
			case RSC_DEVMEM:
				handleDevMem((struct fw_rsc_devmem *) hdr);
				break;
			case RSC_TRACE:
				handleTrace((struct fw_rsc_trace *) hdr);
				break;
			case RSC_VDEV:
				handleVdev((struct fw_rsc_vdev *) hdr);
				break;
			default:
				printf("type: %lu not supportet", hdr->type);
				continue;
		}
	}
	return 0;
}
struct rproc *rproc_init(const struct rproc_ops *ops, void *initData, struct resource_table *rsc, uint32_t cpuID, bool master) {
	int32_t ret;
	struct rproc *rproc = pvPortMalloc(sizeof(struct rproc));
	if (rproc == NULL) {
		goto rproc_init_error0;
	}
	rproc->ops = ops;
	rproc->rsc = rsc;
	rproc->master = master;
	rproc->cpuID = cpuID;
	/**
	 * Init Interrupt based Comunication Subsystem
	 */
	rproc->priv = ops->init(rproc, initData, cpuID);
	if (rproc->priv == NULL) {
		goto rproc_init_error1;
	}
	ret = parseRSC(rproc);
	if (ret < 0) {
		goto rproc_init_error2;
	}
	return rproc;
rproc_init_error2:
	rproc->ops->deinit(rproc, rproc->priv);
rproc_init_error1:
	vPortFree(rproc);
rproc_init_error0:
	return NULL;
}
int32_t rproc_deinit(struct rproc *rproc) {
	rproc->ops->deinit(rproc, rproc->priv);
	vPortFree(rproc);
	return 0;
}
int32_t rproc_isr(struct rproc *rproc, uint32_t virtID) {
	/* TODO */
	return 0;
}
int32_t rproc_notify(struct rproc *rproc, uint32_t virtID) {
	return rproc->ops->notify(rproc, rproc->priv, virtID);
}
