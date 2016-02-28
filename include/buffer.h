#ifndef SHARED_H_
#define SHARED_H_
#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
/**
 * \defgroup BUFFER Driver for Shared Memmory for heterogeneous Multicore Possessor
 * \ingroup HAL
 * \code
 * #include <buffer.h>
 * \endcode
 * 
 * Shared Memmory driver for heterogeneous Multicore Possessor
 * \{
 */
/**
 * Magic Number used to detect memory is already initialized    
 */
#define SHARED_MAGIC 0x42424343
/**
 * Interface shared between Processors
 */
struct buffer_base;
/**
 * Private Structure of Buffer driver
 */
struct buffer;
/**
 * Error Code return if Operation not Supported 
 */
#define BUFFER_OPERATION_NOT_SUPPORTED -1
/**
 * Error Code return if no space Left on BUffer
 */
#define BUFFER_NO_SPACE_LEFT -2
/**
 * Error Code return if Argument not valid 
 */
#define BUFFER_EINVAL -3 
/**
 * Init Buffer Instance 
 * \param base Base Pointer in SRAM
 * \param len Length of Buffer shall Power of 2
 * \param sizeOfEntry Size of one Entry in Buffer shall Power of 2
 * \param readOnly one Buffer Instance is unidirectional read only or write only
 * \param irqnr IRQ Nummber
 * \return Buffer Instance or NULL on error
 */
struct buffer *buffer_init(struct buffer_base *base, uint32_t len, uint32_t sizeOfEntry, bool readOnly, uint32_t irqnr);
/**
 * Deinit Buffer Instance
 * \param buffer Buffer Instance 
 * \return -1 on error 0 on ok
 */
int32_t buffer_deinit(struct buffer *buffer);
/**
 * Check Buffer is full
 * \param buffer Buffer Instance 
 * \return Return 0 on is full else space left on buffer
 */
int32_t buffer_is_full(struct buffer *buffer);
/**
 * Check Buffer is empty 
 * \param buffer Buffer Instance 
 * \return true = buffer empty false = buffer not empty
 */
bool buffer_empty(struct buffer *buffer);
/**
 * Write one entry from buffer
 * \param buffer Buffer Instance 
 * \param data Data to write
 * \param size Size of writing data in entrys! Not in bytes
 * \return < 0 on error >= 0 size write to buffer
 */
int32_t buffer_write(struct buffer *buffer, uint8_t *data, int32_t size);
/**
 * read one entry from buffer. This Function blocks until data is in Buffer. The function read until buffer is empty or user pointer is full. If waittime == 0 function is unblocking!
 * \param buffer Buffer Instance
 * \param data Data to write
 * \param size Size of writing data in entrys! Not in bytes
 * \param waittime max waittime in ISR lock see xSemaphoreTake()
 * \return < 0 on error >= 0 size read from buffer
 */
int32_t buffer_read(struct buffer *buffer, uint8_t *data, int32_t size, TickType_t waittime);
/**\}*/
#endif
