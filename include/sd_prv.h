/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef SD_PRV_H_
#define SD_PRV_H_

#define SD_PRV_
#ifndef SD_PRV
# error "Never include this file out of a SD driver"
#endif
#include <FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include <stdbool.h>
#include <hal.h>
enum sd_responseLength {
	SD_NO_RESPONSE,
	SD_SHORT,
	SD_LONG
};
#define SD_ALREDY_INITED 1
int32_t sd_genericInit(struct sd *sd, struct sd_setting *settings);
/**
 * Return size of Response Fild
 * \param sd SD Instance
 * \param command Command
 * \param argument Argument
 * \return SD_SHORT for 48 Bit Response and SD_LONG for 136 Bit Response
 */
enum sd_responseLength sd_get_responseLength(struct sd *sd, uint32_t command, uint32_t argument);
/**
 * Check SD Response
 * \param sd SD Instance
 * \param command Command
 * \param argument Argument 
 * \param response Response form Card
 * \return < 0 on error 0 on ok
 *  -1 == Unkown Error
 * Is Response == R2, R3, R4 and R7 return is always 0
 * Is Response == R1 following error can occur (Desciption in Standart s. Card Status)
 *  SD_ERROR_AKE_SEQ_ERROR -2
 *  SD_ERROR_ERROR -3
 *  SD_ERROR_CC_ERROR -4
 *  SD_ERROR_CARD_ECC_FAIELD -5
 *  SD_ERROR_ILLEGAL_COMMAND -6
 *  SD_ERROR_COM_CRC_ERROR -7
 *  SD_ERROR_LOCK_UNLOCK_FAILED -8
 *  SD_ERROR_CARD_IS_LOCKED -9
 *  SD_ERROR_WP_VIOLATION -10
 *  SD_ERROR_ERASE_PARAM -11
 *  SD_ERROR_ERASE_SEQ_ERROR -12
 *  SD_ERROR_ERASE_LEN_ERROR -13
 *  SD_ERROR_ADDRESS_ERROR -14
 *  SD_ERROR_OUT_OF_RANGE -15
 * Is Response == R5 following error can occur (Desciption in Standart s. SDIO R5)
 *  SD_ERROR_ERROR -3
 *  SD_ERROR_ILLEGAL_COMMAND -6
 *  SD_ERROR_COM_CRC_ERROR -7
 *  SD_ERROR_OUT_OF_RANGE -15
 *  SD_ERROR_FUNCTION_NUMBER -16
 * Is Response == R6 following error can occur (Desciption in Standart Response R6)
 *  SD_ERROR_AKE_SEQ_ERROR -2
 *  SD_ERROR_ERROR -3
 *  SD_ERROR_ILLEGAL_COMMAND -6
 *  SD_ERROR_COM_CRC_ERROR -7
 */
int32_t sd_check_response(struct sd *sd, uint32_t command, uint32_t argument, struct sd_response *response);
#ifdef CONFIG_SD_THREAD_SAVE
# define sd_lock(u, w, e) HAL_LOCK(u, w, e)
# define sd_unlock(u, e) HAL_UNLOCK(u, e)
#else
# define sd_lock(u, w, e)
# define sd_unlock(u, e)
#endif
#define SD_ADDDEV(ns, p) HAL_ADDDEV(sd, ns, p)
HAL_DEFINE_GLOBAL_ARRAY(sd);
#define SD_GET_DEV(index) HAL_GET_DEV(sd, index)
#ifndef CONFIG_SD_MULTI
# define SD_OPS(ns)
# define SD_INIT_DEV(ns) 

# define SD_INIT(ns, index, s) struct sd *sd_init(uint32_t index, struct sd_setting *s)
# define SD_DEINIT(ns, p) int32_t sd_deinit(struct sd *p)
# define SD_SET_BLOCK_SIZE(ns, s, bs) int32_t sd_setBlockSize(struct sd *s, enum sd_block_size bs)
# define SD_SET_BUS_WIDE(ns, s, bw) int32_t sd_setBusWide(struct sd *s, enum sd_bus_wide bw)
# define SD_SET_CLOCK(ns, s, clk) int32_t sd_setClock(struct sd *s, uint64_t clk)
# define SD_SEND_COMMAND(ns, s, c, arg, r, w) int32_t sd_sendCommand(struct sd *s, uint32_t c, uint32_t arg, struct sd_response *r, TickType_t w)
# define SD_WIRTE(ns, s, c, arg, l, d, w) int32_t sd_write(struct sd *s, uint32_t c, uint32_t arg, size_t l, uint32_t *d, TickType_t w)
# define SD_READ(ns, s, c, arg, l, d, w) int32_t sd_read(struct sd *s, uint32_t c, uint32_t arg, size_t l, uint32_t *d, TickType_t w)
# define SD_SEND_COMMAND_ISR(ns, s, c, arg, r) int32_t sd_sendCommandISR(struct sd *s, uint32_t c, uint32_t arg, struct sd_response *r)
# define SD_WIRTE_ISR(ns, s, c, arg, l, d) int32_t sd_writeISR(struct sd *s, uint32_t c, uint32_t arg, size_t l, uint32_t *d)
# define SD_READ_ISR(ns, s, c, arg, l, d) int32_t sd_readISR(struct sd *s, uint32_t c, uint32_t arg, size_t l, uint32_t *d)
#else
# define SD_OPS(ns) static const struct sd_ops ns##_sd_ops = { \
	.sd_init = &ns##_sd_init,\
	.sd_deinit = &ns##_sd_deinit,\
	.sd_setBlockSize = &ns##_sd_setBlockSize, \
	.sd_setBusWide = &ns##_sd_setBusWide, \
	.sd_setClock = &ns##_sd_setClock, \
	.sd_sendCommand = &ns##_sd_sendCommand, \
	.sd_write = &ns##_sd_write, \
	.sd_read = &ns##_sd_read, \
	.sd_sendCommandISR = &ns##_sd_sendCommandISR, \
	.sd_writeISR = &ns##_sd_writeISR, \
	.sd_readISR = &ns##_sd_readISR, \
}
# define SD_INIT_DEV(ns) .gen.ops = &ns##_sd_ops,
# define SD_INIT(ns, s) static struct sd *ns##_sd_init(uint32_t index, struct sd_setting *s)
# define SD_DEINIT(ns, p) static int32_t ns##_sd_deinit(struct sd *p)
# define SD_SET_BLOCK_SIZE(ns, s, bs) static int32_t ns##_sd_setBlockSize(struct sd *s, enum sd_block_size bs)
# define SD_SET_BUS_WIDE(ns, s, bw) static int32_t ns##_sd_setBusWide(struct sd *s, enum sd_bus_wide bw)
# define SD_SET_CLOCK(ns, s, clk) static int32_t ns##_sd_setClock(struct sd *s, uint64_t clk)
# define SD_SEND_COMMAND(ns, s, c, arg, r, w) static int32_t ns##_sd_sendCommand(struct sd *s, uint32_t c, uint32_t arg, struct sd_response *r, TickType_t w)
# define SD_WIRTE(ns, s, c, arg, l, d, w) static int32_t ns##_sd_write(struct sd *s, uint32_t c, uint32_t arg, size_t l, uint32_t *d, TickType_t w)
# define SD_READ(ns, s, c, arg, l, d, w) static int32_t ns##_sd_read(struct sd *s, uint32_t c, uint32_t arg, size_t l, uint32_t *d, TickType_t w)
# define SD_SEND_COMMAND_ISR(ns, s, c, arg, r) static int32_t ns##_sd_sendCommandISR(struct sd *s, uint32_t c, uint32_t arg, struct sd_response *r)
# define SD_WIRTE_ISR(ns, s, c, arg, l, d) static int32_t ns##_sd_writeISR(struct sd *s, uint32_t c, uint32_t arg, size_t l, uint32_t *d)
# define SD_READ_ISR(ns, s, c, arg, l, d) static int32_t ns##_sd_readISR(struct sd *s, uint32_t c, uint32_t arg, size_t l, uint32_t *d)
#endif
#endif
