/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2018
 */
#ifndef CAN_H_
#define CAN_H_
#include <stdint.h>
#include <stdbool.h>
#include <system.h>
#include <FreeRTOS.h>
#include <semphr.h>
#include <hal.h>
#include <gpio.h>
/**
 * \defgroup CAN CAN Subsystem
 * \ingroup HAL
 * \code
 * #include <can.h>
 * \endcode
 *
 * This is the CAN Subsystem for controlling CAN of a SOC. 
 * \{
 */
/**
 * Private Structure of CAN driver
 */
struct can;
/**
 * CAN Filter Struct
 *
 * Filter ist true if:
 * <code>
 * recv.id & filter.mask == filter.id & filter.mask
 * </code>
 */
struct can_filter {
	/**
	 * ID
	 */
	uint32_t id;
	/**
	 * Mask
	 */
	uint32_t mask;
};
#ifdef CONFIG_CAN_FD
# define CAN_MAX_LENGTH 64
#else
/**
 * MAX CAN length 8 or 64 if CAN_FD
 */
# define CAN_MAX_LENGTH 8
#endif
/**
 * Frame
 */
struct can_msg {
	/**
	 * Timestamp
	 *
	 * May from a free Running Conunter of the Hardware
	 */
	uint32_t ts;
	/**
	 * ID
	 */
	uint32_t id;
	/**
	 * Request Flag
	 */
	bool req;
	/**
	 * Length
	 *
	 * DLC values ranging from 1001 to 1111 are used to specify the data lengths of 12, 16, 20, 24, 32, 48, and 64 bytes.
	 */
	uint8_t length;
	/**
	 * Daten
	 */
	uint8_t data[CAN_MAX_LENGTH];
};

typedef uint32_t can_error_t;
typedef uint64_t can_errorData_t;

/**
 * controller problems
 */
#define CAN_ERR_CRTL BIT64(0)
#define CAN_ERR_CTRL_OFFSET 0
/**
 * unspecified contoller protocol
 */
#define CAN_ERR_CRTL_UNSPEC BIT64(CAN_ERR_CTRL_OFFSET + 0)
/**
 * RX buffer overflow 
 */
#define CAN_ERR_CRTL_RX_OVERFLOW BIT64(CAN_ERR_CTRL_OFFSET + 1)
/**
 * TX buffer overflow
 */
#define CAN_ERR_CRTL_TX_OVERFLOW BIT64(CAN_ERR_CTRL_OFFSET + 2)
/**
 * reached warning level for RX errors
 */
#define CAN_ERR_CRTL_RX_WARNING BIT64(CAN_ERR_CTRL_OFFSET + 3)
/*
 * Reached warning level for TX errors
 */
#define CAN_ERR_CRTL_TX_WARNING BIT64(CAN_ERR_CTRL_OFFSET + 4)
/**
 * reached error passive status RX
 */
#define CAN_ERR_CRTL_RX_PASSIVE BIT64(CAN_ERR_CTRL_OFFSET + 5)
/**
 * reached error passive status TX
 * (at least one error counter exceeds
 * the protocol-defined level of 127)
 */
#define CAN_ERR_CRTL_TX_PASSIVE BIT64(CAN_ERR_CTRL_OFFSET + 6)
/**
 * recovered to error active state
 * (one error counter is under a
 * the protocol-defined level of 127)
 */
#define CAN_ERR_CRTL_ACTIVE BIT64(CAN_ERR_CTRL_OFFSET + 7)
/**
 * protocol violations
 */
#define CAN_ERR_PROT BIT64(1)
#define CAN_ERR_PROT_OFFSET 8
/**
 * Unspecified protocol violations
 */
#define CAN_ERR_PROT_UNSPEC BIT64(CAN_ERR_PROT_OFFSET + 0)
/**
 * Single bit error
 */
#define CAN_ERR_PROT_BIT BIT64(CAN_ERR_PROT_OFFSET + 1)
/**
 * frame format error
 */
#define CAN_ERR_PROT_FORM BIT64(CAN_ERR_PROT_OFFSET + 2)
/**
 * Bit stuffing error
 */
#define CAN_ERR_PROT_STUFF BIT64(CAN_ERR_PROT_OFFSET + 3)
/**
 * Unable to send dominant bit
 */
#define CAN_ERR_PROT_BIT0 BIT64(CAN_ERR_PROT_OFFSET + 4)
/**
 * Unable to send recessive bit
 */
#define CAN_ERR_PROT_BIT1 BIT64(CAN_ERR_PROT_OFFSET + 5)
/**
 * Bus overload
 */
#define CAN_ERR_PROT_OVERLOAD BIT64(CAN_ERR_PROT_OFFSET + 6)
/**
 * Active error announcement
 */
#define CAN_ERR_PROT_ACTIVE BIT64(CAN_ERR_PROT_OFFSET + 7)
/**
 * Error occurred on transmission
 */
#define CAN_ERR_PROT_TX BIT64(CAN_ERR_PROT_OFFSET + 8)

#define CAN_ERR_PROT_LOC_OFFSET 24
/**
 * Unspecified error in CAN protocol (location)
 */
#define CAN_ERR_PROT_LOC_UNSPEC BIT64(CAN_ERR_PROT_LOC_OFFSET + 0)
/**
 * Error in CAN protocol at
 * Start of frame
 */
#define CAN_ERR_PROT_LOC_SOF BIT64(CAN_ERR_PROT_LOC_OFFSET + 1)
/**
 * Error in CAN protocol at
 * ID bits 28 - 21 (SFF: 10 - 3)
 */
#define CAN_ERR_PROT_LOC_ID28_21 BIT64(CAN_ERR_PROT_LOC_OFFSET + 2)
/**
 * Error in CAN protocol at
 * ID bits 20 - 18 (SFF: 2 - 0)
 */
#define CAN_ERR_PROT_LOC_ID20_18 BIT64(CAN_ERR_PROT_LOC_OFFSET + 3)
/**
 * Error in CAN protocol at
 * Substitute RTR (SFF: RTR)
 */
#define CAN_ERR_PROT_LOC_SRTR BIT64(CAN_ERR_PROT_LOC_OFFSET + 4)
/**
 * Error in CAN protocol at
 * Identifier extension
 */
#define CAN_ERR_PROT_LOC_IDE BIT64(CAN_ERR_PROT_LOC_OFFSET + 5)
/**
 * Error in CAN protocol at
 * ID bits 17-13
 */
#define CAN_ERR_PROT_LOC_ID17_13 BIT64(CAN_ERR_PROT_LOC_OFFSET + 6)
/**
 * Error in CAN protocol at
 * ID bits 12-5
 */
#define CAN_ERR_PROT_LOC_ID12_05 BIT64(CAN_ERR_PROT_LOC_OFFSET + 7)
/**
 * Error in CAN protocol at
 * ID bits 4-0
 */
#define CAN_ERR_PROT_LOC_ID04_00 BIT64(CAN_ERR_PROT_LOC_OFFSET + 8)
/**
 * Error in CAN protocol at
 * RTR
 */
#define CAN_ERR_PROT_LOC_RTR BIT64(CAN_ERR_PROT_LOC_OFFSET + 9)
/**
 * Error in CAN protocol at
 * reserved bit 1
 */
#define CAN_ERR_PROT_LOC_RES1 BIT64(CAN_ERR_PROT_LOC_OFFSET + 10)
/**
 * Error in CAN protocol at
 * reserved bit 0
 */
#define CAN_ERR_PROT_LOC_RES0 BIT64(CAN_ERR_PROT_LOC_OFFSET + 11)
/**
 * Error in CAN protocol at
 * data length code
 */
#define CAN_ERR_PROT_LOC_DLC BIT64(CAN_ERR_PROT_LOC_OFFSET + 12)
/**
 * Error in CAN protocol at
 * data section
 */
#define CAN_ERR_PROT_LOC_DATA BIT64(CAN_ERR_PROT_LOC_OFFSET + 13)
/**
 * Error in CAN protocol at
 * CRC sequence
 */
#define CAN_ERR_PROT_LOC_CRC_SEQ BIT64(CAN_ERR_PROT_LOC_OFFSET + 14)
/**
 * Error in CAN protocol at
 * CRC delimiter
 */
#define CAN_ERR_PROT_LOC_CRC_DEL BIT64(CAN_ERR_PROT_LOC_OFFSET + 15)
/**
 * Error in CAN protocol at
 * ACK slot
 */
#define CAN_ERR_PROT_LOC_ACK BIT64(CAN_ERR_PROT_LOC_OFFSET + 16)
/**
 * Error in CAN protocol at
 * ACK delimiter
 */
#define CAN_ERR_PROT_LOC_ACK_DEL BIT64(CAN_ERR_PROT_LOC_OFFSET + 17)
/**
 * Error in CAN protocol at
 * end of frame
 */
#define CAN_ERR_PROT_LOC_EOF BIT64(CAN_ERR_PROT_LOC_OFFSET + 18)
/**
 * Error in CAN protocol at
 * intermission
 */
#define CAN_ERR_PROT_LOC_INTERM BIT64(CAN_ERR_PROT_LOC_OFFSET + 19)

/**
 * transceiver status
 */
#define CAN_ERR_TRX BIT64(2)
#define CAN_ERR_TRX_OFFSET 48
/**
 * Transiver Status Error is unspecified
 */
#define CAN_ERR_TRX_UNSPEC BIT64(CAN_ERR_TRX_OFFSET + 0)
/**
 * No Wire at CAN High
 */
#define CAN_ERR_TRX_CANH_NO_WIRE BIT64(CAN_ERR_TRX_OFFSET + 1)
/**
 * CAN High is short to Bat
 */
#define CAN_ERR_TRX_CANH_SHORT_TO_BAT BIT64(CAN_ERR_TRX_OFFSET + 2)
/**
 * CAN High is short to VCC
 */
#define CAN_ERR_TRX_CANH_SHORT_TO_VCC BIT64(CAN_ERR_TRX_OFFSET + 3)

/**
 * CAN High is short to GND
 */
#define CAN_ERR_TRX_CANH_SHORT_TO_GND BIT64(CAN_ERR_TRX_OFFSET + 4)
/**
 * No Wire at CAN Low
 */
#define CAN_ERR_TRX_CANL_NO_WIRE BIT64(CAN_ERR_TRX_OFFSET + 5)
/**
 * CAN Low is short to Bat
 */
#define CAN_ERR_TRX_CANL_SHORT_TO_BAT BIT64(CAN_ERR_TRX_OFFSET + 6)
/**
 * CAN Low is short to VCC
 */
#define CAN_ERR_TRX_CANL_SHORT_TO_VCC BIT64(CAN_ERR_TRX_OFFSET + 7)
/**
 * CAN Low is short to GND
 */
#define CAN_ERR_TRX_CANL_SHORT_TO_GND BIT64(CAN_ERR_TRX_OFFSET + 8)
/**
 * CAN Low is short to CAN High
 */
#define CAN_ERR_TRX_CANL_SHORT_TO_CANH BIT64(CAN_ERR_TRX_OFFSET + 9)
/**
 * received no ACK on transmission
 */
#define CAN_ERR_ACK BIT64(3)
/**
 * bus off
 */
#define CAN_ERR_BUSOFF BIT64(4)
/**
 * bus error (may flood!)
 */
#define CAN_ERR_BUSERROR BIT64(5)
/**
 * controller restarted
 */
#define CAN_ERR_RESTARTED BIT64(6)

#ifdef CONFIG_CAN_MULTI
/**
 * Function of CAN driver in Multi Driver implementation mode 
 */
struct can_ops {
	struct can *(*can_init)(uint32_t index, uint32_t bitrate, struct gpio_pin *pin, bool pinHigh, bool (*callback)(struct can *can, can_error_t error, can_errorData_t data), void *data);
	int32_t (*can_deinit)(struct can *can);
	int32_t (*can_setCallback)(struct can *can, int32_t filterID, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data);
	int32_t (*can_registerFilter)(struct can *can, struct can_filter *filter);
	int32_t (*can_deregisterFilter)(struct can *can, int32_t filterID);
	int32_t (*can_send)(struct can *can, struct can_msg *msg, TickType_t waittime);
	int32_t (*can_recv)(struct can *can, int32_t filterID, struct can_msg *msg, TickType_t waittime);
	int32_t (*can_sendISR)(struct can *can, struct can_msg *msg);
	int32_t (*can_recvISR)(struct can *can, int32_t filterID, struct can_msg *msg);
	int32_t (*can_up)(struct can *can);
	int32_t (*can_down)(struct can *can);
};
#endif
/**
 * Generic CAN Interface  
 */
struct can_generic {
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
#ifdef CONFIG_CAN_THREAD_SAVE
	/**
	 * Mutex
	 */
	OS_DEFINE_MUTEX_RECURSIVE(lock);
#endif
#ifdef CONFIG_CAN_MULTI
	/**
	 * Ops of driver in Multi mode
	 */
	const struct can_ops *ops;
#endif
};
#ifndef CONFIG_CAN_MULTI
/**
 * init CAN instance
 * \param index Index of CAN
 * \param bitrate bitrate
 * \param pin enable pin
 * \param pinHigh Enable Pin Driven High or low for enable
 * \param callback Callback if error is happened, ignored if defined NULL, The Hardware or the diver should restore the bus of Bus off automatically
 * \param data Callback User Data
 * \return CAN Instance NULL on Error if only get instance on bits == 0 
 */
struct can *can_init(uint32_t index, uint32_t bitrate, struct gpio_pin *pin, bool pinHigh, bool (*callback)(struct can *can, can_error_t error, can_errorData_t data), void *data);

/**
 * Deinit CAN
 * \param can CAN instance
 * \return -1 on error 0 on ok
 */
int32_t can_deinit(struct can *can);

/**
 * Set CAN Interrupt Callback for a filter
 *
 * \warning This Callback is may executed in the contex of CAN interrupt ISR!
 * \param can CAN instance
 * \param filterID filterID
 * \param callback Callback
 * \param data Data parsed to Callback
 * \return -1 on error 0 on ok
 */
int32_t can_setCallback(struct can *can, int32_t filterID, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data);
/**
 * Register CAN Filter
 * \param can CAN instance
 * \param filter Filter
 * \return -1 on error or filterID
 */
int32_t can_registerFilter(struct can *can, struct can_filter *filter);
/**
 * Deregister CAN Filter
 * \param can CAN instance
 * \param filterID FilterID
 * \return -1 on error 0 on ok
 */
int32_t can_deregisterFilter(struct can *can, int32_t filterID);

/**
 * Send CAN Message
 * \param can CAN instance
 * \param msg CAN Message
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t can_send(struct can *can, struct can_msg *msg, TickType_t waittime);
/**
 * Receive CAN Message
 * \param can CAN instance
 * \param filterID filterID
 * \param msg CAN Message
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t can_recv(struct can *can, int32_t filterID, struct can_msg *msg, TickType_t waittime);
/**
 * Send CAN Message
 * \param can CAN instance
 * \param msg CAN Message
 * \return -1 on error 0 on ok
 */
int32_t can_sendISR(struct can *can, struct can_msg *msg);
/**
 * Receive CAN Message
 * \param can CAN instance
 * \param filterID filterID
 * \param msg CAN Message
 * \return -1 on error 0 on ok
 */
int32_t can_recvISR(struct can *can, int32_t filterID, struct can_msg *msg);

/**
 * Start CAN Net
 * \param can CAN instance
 * \return -1 on error 0 on ok
 */
int32_t can_up(struct can *can);

/**
 * Stop CAN Net
 * \param can CAN instance
 * \return -1 on error 0 on ok
 */
int32_t can_down(struct can *can);

#else
inline struct can *can_init(uint32_t index, uint32_t bitrate, struct gpio_pin *pin, bool pinHigh, bool (*callback)(struct can *can, can_error_t error, can_errorData_t data), void *data) {
	HAL_DEFINE_GLOBAL_ARRAY(can);
	struct can_generic *a = (struct can_generic *) HAL_GET_DEV(can, index);
	if (a == NULL) {
		return NULL;
	}
	return a->ops->can_init(index, bitrate, pin, pinHigh, callback, data);
}
inline int32_t can_deinit(struct can *can) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_deinit(can);
}
inline int32_t can_setCallback(struct can *can, int32_t filterID, bool (*callback)(struct can *can, struct can_msg *msg, void *data), void *data) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_setCallback(can, filterID, callback, data);
}
inline int32_t can_registerFilter(struct can *can, struct can_filter *filter) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_registerFilter(can, filter);
}
inline int32_t can_deregisterFilter(struct can *can, int32_t filterID) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_deregisterFilter(can, filterID);
}
inline int32_t can_send(struct can *can, struct can_msg *msg, TickType_t waittime) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_send(can, msg, waittime);
}
inline int32_t can_recv(struct can *can, int32_t filterID, struct can_msg *msg, TickType_t waittime) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_recv(can, filterID, msg, waittime);
}
inline int32_t can_sendISR(struct can *can, struct can_msg *msg) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_sendISR(can, msg);
}
inline int32_t can_recvISR(struct can *can, int32_t filterID, struct can_msg *msg) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_recvISR(can, filterID, msg);
}
inline int32_t can_up(struct can *can) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_up(can);
}
inline int32_t can_down(struct can *can) {
	struct can_generic *a = (struct can_generic *) can;
	return a->ops->can_down(can);
}
#endif
/**\}*/
#endif
