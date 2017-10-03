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
#ifndef SD_H_
#define SD_H_
#include <FreeRTOS.h>
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <semphr.h>
#include <hal.h>
/**
 * \defgroup sd_driver SD Card Controller Interface
 * \ingroup HAL
 * \code
 * #include <sd.h>
 * \endcode
 * \{
 */
/**
 * Error in the sequence of the authentication process
 */
#define SD_ERROR_AKE_SEQ_ERROR -2
/**
 * A general or an unknown error occurred during the operation.
 */
#define SD_ERROR_ERROR -3
/**
 * Internal card controller error
 */
#define SD_ERROR_CC_ERROR -4
/**
 * Card internal ECC was applied but failed to correct the data.
 */
#define SD_ERROR_CARD_ECC_FAIELD -5
/**
 * Command not legal for the card state
 */
#define SD_ERROR_ILLEGAL_COMMAND -6
/**
 * The CRC check of the previous command failed.
 */
#define SD_ERROR_COM_CRC_ERROR -7
/**
 * Set when a sequence or password error has been detected in lock/unlock card command.
 */
#define SD_ERROR_LOCK_UNLOCK_FAILED -8
/**
 * When set, signals that the card is locked by the host
 */
#define SD_ERROR_CARD_IS_LOCKED -9
/**
 * Set when the host attempts to write to a protected block or to the temporary or permanent writeprotected card.
 */
#define SD_ERROR_WP_VIOLATION -10
/**
 *An invalid selection of write-blocks for erase occurred.
 */
#define SD_ERROR_ERASE_PARAM -11
/**
 * An error in the sequence of erase commands occurred.
 */
#define SD_ERROR_ERASE_SEQ_ERROR -12
/**
 * The transferred block length is not allowed for this card, or the number of transferred bytes does not
 * match the block length.
 */
#define SD_ERROR_ERASE_LEN_ERROR -13
/**
 * A misaligned address which did not match the block length was used in the command.
 */
#define SD_ERROR_ADDRESS_ERROR -14
/**
 * The command's argument was out of the allowed range for this card.
 */
#define SD_ERROR_OUT_OF_RANGE -15
/** 
 * An invalid function number was requested
 */
#define SD_ERROR_FUNCTION_NUMBER -16
/**
 * Define a CMD
 * \param x CMD ID
 * \return command
 */
#define CMD(x) (x)
/**
 * ACMD 
 * CMD(55) shall send before!
 * \param x CMD ID
 * \return ACMD
 */
#define ACMD(x) (x)
/**
 * Handle of a Instants of a SD Contoller
 */
struct sd;
#ifdef CONFIG_SD_MULTI
/**
 * Function of a driver in Multi Driver implementation mode 
 */
struct sd_ops {
	struct sd *(*sd_init)(uint32_t index, struct sd_setting *settings);
	int32_t (*sd_deinit)(struct sd *sd);
	int32_t (*sd_setBlockSize)(struct sd *sd, enum sd_block_size blockSize);
	int32_t (*sd_setBusWide)(struct sd *sd, enum sd_bus_wide buswide);
	int32_t (*sd_setClock)(struct sd *sd, uint64_t clock);
	int32_t (*sd_sendCommand)(struct sd *sd, uint32_t command, uint32_t argument, struct sd_response *response, TickType_t waittime);
	int32_t (*sd_write)(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data, TickType_t waittime);
	int32_t (*sd_read)(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data, TickType_t waittime);
	int32_t (*sd_sendCommandISR)(struct sd *sd, uint32_t command, uint32_t argument, struct sd_response *response);
	int32_t (*sd_writeISR)(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data);
	int32_t (*sd_readISR)(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data);
};
#endif
/**
 * Mode
 */
enum sd_mode {
	/**
	 * Contoller is in SPI Mode
	 */
	SD_SPI,
	/**
	 * Contoller is in SD Mode
	 */
	SD_SD,
	/**
	 * Contoller is in SDIO Mode
	 */
	SD_SDIO
};
/**
 * All driver shall implement this Struct
 */
struct sd_generic {
	/**
	 * true = is init
	 * false = is not init
	 */
	bool init;
#ifdef CONFIG_INSTANCE_NAME
	/**
	 * Name of Driver Instance for Debugging 
	 */
	const char *name;
#endif
#ifdef CONFIG_SD_THREAD_SAVE
	/**
	 * Mutex
	 */
	OS_DEFINE_MUTEX_RECURSIVE(lock);
#endif
#ifdef CONFIG_SD_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct sd_ops *ops;
#endif
	/**
	 * SD Mode
	 */
	enum sd_mode mode;
	/**
	 * True if last Command is CMD55
	 */
	bool selectACMD;
};
enum sd_block_size {
	SD_BLOCK_SIZE_1B,
	SD_BLOCK_SIZE_2B,
	SD_BLOCK_SIZE_4B,
	SD_BLOCK_SIZE_8B,
	SD_BLOCK_SIZE_16B,
	SD_BLOCK_SIZE_32B,
	SD_BLOCK_SIZE_64B,
	SD_BLOCK_SIZE_128B,
	SD_BLOCK_SIZE_256B,
	SD_BLOCK_SIZE_512B,
	SD_BLOCK_SIZE_1024B,
	SD_BLOCK_SIZE_2048B,
};
/**
 * SD Response Formart
 */
struct sd_response {
	/**
	 * CRC and Index checkt by Driver
	 * data[0] = Bit(127:96)
	 * data[1] = Bit(95:64)
	 * data[2] = Bit(63:32)
	 * data[3] = Bit(31:0)
	 */
	uint32_t data[4];
};
enum sd_bus_wide {
	SD_BusWide_1b,
	SD_BusWide_4b,
	SD_BusWide_8b,
};
/**
 * SD Settings
 */
struct sd_setting {
	/**
	 * Clock Speed
	 */
	uint64_t clock;
	/**
	 * Bus Wide
	 */
	enum sd_bus_wide wide;
	/**
	 * Mode
	 */
	enum sd_mode mode;
};
#ifndef CONFIG_SD_MULTI
/**
 * Init Function
 * \param index in sds Array
 * \param settings Settings
 * \return Example Instants or NULL
 */
struct sd *sd_init(uint32_t index, struct sd_setting *settings);
/**
 * Deinit Driver Instants 
 * \param sd Instant
 * \return 0 on ok -1 on failure
 */
int32_t sd_deinit(struct sd *sd);
/**
 * Set Block Size
 * \param sd Instants
 * \param blockSize Block Size
 * \return 0 on ok -1 on failure
 */
int32_t sd_setBlockSize(struct sd *sd, enum sd_block_size blockSize);
/**
 * Set Bus Wide
 * \param sd Instants
 * \param busWide Bus Wide
 * \return 0 on ok -1 on failure
 */
int32_t sd_setBusWide(struct sd *sd, enum sd_bus_wide busWide);
/**
 * Set Clock
 * \param sd Instants
 * \param clock Clock Speed in Hz
 * \return 0 on ok -1 on failure
 */
int32_t sd_setClock(struct sd *sd, uint64_t clock);
/**
 * Send Command
 * \param sd Instants
 * \param command Command
 * \param argument Command Argument without CMD ID and CRC. CRC is Calculated by Driver or Hardware
 * \param response Card Response NULL on not needed
 * \param waittime Waittime
 * \return < 0 on error 0 on ok 
 *  -1 == Unkown Error 
 * 
 * Is Response == R2, R3, R4 and R7 return is always 0  
 * 
 * Is Response == R1 following error can occur (Desciption in Standart s. Card Status) 
 * - {@link SD_ERROR_AKE_SEQ_ERROR} -2 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_CC_ERROR} -4 
 * - {@link SD_ERROR_CARD_ECC_FAIELD} -5 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7 
 * - {@link SD_ERROR_LOCK_UNLOCK_FAILED} -8 
 * - {@link SD_ERROR_CARD_IS_LOCKED} -9 
 * - {@link SD_ERROR_WP_VIOLATION} -10 
 * - {@link SD_ERROR_ERASE_PARAM} -11 
 * - {@link SD_ERROR_ERASE_SEQ_ERROR} -12 
 * - {@link SD_ERROR_ERASE_LEN_ERROR} -13 
 * - {@link SD_ERROR_ADDRESS_ERROR} -14 
 * - {@link SD_ERROR_OUT_OF_RANGE} -15  
 * 
 * Is Response == R5 following error can occur (Desciption in Standart s. SDIO R5) 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7 
 * - {@link SD_ERROR_OUT_OF_RANGE} -15 
 * - {@link SD_ERROR_FUNCTION_NUMBER} -16  
 * 
 * Is Response == R6 following error can occur (Desciption in Standart Response R6) 
 * - {@link SD_ERROR_AKE_SEQ_ERROR} -2 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7
 */
int32_t sd_sendCommand(struct sd *sd, uint32_t command, uint32_t argument, struct sd_response *response, TickType_t waittime);
/**
 * Wirte Data to SD
 * \param sd Instants
 * \param command Command
 * \param argument Command Argument without CMD ID and CRC. CRC is Calculated by Driver or Hardware
 * \param size Buffer Size
 * \param data Buffer Pointer
 * \param waittime Waittime
 * \return < 0 on error 0 on ok 
 *  -1 == Unkown Error 
 * 
 * Is Response == R2, R3, R4 and R7 return is always 0 
 * 
 * Is Response == R1 following error can occur (Desciption in Standart s. Card Status) 
 * - {@link SD_ERROR_AKE_SEQ_ERROR} -2 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_CC_ERROR} -4 
 * - {@link SD_ERROR_CARD_ECC_FAIELD} -5 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7 
 * - {@link SD_ERROR_LOCK_UNLOCK_FAILED} -8 
 * - {@link SD_ERROR_CARD_IS_LOCKED} -9 
 * - {@link SD_ERROR_WP_VIOLATION} -10 
 * - {@link SD_ERROR_ERASE_PARAM} -11 
 * - {@link SD_ERROR_ERASE_SEQ_ERROR} -12 
 * - {@link SD_ERROR_ERASE_LEN_ERROR} -13 
 * - {@link SD_ERROR_ADDRESS_ERROR} -14 
 * - {@link SD_ERROR_OUT_OF_RANGE} -15 
 * 
 * Is Response == R5 following error can occur (Desciption in Standart s. SDIO R5) 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7 
 * - {@link SD_ERROR_OUT_OF_RANGE} -15 
 * - {@link SD_ERROR_FUNCTION_NUMBER} -16 
 * 
 * Is Response == R6 following error can occur (Desciption in Standart Response R6) 
 * - {@link SD_ERROR_AKE_SEQ_ERROR} -2 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7 
 */
int32_t sd_write(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data, TickType_t waittime);
/**
 * Read Data to SD
 * \param sd Instants
 * \param command Command
 * \param argument Command Argument without CMD ID and CRC. CRC is Calculated by Driver or Hardware
 * \param size Buffer Size
 * \param data Buffer Pointer
 * \param waittime Waittime
 * \return < 0 on error 0 on ok
 *  -1 == Unkown Error
 *
 * Is Response == R2, R3, R4 and R7 return is always 0 
 *
 * Is Response == R1 following error can occur (Desciption in Standart s. Card Status) 
 * - {@link SD_ERROR_AKE_SEQ_ERROR} -2 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_CC_ERROR} -4 
 * - {@link SD_ERROR_CARD_ECC_FAIELD} -5 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7 
 * - {@link SD_ERROR_LOCK_UNLOCK_FAILED} -8 
 * - {@link SD_ERROR_CARD_IS_LOCKED} -9 
 * - {@link SD_ERROR_WP_VIOLATION} -10 
 * - {@link SD_ERROR_ERASE_PARAM} -11 
 * - {@link SD_ERROR_ERASE_SEQ_ERROR} -12 
 * - {@link SD_ERROR_ERASE_LEN_ERROR} -13 
 * - {@link SD_ERROR_ADDRESS_ERROR} -14 
 * - {@link SD_ERROR_OUT_OF_RANGE} -15 
 * 
 * Is Response == R5 following error can occur (Desciption in Standart s. SDIO R5) 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7 
 * - {@link SD_ERROR_OUT_OF_RANGE} -15 
 * - {@link SD_ERROR_FUNCTION_NUMBER} -16 
 * 
 * Is Response == R6 following error can occur (Desciption in Standart Response R6) 
 * - {@link SD_ERROR_AKE_SEQ_ERROR} -2 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7
 */
int32_t sd_read(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data, TickType_t waittime);
/**
 * Send Command
 * \param sd Instants
 * \param command Command
 * \param argument Command Argument without CMD ID and CRC. CRC is Calculated by Driver or Hardware
 * \param response Card Response
 * \return < 0 on error 0 on ok
 *  -1 == Unkown Error
 *
 * Is Response == R2, R3, R4 and R7 return is always 0 
 *
 * Is Response == R1 following error can occur (Desciption in Standart s. Card Status) 
 * - {@link SD_ERROR_AKE_SEQ_ERROR} -2 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_CC_ERROR} -4 
 * - {@link SD_ERROR_CARD_ECC_FAIELD} -5 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7 
 * - {@link SD_ERROR_LOCK_UNLOCK_FAILED} -8 
 * - {@link SD_ERROR_CARD_IS_LOCKED} -9 
 * - {@link SD_ERROR_WP_VIOLATION} -10 
 * - {@link SD_ERROR_ERASE_PARAM} -11 
 * - {@link SD_ERROR_ERASE_SEQ_ERROR} -12 
 * - {@link SD_ERROR_ERASE_LEN_ERROR} -13 
 * - {@link SD_ERROR_ADDRESS_ERROR} -14 
 * - {@link SD_ERROR_OUT_OF_RANGE} -15 
 *
 * Is Response == R5 following error can occur (Desciption in Standart s. SDIO R5) 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7 
 * - {@link SD_ERROR_OUT_OF_RANGE} -15 
 * - {@link SD_ERROR_FUNCTION_NUMBER} -16 
 * 
 * Is Response == R6 following error can occur (Desciption in Standart Response R6) 
 * - {@link SD_ERROR_AKE_SEQ_ERROR} -2 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7
 */
int32_t sd_sendCommandISR(struct sd *sd, uint32_t command, uint32_t argument, struct sd_response *response);
/**
 * Wirte Data to SD
 * \param sd Instants
 * \param command Command
 * \param argument Command Argument without CMD ID and CRC. CRC is Calculated by Driver or Hardware
 * \param size Buffer Size
 * \param data Buffer Pointer
 * \return < 0 on error 0 on ok 
 *  -1 == Unkown Error 
 *
 * Is Response == R2, R3, R4 and R7 return is always 0 
 *
 * Is Response == R1 following error can occur (Desciption in Standart s. Card Status) 
 * - {@link SD_ERROR_AKE_SEQ_ERROR} -2 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_CC_ERROR} -4 
 * - {@link SD_ERROR_CARD_ECC_FAIELD} -5 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7 
 * - {@link SD_ERROR_LOCK_UNLOCK_FAILED} -8 
 * - {@link SD_ERROR_CARD_IS_LOCKED} -9 
 * - {@link SD_ERROR_WP_VIOLATION} -10 
 * - {@link SD_ERROR_ERASE_PARAM} -11 
 * - {@link SD_ERROR_ERASE_SEQ_ERROR} -12 
 * - {@link SD_ERROR_ERASE_LEN_ERROR} -13 
 * - {@link SD_ERROR_ADDRESS_ERROR} -14 
 * - {@link SD_ERROR_OUT_OF_RANGE} -15 
 *
 * Is Response == R5 following error can occur (Desciption in Standart s. SDIO R5) 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7 
 * - {@link SD_ERROR_OUT_OF_RANGE} -15 
 * - {@link SD_ERROR_FUNCTION_NUMBER} -16 
 *
 * Is Response == R6 following error can occur (Desciption in Standart Response R6) 
 * - {@link SD_ERROR_AKE_SEQ_ERROR} -2 
 * - {@link SD_ERROR_ERROR} -3 
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6 
 * - {@link SD_ERROR_COM_CRC_ERROR} -7
 */
int32_t sd_writeISR(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data);
/**
 * Read Data to SD
 * \param sd Instants
 * \param command Command
 * \param argument Command Argument without CMD ID and CRC. CRC is Calculated by Driver or Hardware
 * \param size Buffer Size
 * \param data Buffer Pointer
 * \return < 0 on error 0 on ok
 *  -1 == Unkown Error
 * 
 * Is Response == R2, R3, R4 and R7 return is always 0
 * 
 * Is Response == R1 following error can occur (Desciption in Standart s. Card Status)
 * 
 * - {@link SD_ERROR_AKE_SEQ_ERROR} -2
 * - {@link SD_ERROR_ERROR} -3
 * - {@link SD_ERROR_CC_ERROR} -4
 * - {@link SD_ERROR_CARD_ECC_FAIELD} -5
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6
 * - {@link SD_ERROR_COM_CRC_ERROR} -7
 * - {@link SD_ERROR_LOCK_UNLOCK_FAILED} -8
 * - {@link SD_ERROR_CARD_IS_LOCKED} -9
 * - {@link SD_ERROR_WP_VIOLATION} -10
 * - {@link SD_ERROR_ERASE_PARAM} -11
 * - {@link SD_ERROR_ERASE_SEQ_ERROR} -12
 * - {@link SD_ERROR_ERASE_LEN_ERROR} -13
 * - {@link SD_ERROR_ADDRESS_ERROR} -14
 * - {@link SD_ERROR_OUT_OF_RANGE} -15
 * 
 * Is Response == R5 following error can occur (Desciption in Standart s. SDIO R5)
 * - {@link SD_ERROR_ERROR} -3
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6
 * - {@link SD_ERROR_COM_CRC_ERROR} -7
 * - {@link SD_ERROR_OUT_OF_RANGE} -15
 * - {@link SD_ERROR_FUNCTION_NUMBER} -16
 * 
 * Is Response == R6 following error can occur (Desciption in Standart Response R6)
 * - {@link SD_ERROR_AKE_SEQ_ERROR} -2
 * - {@link SD_ERROR_ERROR} -3
 * - {@link SD_ERROR_ILLEGAL_COMMAND} -6
 * - {@link SD_ERROR_COM_CRC_ERROR} -7
 */
int32_t sd_readISR(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data);
#else
inline struct sd *sd_init(uint32_t index, struct sd_setting *settings) {
	HAL_DEFINE_GLOBAL_ARRAY(sd);
	struct sd_generic *p = (struct sd_generic *) HAL_GET_DEV(sd, index);
	if (p == NULL) {
		return NULL;
	}
	return p->ops->sd_init(index, settings);
}
inline int32_t sd_deinit(struct sd *sd) {
	struct sd_generic *p = (struct sd_generic *) sd;
	return p->ops->sd_deinit(sd);
}
int32_t sd_setBlockSize(struct sd *sd, enum sd_block_size blockSize) {
	struct sd_generic *p = (struct sd_generic *) sd;
	return p->ops->sd_setBlockSize(sd, blockSize);
}
int32_t sd_setBusWide(struct sd *sd, enum sd_bus_wide busWide) {
	struct sd_generic *p = (struct sd_generic *) sd;
	return p->ops->sd_setBusWide(sd, busWide);
}
int32_t sd_setClock(struct sd *sd, uint64_t clock) {
	struct sd_generic *p = (struct sd_generic *) sd;
	return p->ops->sd_setClock(sd, clock);
}
inline int32_t sd_write(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data, TickType_t waittime) {
	struct sd_generic *p = (struct sd_generic *) sd;
	return p->ops->sd_write(sd, command, argument, size, data, waittime);
}
inline int32_t sd_read(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data, TickType_t waittime) {
	struct sd_generic *p = (struct sd_generic *) sd;
	return p->ops->sd_read(sd, command, argument, size, data, waittime);
}
inline int32_t sd_writeISR(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data) {
	struct sd_generic *p = (struct sd_generic *) sd;
	return p->ops->sd_write(sd, command, argument, size, data);
}
inline int32_t sd_readISR(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data) {
	struct sd_generic *p = (struct sd_generic *) sd;
	return p->ops->sd_read(sd, command, argument, size, data);
}
#endif
/**\}*/
#endif
