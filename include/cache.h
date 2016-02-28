#ifndef CACHE_H_
#define CACHE_H_
#include <stdint.h>
/**
 * \defgroup CACHE Cache Subsystem
 * \ingroup HAL
 * \code
 * #include <cache.h>
 * \endcode
 * 
 * This is the Cache Subsystem.
 * \{
 */
/**
 * Cache Init 
 * \return -1 on error 0 on ok
 */
int32_t cache_init();
/**
 * Flash All data in Data Cache
 * \return -1 on error 0 on ok
 */
int32_t cache_flushDataAll();
/**
 * Flush Data 
 * \param addr Address
 * \param size Size
 * \return -1 on error 0 on ok
 */
int32_t cache_flushData(uint32_t *addr, uint32_t size);
/**
 * INvalid all data in Data Cache
 * \return -1 on error 0 on ok
 */
int32_t cache_invalidDataAll();
/**
 * Invalid Data 
 * \param addr Address
 * \param size Size
 * \return -1 on error 0 on ok
 */
int32_t cache_invalidData(uint32_t *addr, uint32_t size);
/**\}*/
#endif
