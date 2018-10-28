#ifndef REMOTEPROC_H_
#define REMOTEPROC_H_
#include <stdint.h>
#include <system.h>
#include <mailbox.h>
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
#ifndef __packed
# define __packed PACKED
#endif
/**
 * \defgroup Remoteproc Remoteproc Subssystem
 * \ingroup HAL
 * Remoteproc based on Linux Kernel Documention
 * \{
 */
/**
 * struct resource_table - firmware resource table header
 *
 * A resource table is essentially a list of system resources required
 * by the remote processor. It may also include configuration entries.
 * If needed, the remote processor firmware should contain this table
 * as a dedicated ".resource_table" ELF section.
 *
 * Some resources entries are mere announcements, where the host is informed
 * of specific remoteproc configuration. Other entries require the host to
 * do something (e.g. allocate a system resource). Sometimes a negotiation
 * is expected, where the firmware requests a resource, and once allocated,
 * the host should provide back its details (e.g. address of an allocated
 * memory region).
 *
 * The header of the resource table, as expressed by this structure,
 * contains a version number (should we need to change this format in the
 * future), the number of available resource entries, and their offsets
 * in the table.
 *
 * Immediately following this header are the resource entries themselves,
 * each of which begins with a resource entry header (as described below).
 */
struct resource_table {
	/**
	 * version number
	 */
	u32 ver;
	/**
	 * number of resource entries
	 */
	u32 num;
	/**
	 * reserved (must be zero)
	 */
	u32 reserved[2];
	/**
	 * array of offsets pointing at the various resource entries
	 */
	uint32_t offset[0];
} __packed;

/**
 * struct fw_rsc_hdr - firmware resource entry header
 *
 * Every resource entry begins with a 'struct fw_rsc_hdr' header providing
 * its type. The content of the entry itself will immediately follow
 * this header, and it should be parsed according to the resource type.
 */
struct fw_rsc_hdr {
	/**
	 * resource type
	 */
	u32 type;
} __packed;

/**
 * enum fw_resource_type - types of resource entries
 *
 *
 * For more details regarding a specific resource type, please see its
 * dedicated structure below.
 *
 * Please note that these values are used as indices to the rproc_handle_rsc
 * lookup table, so please keep them sane. Moreover, RSC_LAST is used to
 * check the validity of an index before the lookup table is accessed, so
 * please update it as needed.
 */
enum fw_resource_type {
	/**
	 * request for allocation of a physically contiguous
	 * memory region.
	 */
	RSC_CARVEOUT	= 0,
	/**
	 * request to iommu_map a memory-based peripheral.
	 */
	RSC_DEVMEM	= 1,
	/**
	 * announces the availability of a trace buffer into which
	 * the remote processor will be writing logs.
	 */
	RSC_TRACE	= 2,
	/**
	 * declare support for a virtio device, and serve as its
	 * virtio header.
	 */
	RSC_VDEV	= 3,
	/**
	 * just keep this one at the end
	 */
	RSC_LAST	= 4,
};

#define FW_RSC_ADDR_ANY (0xFFFFFFFFFFFFFFFF)

/**
 * struct fw_rsc_carveout - physically contiguous memory request
 *
 * This resource entry requests the host to allocate a physically contiguous
 * memory region.
 *
 * These request entries should precede other firmware resource entries,
 * as other entries might request placing other data objects inside
 * these memory regions (e.g. data/code segments, trace resource entries, ...).
 *
 * Allocating memory this way helps utilizing the reserved physical memory
 * (e.g. CMA) more efficiently, and also minimizes the number of TLB entries
 * needed to map it (in case rproc is using an IOMMU). Reducing the TLB
 * pressure is important; it may have a substantial impact on performance.
 *
 * If the firmware is compiled with static addresses, then da should specify
 * the expected device address of this memory region. If da is set to
 * FW_RSC_ADDR_ANY, then the host will dynamically allocate it, and then
 * overwrite da with the dynamically allocated address.
 *
 * We will always use da to negotiate the device addresses, even if it
 * isn't using an iommu. In that case, though, it will obviously contain
 * physical addresses.
 *
 * Some remote processors needs to know the allocated physical address
 * even if they do use an iommu. This is needed, e.g., if they control
 * hardware accelerators which access the physical memory directly (this
 * is the case with OMAP4 for instance). In that case, the host will
 * overwrite pa with the dynamically allocated physical address.
 * Generally we don't want to expose physical addresses if we don't have to
 * (remote processors are generally _not_ trusted), so we might want to
 * change this to happen _only_ when explicitly required by the hardware.
 *
 */
struct fw_rsc_carveout {
	/**
	 * resource type
	 * @see fw_rsc_hdr
	 */
	u32 type;
	/**
	 * device address
	 */
	u32 da;
	/**
	 * physical address
	 */
	u32 pa;
	/**
	 * length (in bytes)
	 */
	u32 len;
	/**
	 * iommu protection flags
	 *
	 * is used to provide IOMMU protection flags
	 */
	u32 flags;
	/**
	 * reserved (must be zero)
	 */
	u32 reserved;
	/**
	 * human-readable name of the requested memory region
	 *
	 * should (optionally) contain a human readable name of this carveout region
	 * (mainly for debugging purposes).
	 */
	u8 name[32];
} __packed;

/**
 * struct fw_rsc_devmem - iommu mapping request
 *
 * This resource entry requests the host to iommu map a physically contiguous
 * memory region. This is needed in case the remote processor requires
 * access to certain memory-based peripherals; _never_ use it to access
 * regular memory.
 *
 * This is obviously only needed if the remote processor is accessing memory
 * via an iommu.
 *
 *
 * Note: at this point we just "trust" those devmem entries to contain valid
 * physical addresses, but this isn't safe and will be changed: eventually we
 * want remoteproc implementations to provide us ranges of physical addresses
 * the firmware is allowed to request, and not allow firmwares to request
 * access to physical addresses that are outside those ranges.
 */
struct fw_rsc_devmem {
	/**
	 * resource type
	 * @see fw_rsc_hdr
	 */
	u32 type;
	/**
	 * device address
	 *
	 * should specify the required device address, 
	 */
	u32 da;
	/**
	 * physical address
	 *
 	 * should specify the physical address we want to map.
	 */
	u32 pa;
	/**
	 * length (in bytes)
	 * should specify the size of the mapping and 
	 */
	u32 len;
	/**
	 * iommu protection flags
 	 * 
	 * is the IOMMU protection flags. As always, 
	 */
	u32 flags;
	/**
	 * reserved (must be zero)
	 */
	u32 reserved;
	/**
	 * human-readable name of the requested region to be mapped
	 *
	 * may (optionally) contain a human readable name of this mapping (mainly for debugging purposes).
	 */
	u8 name[32];
} __packed;

/**
 * struct fw_rsc_trace - trace buffer declaration
 *
 * This resource entry provides the host information about a trace buffer
 * into which the remote processor will write log messages.
 *
 * After booting the remote processor, the trace buffers are exposed to the
 * user via debugfs entries (called trace0, trace1, etc..).
 */
struct fw_rsc_trace {
	/**
	 * resource type
	 * @see fw_rsc_hdr
	 */
	u32 type;
	/**
	 * device address
	 *
 	 * specifies the device address of the buffer.
	 */
	u32 da;
	/**
	 * length (in bytes)
	 *
	 * specifies its size, and @name may contain a human readable name of the trace buffer.
	 */
	u32 len;
	/**
	 * reserved (must be zero)
	 */
	u32 reserved;
	/**
	 * human-readable name of the trace buffer
	 */
	u8 name[32];
} __packed;

/**
 * struct fw_rsc_vdev_vring - vring descriptor entry
 *
 * This descriptor is not a resource entry by itself; it is part of the
 * vdev resource type (see below).
 */
struct fw_rsc_vdev_vring {
	/**
	 * device address
	 *
	 * \note that da should either contain the device address where
	 * the remote processor is expecting the vring, or indicate that
	 * dynamically allocation of the vring's device address is supported.
	 */
	u32 da;
	/**
	 * the alignment between the consumer and producer parts of the vring
	 */
	u32 align;
	/**
	 * num of buffers supported by this vring (must be power of two)
	 */
	u32 num;
	/**
	 * is a unique rproc-wide notify index for this vring. This notify
	 * index is used when kicking a remote processor, to let it know that this
	 * vring is triggered.
	 */
	u32 notifyid;
	/**
	 * reserved (must be zero)
	 */
	u32 reserved;
} __packed;

/**
 * struct fw_rsc_vdev - virtio device header
 *
 * This resource is a virtio device header: it provides information about
 * the vdev, and is then used by the host and its peer remote processors
 * to negotiate and share certain virtio properties.
 *
 * By providing this resource entry, the firmware essentially asks remoteproc
 * to statically allocate a vdev upon registration of the rproc (dynamic vdev
 * allocation is not yet supported).
 *
 * Note: unlike virtualization systems, the term 'host' here means
 * the Linux side which is running remoteproc to control the remote
 * processors. We use the name 'gfeatures' to comply with virtio's terms,
 * though there isn't really any virtualized guest OS here: it's the host
 * which is responsible for negotiating the final features.
 * Yeah, it's a bit confusing.
 *
 * Note: immediately following this structure is the virtio config space for
 * this vdev (which is specific to the vdev; for more info, read the virtio
 * spec). the size of the config space is specified by config_len.
 */
struct fw_rsc_vdev {
	/**
	 * resource type
	 * @see fw_rsc_hdr
	 */
	u32 type;
	/**
	 * virtio device id (as in virtio_ids.h)
	 */
	u32 id;
	/**
	 * is a unique rproc-wide notify index for this vdev. This notify
	 * index is used when kicking a remote processor, to let it know that the
	 * status/features of this vdev have changes.
	 */
	u32 notifyid;
	/**
	 * specifies the virtio device features supported by the firmware
	 */
	u32 dfeatures;
	/**
	 * is a place holder used by the host to write back the
	 * negotiated features that are supported by both sides.
	 */
	u32 gfeatures;
	/**
	 * is the size of the virtio config space of this vdev. The config
	 * space lies in the resource table immediate after this vdev header.
	 */
	u32 config_len;
	/**
	 * is a place holder where the host will indicate its virtio progress.
	 */
	u8 status;
	/**
	 * indicates how many vrings are described in this vdev header
	 */
	u8 num_of_vrings;
	/**
	 * reserved (must be zero)
	 */
	u8 reserved[2];
	/**
	 * is an array of num_of_vrings entries of fw_rsc_vdev_vring.
	 */
	struct fw_rsc_vdev_vring vring[0];
} __packed;

struct rproc;
struct rproc_ops {
	void *(*init)(struct rproc *rpoc, void *initData, uint32_t cpuID);
	int32_t (*deinit)(struct rproc *rproc, void *data);
	int32_t (*notify)(struct rproc *rproc, void *data, uint32_t virtID);
};
/**
 * Remoteproc Init
 * \return 0 on ok -1 on error
 */
struct rproc *rproc_init(const struct rproc_ops *ops, void *initData, struct resource_table *rsc, uint32_t cpuID, bool master);
/**
 * Remoteproc deinit
 * \return 0 on ok -1 on error
 */
int32_t rproc_deinit(struct rproc *rproc);
int32_t rproc_isr(struct rproc *rproc, uint32_t virtID);
int32_t rproc_notify(struct rproc *rproc, uint32_t virtID);
/**\}*/

#endif
