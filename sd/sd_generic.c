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
#include <sd.h>
#define SD_PRV
#include <sd_prv.h>

int32_t sd_genericInit(struct sd *t, struct sd_setting *settings) {
	struct sd_generic *sd = (struct sd_generic *) t;
	if (hal_isInit(sd)) {
		return SD_ALREDY_INITED;
	}
#ifdef CONFIG_SD_THREAD_SAVE
	{
		int32_t ret = hal_init(sd);
		if (ret < 0) {
			goto sd_generic_init_error0;
		}
	}
#endif
	sd->init = true;
	sd->mode = settings->mode;
	return 0;
#ifdef CONFIG_SD_THREAD_SAVE
sd_generic_init_error0:
	return -1;
#endif

}
#define CMD(x) x
#define R1_SIZE SD_SHORT
#define R1B_SIZE SD_SHORT
#define R2_SIZE SD_LONG
#define R3_SIZE SD_SHORT
#define R4_SIZE SD_SHORT /* SDIO */
#define R5_SIZE SD_SHORT /* SDIO */
#define R6_SIZE SD_SHORT
#define R7_SIZE SD_SHORT

enum sd_responseLength sd_get_responseLength(struct sd *sd, uint32_t command, uint32_t argument) {
	/* TODO App Command !! */
	struct sd_generic *s = (struct sd_generic *) sd;
	if (s->selectACMD) {
		if (command == CMD(41)) {
			return R3_SIZE;
		}
		return R1_SIZE;
	}
	switch(command) {
		case CMD(11):
		case CMD(13):
		case CMD(16):
		case CMD(17):
		case CMD(18):
		case CMD(19):
		case CMD(23):
		case CMD(24):
		case CMD(25):
		case CMD(27):
		case CMD(30):
		case CMD(32):
		case CMD(33):
		case CMD(40):
		case CMD(42):
		case CMD(55):
		case CMD(56):
			return R1_SIZE;
		case CMD(7):
		case CMD(12):
		case CMD(20):
		case CMD(28):
		case CMD(29):
		case CMD(38):
			return R1B_SIZE;
		case CMD(2):
		case CMD(9):
		case CMD(10):
			return R2_SIZE;
		case CMD(5):
			return R4_SIZE;
		case CMD(52): /* SDIO */
		case CMD(53): /* SDIO */
			return R5_SIZE;
		case CMD(3):
			return R6_SIZE;
		case CMD(8):
			return R7_SIZE;
		default:
			return SD_NO_RESPONSE;
	}
}
#define CHECK_IS_BIT_SET(x, bit) (((x >> bit) & 0x1) == 0x1)
#define IS_AKE_SEQ_ERROR(x) CHECK_IS_BIT_SET(x, 3)
#define IS_ERROR(x) CHECK_IS_BIT_SET(x, 19)
#define IS_CC_ERROR(x) CHECK_IS_BIT_SET(x, 20)
#define IS_CARD_ECC_FAIELD(x) CHECK_IS_BIT_SET(x, 21)
#define IS_ILLEGAL_COMMAND(x) CHECK_IS_BIT_SET(x, 22)
#define IS_COM_CRC_ERROR(x) CHECK_IS_BIT_SET(x, 23)
#define IS_LOCK_UNLOCK_FAILED(x) CHECK_IS_BIT_SET(x, 24)
#define IS_CARD_IS_LOCKED(x) CHECK_IS_BIT_SET(x, 25)
#define IS_WP_VIOLATION(x) CHECK_IS_BIT_SET(x, 26)
#define IS_ERASE_PARAM(x) CHECK_IS_BIT_SET(x, 27)
#define IS_ERASE_SEQ_ERROR(x) CHECK_IS_BIT_SET(x, 28)
#define IS_ERASE_LEN_ERROR(x) CHECK_IS_BIT_SET(x, 29)
#define IS_ADDRESS_ERROR(x) CHECK_IS_BIT_SET(x, 30)
#define IS_OUT_OF_RANGE(x) CHECK_IS_BIT_SET(x, 31)
static int32_t sd_check_response_r1(struct sd *sd, uint32_t command, uint32_t argument, struct sd_response *response) {
	struct sd_generic *s = (struct sd_generic *) sd;
	uint32_t status = response->data[3];
	switch (s->mode) {
		case SD_SD:
			{
				if (IS_CC_ERROR(status)) {
					return SD_ERROR_CC_ERROR;
				}
				if (IS_CARD_ECC_FAIELD(status)) {
					return SD_ERROR_CARD_ECC_FAIELD;
				}
				if (IS_LOCK_UNLOCK_FAILED(status)) {
					return SD_ERROR_LOCK_UNLOCK_FAILED;
				}
				if (IS_CARD_IS_LOCKED(status)) {
					return SD_ERROR_CARD_IS_LOCKED;
				}
				if (IS_WP_VIOLATION(status)) {
					return SD_ERROR_WP_VIOLATION;
				}
				if (IS_ERASE_PARAM(status)) {
					return SD_ERROR_ERASE_PARAM;
				}
				if (IS_ERASE_SEQ_ERROR(status)) {
					return SD_ERROR_ERASE_SEQ_ERROR;
				}
				if (IS_ERASE_LEN_ERROR(status)) {
					return SD_ERROR_ERASE_LEN_ERROR;
				}
				if (IS_ADDRESS_ERROR(status)) {
					return SD_ERROR_ADDRESS_ERROR;
				}
			}
			/* SD also have following Errors */
		case SD_SDIO:
			{
				if (IS_ERROR(status)) {
					return SD_ERROR_ERROR;
				}
				if (IS_ILLEGAL_COMMAND(status)) {
					return SD_ERROR_ILLEGAL_COMMAND;
				}
				if (IS_COM_CRC_ERROR(status)) {
					return SD_ERROR_COM_CRC_ERROR;
				}
				if (IS_OUT_OF_RANGE(status)) {
					return SD_ERROR_OUT_OF_RANGE;
				}
			}
			break;
		case SD_SPI:
			return -1; /* TODO */
	}
	return 0;
}
static int32_t sd_check_response_r2(struct sd *sd, uint32_t command, uint32_t argument, struct sd_response *response)  {
	/* Noting to check */
	return 0;
}
static int32_t sd_check_response_r3(struct sd *sd, uint32_t command, uint32_t argument, struct sd_response *response)  {
	/* Noting to check */
	return 0;
}
static int32_t sd_check_response_r4(struct sd *sd, uint32_t command, uint32_t argument, struct sd_response *response)  {
	/* Noting to check */
	return 0;
}
static int32_t sd_check_response_r5(struct sd *sd, uint32_t command, uint32_t argument, struct sd_response *response)  {
	struct sd_generic *s = (struct sd_generic *) sd;
	return 0; /* TODO remove */
	switch (s->mode) {
		case SD_SD:
			/* SDIO Response */
			return -1;
		case SD_SDIO:
			{
				/* Fist bit is read or write */
				uint32_t status = response->data[3] >> 8;
				if (CHECK_IS_BIT_SET(status, 0)) {
					return SD_ERROR_OUT_OF_RANGE;
				}
				if (CHECK_IS_BIT_SET(status, 1)) {
					return SD_ERROR_FUNCTION_NUMBER;
				}
				/* Bit 2 is reserved */
				if (CHECK_IS_BIT_SET(status, 3)) {
					return SD_ERROR_ERROR;
				}
				/* Bit 4 -5 is IO_CURRENT_STATE */
				if (CHECK_IS_BIT_SET(status, 6)) {
					return SD_ERROR_COM_CRC_ERROR;
				}
				if (CHECK_IS_BIT_SET(status, 7)) {
					return SD_ERROR_ILLEGAL_COMMAND;
				}
			}
			break;
		case SD_SPI:
			return -1; /* TODO */
	}
	return 0;
}
static int32_t sd_check_response_r6(struct sd *sd, uint32_t command, uint32_t argument, struct sd_response *response)  {
	struct sd_generic *s = (struct sd_generic *) sd;
	uint32_t status = response->data[3];
	switch (s->mode) {
		case SD_SD:
			{
				if (CHECK_IS_BIT_SET(status, 3)) {
					return SD_ERROR_AKE_SEQ_ERROR;
				}
			}
			/* SD also have following Errors */
		case SD_SDIO:
			{
				if (CHECK_IS_BIT_SET(status, 13)) {
					return SD_ERROR_ERROR;
				}
				if (CHECK_IS_BIT_SET(status, 14)) {
					return SD_ERROR_ILLEGAL_COMMAND;
				}
				if (CHECK_IS_BIT_SET(status, 15)) {
					return SD_ERROR_COM_CRC_ERROR;
				}
			}
			break;
		case SD_SPI:
			return -1; /* TODO */
	}
	return 0;
}
static int32_t sd_check_response_r7(struct sd *sd, uint32_t command, uint32_t argument, struct sd_response *response)  {
	/* Noting to check */
	return 0;
}
int32_t sd_check_response(struct sd *sd, uint32_t command, uint32_t argument, struct sd_response *response) {
	struct sd_generic *s = (struct sd_generic *) sd;
	if (s->selectACMD) {
		s->selectACMD = false;
		if (command == CMD(41)) {
			return sd_check_response_r3(sd, command, argument, response);
		}
		return sd_check_response_r1(sd, command, argument, response);
	}
	switch(command) {
		case CMD(55):
				s->selectACMD = true;
		case CMD(11):
		case CMD(13):
		case CMD(16):
		case CMD(17):
		case CMD(18):
		case CMD(19):
		case CMD(23):
		case CMD(24):
		case CMD(25):
		case CMD(27):
		case CMD(30):
		case CMD(32):
		case CMD(33):
		case CMD(40):
		case CMD(42):
		case CMD(56):
			/* Handle R1 */
			return sd_check_response_r1(sd, command, argument, response);
		case CMD(7):
		case CMD(12):
		case CMD(20):
		case CMD(28):
		case CMD(29):
		case CMD(38):
			/* Handle R1B */
			return sd_check_response_r1(sd, command, argument, response);
		case CMD(2):
		case CMD(9):
		case CMD(10):
			/* Handle R2 */
			return sd_check_response_r2(sd, command, argument, response);
		case CMD(5):
			/* Handle R3 */
			return sd_check_response_r4(sd, command, argument, response);
		case CMD(52): /* SDIO */
		case CMD(53): /* SDIO */
			return sd_check_response_r5(sd, command, argument, response);
		case CMD(3):
			/* Handle R6 */
			return sd_check_response_r6(sd, command, argument, response);
		case CMD(8):
			/* Handle R7 */
			return sd_check_response_r7(sd, command, argument, response);
		default:
			/* Not Supported or no respone */
			return -1;
	}
}
#ifdef CONFIG_SD_MULTI
int32_t sd_setBlockSize(struct sd *sd, enum sd_block_size blockSize, TickType_t waittime);
int32_t sd_write(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data, TickType_t waittime);
int32_t sd_read(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data, TickType_t waittime);
int32_t sd_setBlockSizeISR(struct sd *sd, enum sd_block_size blockSize);
int32_t sd_writeISR(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data);
int32_t sd_readISR(struct sd *sd, uint32_t command, uint32_t argument, size_t size, uint32_t *data);
#endif
