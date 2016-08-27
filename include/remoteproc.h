#ifndef REMOTEPROC_H_
#define REMOTEPROC_H_
#include <stdint.h>
#include <system.h>
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
#define __packed PACKED
/**
 * Remoteproc based on Linux Kernel Documention
 */
/**
 * struct resource_table - firmware resource table header
 * @ver: version number
 * @num: number of resource entries
 * @reserved: reserved (must be zero)
 * @offset: array of offsets pointing at the various resource entries
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
	u32 ver;
	u32 num;
	u32 reserved[2];
} __packed;

/**
 * struct fw_rsc_hdr - firmware resource entry header
 * @type: resource type
 * @data: resource data
 *
 * Every resource entry begins with a 'struct fw_rsc_hdr' header providing
 * its @type. The content of the entry itself will immediately follow
 * this header, and it should be parsed according to the resource type.
 */
struct fw_rsc_hdr {
	u32 type;
} __packed;

/**
 * enum fw_resource_type - types of resource entries
 *
 * @RSC_CARVEOUT:   request for allocation of a physically contiguous
 *		    memory region.
 * @RSC_DEVMEM:     request to iommu_map a memory-based peripheral.
 * @RSC_TRACE:	    announces the availability of a trace buffer into which
 *		    the remote processor will be writing logs.
 * @RSC_VDEV:       declare support for a virtio device, and serve as its
 *		    virtio header.
 * @RSC_LAST:       just keep this one at the end
 *
 * For more details regarding a specific resource type, please see its
 * dedicated structure below.
 *
 * Please note that these values are used as indices to the rproc_handle_rsc
 * lookup table, so please keep them sane. Moreover, @RSC_LAST is used to
 * check the validity of an index before the lookup table is accessed, so
 * please update it as needed.
 */
enum fw_resource_type {
	RSC_CARVEOUT	= 0,
	RSC_DEVMEM	= 1,
	RSC_TRACE	= 2,
	RSC_VDEV	= 3,
	RSC_LAST	= 4,
};

#define FW_RSC_ADDR_ANY (0xFFFFFFFFFFFFFFFF)

/**
 * struct fw_rsc_carveout - physically contiguous memory request
 * @da: device address
 * @pa: physical address
 * @len: length (in bytes)
 * @flags: iommu protection flags
 * @reserved: reserved (must be zero)
 * @name: human-readable name of the requested memory region
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
 * needed to map it (in case @rproc is using an IOMMU). Reducing the TLB
 * pressure is important; it may have a substantial impact on performance.
 *
 * If the firmware is compiled with static addresses, then @da should specify
 * the expected device address of this memory region. If @da is set to
 * FW_RSC_ADDR_ANY, then the host will dynamically allocate it, and then
 * overwrite @da with the dynamically allocated address.
 *
 * We will always use @da to negotiate the device addresses, even if it
 * isn't using an iommu. In that case, though, it will obviously contain
 * physical addresses.
 *
 * Some remote processors needs to know the allocated physical address
 * even if they do use an iommu. This is needed, e.g., if they control
 * hardware accelerators which access the physical memory directly (this
 * is the case with OMAP4 for instance). In that case, the host will
 * overwrite @pa with the dynamically allocated physical address.
 * Generally we don't want to expose physical addresses if we don't have to
 * (remote processors are generally _not_ trusted), so we might want to
 * change this to happen _only_ when explicitly required by the hardware.
 *
 * @flags is used to provide IOMMU protection flags, and @name should
 * (optionally) contain a human readable name of this carveout region
 * (mainly for debugging purposes).
 */
struct fw_rsc_carveout {
	u32 da;
	u32 pa;
	u32 len;
	u32 flags;
	u32 reserved;
	u8 name[32];
} __packed;

/**
 * struct fw_rsc_devmem - iommu mapping request
 * @da: device address
 * @pa: physical address
 * @len: length (in bytes)
 * @flags: iommu protection flags
 * @reserved: reserved (must be zero)
 * @name: human-readable name of the requested region to be mapped
 *
 * This resource entry requests the host to iommu map a physically contiguous
 * memory region. This is needed in case the remote processor requires
 * access to certain memory-based peripherals; _never_ use it to access
 * regular memory.
 *
 * This is obviously only needed if the remote processor is accessing memory
 * via an iommu.
 *
 * @da should specify the required device address, @pa should specify
 * the physical address we want to map, @len should specify the size of
 * the mapping and @flags is the IOMMU protection flags. As always, @name may
 * (optionally) contain a human readable name of this mapping (mainly for
 * debugging purposes).
 *
 * Note: at this point we just "trust" those devmem entries to contain valid
 * physical addresses, but this isn't safe and will be changed: eventually we
 * want remoteproc implementations to provide us ranges of physical addresses
 * the firmware is allowed to request, and not allow firmwares to request
 * access to physical addresses that are outside those ranges.
 */
struct fw_rsc_devmem {
	u32 da;
	u32 pa;
	u32 len;
	u32 flags;
	u32 reserved;
	u8 name[32];
} __packed;

/**
 * struct fw_rsc_trace - trace buffer declaration
 * @da: device address
 * @len: length (in bytes)
 * @reserved: reserved (must be zero)
 * @name: human-readable name of the trace buffer
 *
 * This resource entry provides the host information about a trace buffer
 * into which the remote processor will write log messages.
 *
 * @da specifies the device address of the buffer, @len specifies
 * its size, and @name may contain a human readable name of the trace buffer.
 *
 * After booting the remote processor, the trace buffers are exposed to the
 * user via debugfs entries (called trace0, trace1, etc..).
 */
struct fw_rsc_trace {
	u32 da;
	u32 len;
	u32 reserved;
	u8 name[32];
} __packed;
#endif
