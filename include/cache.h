#ifndef CACHE_H_
#define CACHE_H_
#include <stdint.h>

#define CCR_ENABLE_CACHE BIT(0)
#define CCR_ENABLE_WRITE_BUFFER BIT(1)
#define CCR_FORCE_WRITE_THROUGH BIT(2)
#define CCR_INVALID_WAY_0 BIT(24)
#define CCR_PUSH_WAY_0 BIT(25)
#define CCR_INVALID_WAY_1 BIT(26)
#define CCR_PUSH_WAY_1 BIT(27)
#define CCR_GO BIT(31)

#define CLCR_LGO BIT(0)
#define CLCR_ADDR(x) ((x & 0x3FF) << 2)
#define CLCR_WAY_SEL(x) ((x & 0x1) << 14)
#define CLCR_WAY_0 CLCR_WAY_SEL(0)
#define CLCR_WAY_1 CLCR_WAY_SEL(1)
#define CLCR_TAG_DATA_SEL(x) ((x & 0x1) << 16)
#define CLCR_TAG_SEL CLCR_TAG_DATA_SEL(1)
#define CLCR_DATA_SEL CLCR_TAG_DATA_SEL(0)
#define CLCR_IS_VALID(x) ((x >> 20) & 0x1)
#define CLCR_IS_MODIFIED ((x >> 21) & 0x1)
#define CLCR_GET_WAY ((x >> 22) & 0x1)
#define CLCR_LINE_CMD(x) ((x & 0x3) << 24)
#define CLCR_CMD_SEARCH CLCR_LINE_CMD(0)
#define CLCR_CMD_INVAID CLCR_LINE_CMD(1)
#define CLCR_CMD_PUSH CLCR_LINE_CMD(2)
#define CLCR_CMD_CLEAR CLCR_LINE_CMD(3)
#define CLCR_LINE_ADDRESS_SEL(x) ((x & 0x1) << 26)
#define CLCR_CACHE_ADDRESS CLCR_LINE_ADDRESS_SEL(0)
#define CLCR_PYS_ADDRESS CLCR_LINE_ADDRESS_SEL(1)
#define CLCR_LINE_TYPE(x) ((x & 0x1) << 27)
#define CLCR_READ CLCR_LINE_TYPE(0)
#define CLCR_WRITE CLCR_LINE_TYPE(1)

#define CSAR_LGO BIT(0)
#define CSAR_IS_LGO(x) ((x >> 0) & 0x1)
#define CSAR_PYS_ADDRESS(x) ((((uint32_t) x) & 0x1FFFFFFFU) << 2)

int32_t cache_init();
int32_t cache_flushDataAll();
int32_t cache_flushData(uint32_t *addr, uint32_t size);
int32_t cache_invalidDataAll();
int32_t cache_invalidData(uint32_t *addr, uint32_t size);
#endif
