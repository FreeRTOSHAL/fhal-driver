/* SPDX-License-Identifier: MIT */
/*
 * Author: Andreas Werner <kernel@andy89.org>
 * Date: 2016
 */
#ifndef MPU_9250_H_
#define MPU_9250_H_
#include <stdint.h>
#include <spi.h>
#include <vec.h>
#include <accel.h>
#include <gyro.h>
/**
 * \defgroup MPU9250 MPU9250 driver
 * \ingroup HAL
 * \code
 * #include <mpu9250.h>
 * \endcode
 * 
 * This is the MPU9250 Driver.
 * \{
 */

/**\cond INTERNAL*/
#define MPU_READ BIT(7)

#ifdef CONFIG_MPU9250
# define MPU_SELF_TEST_X_GYRO 0x00
# define MPU_SELF_TEST_Y_GYRO 0x01
# define MPU_SELF_TEST_Z_GYRO 0x02
# define MPU_SELF_TEST_X_ACCEL 0x0D
# define MPU_SELF_TEST_Y_ACCEL 0x0E
# define MPU_SELF_TEST_Z_ACCEL 0x0F
# define MPU_XG_OFFSET_H 0x13
# define MPU_XG_OFFSET_L 0x14
# define MPU_YG_OFFSET_H 0x15
# define MPU_YG_OFFSET_L 0x16
# define MPU_ZG_OFFSET_H 0x17
# define MPU_ZG_OFFSET_L 0x18
# define MPU_SMPLRT_DIV 0x19
# define MPU_CONFIG 0x1A
# define MPU_CONFIG_DLPF_CFG(x) (((x) & 0x7) << 0)
# define MPU_CONFIG_EXT_SYNC_SET(x) (((x) & 0x7) << 3)
# define MPU_CONFIG_FIFO_MODE BIT(6)
# define MPU_GYRO_CONFIG 0x1B
# define MPU_GYRO_CONFIG_FCHOICE_B(x) (((x) & 0x3) << 0)
# define MPU_GYRO_CONFIG_GYRO_FS_SEL(x) (((x) & 0x3) << 3)
# define MPU_GYRO_CONFIG_ZGYRO_CT_EN BIT(5)
# define MPU_GYRO_CONFIG_YGYRO_CT_EN BIT(6)
# define MPU_GYRO_CONFIG_XGYRO_CT_EN BIT(7)
# define MPU_ACCEL_CONFIG 0x1C
# define MPU_ACCEL_CONFIG_ACCEL_FS_SEL(x) (((x) & 0x3) << 3)
# define MPU_ACCEL_CONFIG_AZ_ST_EN BIT(5)
# define MPU_ACCEL_CONFIG_AY_ST_EN BIT(6)
# define MPU_ACCEL_CONFIG_AX_ST_EN BIT(7)
# define MPU_ACCEL_CONFIG_2 0x1D
# define MPU_ACCEL_CONFIG_2_A_DLPF_CFG(x) (((x) & 0x3) << 0)
# define MPU_ACCEL_CONFIG_2_ACCEL_FCHOICE_B(x) (((x) & 0x3) << 2)
# define MPU_LP_ACCEL_ODR 0x1E
# define MPU_LP_ACCEL_ODR_LPOSC_CLKSEL (((x) & 0xF) << 0)
# define MPU_WOM_THR 0x1F
# define MPU_FIFO_EN 0x23
# define MPU_FIFO_EN_SLV0 BIT(0)
# define MPU_FIFO_EN_SLV1 BIT(1)
# define MPU_FIFO_EN_SLV2 BIT(2)
# define MPU_FIFO_EN_ACCEL BIT(3)
# define MPU_FIFO_EN_GYRO_ZOUT BIT(4)
# define MPU_FIFO_EN_GYRO_YOUT BIT(5)
# define MPU_FIFO_EN_GYRO_GYRO_XOUT BIT(6)
# define MPU_FIFO_EN_TEMP_FIFO_EN BIT(7)
# define MPU_I2C_MST_CTRL 0x24
# define MPU_I2C_MST_CTRL_I2C_MST_CLK(x) (((x) & 0xF) << 0)
# define MPU_I2C_MST_CTRL_I2C_MST_P_NSR(x) BIT(4)
# define MPU_I2C_MST_CTRL_SLV_3_FIFO_EN BIT(5)
# define MPU_I2C_MST_CTRL_WAIT_FOR_ES BIT(6)
# define MPU_I2C_MST_CTRL_MULT_MST_EN BIT(7)
# define MPU_I2C_SLV0_ADDR 0x25
# define MPU_I2C_SLV_ADDR_I2C_ID(x) (((x) & 0x7F) << 0)
# define MPU_I2C_SLV_ADDR_I2C_SLV0_RNW BIT(7)
# define MPU_I2C_SLV0_REG 0x38
# define MPU_I2C_SLV0_CTRL 0x27
# define MPU_I2C_SLV_CTRL_I2C_SLV_LENG(x) (((x) & 0xF) << 0)
# define MPU_I2C_SLV_CTRL_I2C_SLV_GRP BIT(4)
# define MPU_I2C_SLV_CTRL_I2C_SLV_REG_DIS BIT(5)
# define MPU_I2C_SLV_CTRL_I2C_SLV_BYTE_SW BIT(6)
# define MPU_I2C_SLV_CTRL_I2C_SLV_EN BIT(7)
# define MPU_I2C_SLV1_ADDR 0x28
# define MPU_I2C_SLV1_REG 0x29
# define MPU_I2C_SLV1_CTRL 0x2A
# define MPU_I2C_SLV2_ADDR 0x2B
# define MPU_I2C_SLV2_REG 0x2C
# define MPU_I2C_SLV2_CTRL 0x2D
# define MPU_I2C_SLV3_ADDR 0x2E
# define MPU_I2C_SLV3_REG 0x2F
# define MPU_I2C_SLV3_CTRL 0x30
# define MPU_I2C_SLV4_ADDR 0x31
# define MPU_I2C_SLV4_REG 0x32
# define MPU_I2C_SLV4_DO 0x33
# define MPU_I2C_SLV4_CTRL 0x34
# define MPU_I2C_SLV4_DI 0x35
# define MPU_I2C_MST_STATUS 0x36
# define MPU_I2C_MST_STATUS_I2C_SLV0_NACK BIT(0)
# define MPU_I2C_MST_STATUS_I2C_SLV1_NACK BIT(1)
# define MPU_I2C_MST_STATUS_I2C_SLV2_NACK BIT(2)
# define MPU_I2C_MST_STATUS_I2C_SLV3_NACK BIT(3)
# define MPU_I2C_MST_STATUS_I2C_SLV4_NACK BIT(4)
# define MPU_I2C_MST_STATUS_I2C_LOST_ARB BIT(5)
# define MPU_I2C_MST_STATUS_I2C_SLV4_DONE BIT(6)
# define MPU_I2C_MST_STATUS_PASS_THROUGH BIT(7)
# define MPU_INT_PIN_CFG 0x37
# define MPU_INT_PIN_CFG_BYPASS_EN BIT(1)
# define MPU_INT_PIN_CFG_FSYNC_INT_MODE_EN BIT(2)
# define MPU_INT_PIN_CFG_ACTL_FSYNC BIT(3)
# define MPU_INT_PIN_CFG_INT_ANYRD_2CLEAR BIT(4)
# define MPU_INT_PIN_CFG_LATCH_INT_EN BIT(5)
# define MPU_INT_PIN_CFG_OPEN BIT(6)
# define MPU_INT_PIN_CFG_ACTL BIT(7)
# define MPU_INT_ENABLE 0x38
# define MPU_INT_ENABLE_RAW_RDY_EN BIT(0)
# define MPU_INT_ENABLE_FSYNC_INT_EN BIT(3)
# define MPU_INT_ENABLE_FIFO_OFLOW_EN BIT(4)
# define MPU_INT_ENABLE_WOM_EN BIT(5)
# define MPU_INT_STATUS 0x3A
# define MPU_INT_STATUS_RAW_DATA_RDY_INT BIT(0)
# define MPU_INT_STATUS_FSYNC_INT BIT(3)
# define MPU_INT_STATUS_FIFO_OFLOW_INT BIT(4)
# define MPU_INT_STATUS_WOM_INT BIT(6)
# define MPU_ACCEL_XOUT_H 0x3B
# define MPU_ACCEL_XOUT_L 0x3C
# define MPU_ACCEL_YOUT_H 0x3D
# define MPU_ACCEL_YOUT_L 0x3E
# define MPU_ACCEL_ZOUT_H 0x3F
# define MPU_ACCEL_ZOUT_L 0x40
# define MPU_TEMP_OUT_H 0x41
# define MPU_TEMP_OUT_L 0x42
# define MPU_GYRO_XOUT_H 0x43
# define MPU_GYRO_XOUT_L 0x44
# define MPU_GYRO_YOUT_H 0x45
# define MPU_GYRO_YOUT_L 0x46
# define MPU_GYRO_ZOUT_H 0x47
# define MPU_GYRO_ZOUT_L 0x48
# define MPU_EXT_SENS_DATA(x) (0x49 + x)
# define MPU_I2C_SLV0_DO 0x63
# define MPU_I2C_SLV1_DO 0x64
# define MPU_I2C_SLV2_DO 0x65
# define MPU_I2C_SLV3_DO 0x66
# define MPU_I2C_MST_DELAY_CTRL 0x67
# define MPU_I2C_MST_DELAY_CTRL_I2C_SLV0_DLY_EN BIT(0)
# define MPU_I2C_MST_DELAY_CTRL_I2C_SLV1_DLY_EN BIT(1)
# define MPU_I2C_MST_DELAY_CTRL_I2C_SLV2_DLY_EN BIT(2)
# define MPU_I2C_MST_DELAY_CTRL_I2C_SLV3_DLY_EN BIT(3)
# define MPU_I2C_MST_DELAY_CTRL_I2C_SLV4_DLY_EN BIT(4)
# define MPU_I2C_MST_DELAY_CTRL_DELAY_ES_SHADOW BIT(5)
# define MPU_SIGNAL_PATH_RESET 0x68
# define MPU_SIGNAL_PATH_RESET_TEMP_RST BIT(0)
# define MPU_SIGNAL_PATH_RESET_ACCEL_RST BIT(1)
# define MPU_SIGNAL_PATH_RESET_GYRO_RST BIT(2)
# define MPU_MOT_DETECT_CTRL 0x69
# define MPU_MOT_DETECT_CTRL_ACCEL_INTEL_MODE BIT(6)
# define MPU_MOT_DETECT_CTRL_ACCEL_INTEL_EN BIT(7)
# define MPU_USER_CTRL 0x6A
# define MPU_USER_CTRL_SIG_COND_RST BIT(0)
# define MPU_USER_CTRL_I2C_MST_RST BIT(1)
# define MPU_USER_CTRL_FIFO_RST BIT(2)
# define MPU_USER_CTRL_I2C_IF_DIS BIT(4)
# define MPU_USER_CTRL_I2C_MST_EN BIT(5)
# define MPU_USER_CTRL_FIFO_EN BIT(6)
# define MPU_PWR_MGMT_1 0x6B
# define MPU_PWR_MGMT_1_CLKSEL(x) (((x) & 0x7) << 0)
# define MPU_PWR_MGMT_1_PD_PTAT BIT(3)
# define MPU_PWR_MGMT_1_GYRO_STANDBY BIT(4)
# define MPU_PWR_MGMT_1_CYCLE BIT(5)
# define MPU_PWR_MGMT_1_SLEEP BIT(6)
# define MPU_PWR_MGMT_1_H_RESET BIT(7)
# define MPU_PWR_MGMT_2 0x6C
# define MPU_PWR_MGMT_2_DIS_ZG BIT(0)
# define MPU_PWR_MGMT_2_DIS_YG BIT(1)
# define MPU_PWR_MGMT_2_DIS_XG BIT(2)
# define MPU_PWR_MGMT_2_DIS_ZA BIT(3)
# define MPU_PWR_MGMT_2_DIS_YA BIT(4)
# define MPU_PWR_MGMT_2_DIS_XA BIT(5)
# define MPU_FIFO_COUNTH 0x72
# define MPU_FIFO_COUNTL 0x73
# define MPU_FIFO_R_W 0x74
# define MPU_WHO_AM_I 0x75
# define MPU_WHO_AM_I_VAL 0x71
# define MPU_XA_OFFSET_H 0x77
# define MPU_XA_OFFSET_L 0x78
# define MPU_YA_OFFSET_H 0x7A
# define MPU_YA_OFFSET_L 0x7B
# define MPU_ZA_OFFSET_H 0x7D
# define MPU_ZA_OFFSET_L 0x7E
#else /* CONFIG_MPU9250 */
# define MPU_BANK1 (1 << 8)
# define MPU_BANK2 (2 << 8)
# define MPU_BANK3 (3 << 8)
# define MPU_BANK4 (4 << 8)

# define MPU_REG_BANK_SEL 0x7F
# define MPU_REG_BANK_SEL_USER_BANK(x) (((x) & 0x3) << 4)

# define MPU_WHO_AM_I (0x00 | MPU_BANK1)
# define MPU_WHO_AM_I_VAL 0xEA
# define MPU_USER_CTRL (0x03 | MPU_BANK1)
# define MPU_USER_CTRL_I2C_MST_RST BIT(1)
# define MPU_USER_CTRL_SRAM_RST BIT(2)
# define MPU_USER_CTRL_DMP_RST BIT(3)
# define MPU_USER_CTRL_I2C_IF_DIS BIT(4)
# define MPU_USER_CTRL_I2C_MST_EN BIT(5)
# define MPU_USER_CTRL_FIFO_EN BIT(6)
# define MPU_USER_CTRL_DMP_EN BIT(7)
# define MPU_LP_CONFIG (0x05 | MPU_BANK1)
# define MPU_PWR_MGMT_1 (0x06 | MPU_BANK1)
# define MPU_PWR_MGMT_1_CLKSEL(x) (((x) & 0x7) << 0)
# define MPU_PWR_MGMT_1_TEMP_DIS BIT(3)
# define MPU_PWR_MGMT_1_LP_EN BIT(5)
# define MPU_PWR_MGMT_1_SLEEP BIT(6)
# define MPU_PWR_MGMT_1_DEVICE_RESET BIT(7)
# define MPU_PWR_MGMT_2 (0x07 | MPU_BANK1)
# define MPU_INT_PIN_CFG (0x0F | MPU_BANK1)
# define MPU_INT_ENABLE (0x10 | MPU_BANK1)
# define MPU_INT_ENABLE_1 (0x11 | MPU_BANK1)
# define MPU_INT_ENABLE_2 (0x12 | MPU_BANK1)
# define MPU_INT_ENABLE_3 (0x13 | MPU_BANK1)
# define MPU_I2C_MST_STATUS (0x17 | MPU_BANK1)
# define MPU_INT_STATUS (0x19 | MPU_BANK1)
# define MPU_INT_STATUS_1 (0x1A | MPU_BANK1)
# define MPU_INT_STATUS_2 (0x1B | MPU_BANK1)
# define MPU_INT_STATUS_3 (0x1C | MPU_BANK1)
# define MPU_DELAY_TIMEH (0x28 | MPU_BANK1)
# define MPU_DELAY_TIMEL (0x29 | MPU_BANK1)
# define MPU_ACCEL_XOUT_H (0x2D | MPU_BANK1)
# define MPU_ACCEL_XOUT_L (0x2E | MPU_BANK1)
# define MPU_ACCEL_YOUT_H (0x2F | MPU_BANK1)
# define MPU_ACCEL_YOUT_L (0x30 | MPU_BANK1)
# define MPU_ACCEL_ZOUT_H (0x31 | MPU_BANK1)
# define MPU_ACCEL_ZOUT_L (0x32 | MPU_BANK1)
# define MPU_GYRO_XOUT_H (0x33 | MPU_BANK1)
# define MPU_GYRO_XOUT_L (0x34 | MPU_BANK1)
# define MPU_GYRO_YOUT_H (0x35 | MPU_BANK1)
# define MPU_GYRO_YOUT_L (0x36 | MPU_BANK1)
# define MPU_GYRO_ZOUT_H (0x37 | MPU_BANK1)
# define MPU_GYRO_ZOUT_L (0x38 | MPU_BANK1)
# define MPU_TEMP_OUT_H (0x39 | MPU_BANK1)
# define MPU_TEMP_OUT_L (0x3A | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_00 (0x3B | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_01 (0x3C | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_02 (0x3D | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_03 (0x3E | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_04 (0x3F | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_05 (0x40 | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_06 (0x41 | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_07 (0x42 | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_08 (0x43 | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_09 (0x44 | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_10 (0x45 | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_11 (0x46 | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_12 (0x47 | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_13 (0x48 | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_14 (0x49 | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_15 (0x4A | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_16 (0x4B | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_17 (0x4C | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_18 (0x4D | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_19 (0x4E | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_20 (0x4F | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_21 (0x50 | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_22 (0x51 | MPU_BANK1)
# define MPU_EXT_SLV_SENS_DATA_23 (0x52 | MPU_BANK1)
# define MPU_FIFO_EN_1 (0x66 | MPU_BANK1)
# define MPU_FIFO_EN_2 (0x67 | MPU_BANK1)
# define MPU_FIFO_EN_2_TEMP_FIFO_EN BIT(0)
# define MPU_FIFO_EN_2_GYRO_X_FIFO_EN BIT(1)
# define MPU_FIFO_EN_2_GYRO_Y_FIFO_EN BIT(2)
# define MPU_FIFO_EN_2_GYRO_Z_FIFO_EN BIT(3)
# define MPU_FIFO_EN_2_ACCEL_FIFO_EN BIT(4)
# define MPU_FIFO_RST (0x68 | MPU_BANK1)
# define MPU_FIFO_MODE (0x69 | MPU_BANK1)
# define MPU_FIFO_COUNTH (0x70 | MPU_BANK1)
# define MPU_FIFO_COUNTL (0x71 | MPU_BANK1)
# define MPU_FIFO_R_W (0x72 | MPU_BANK1)
# define MPU_DATA_RDY_STATUS (0x74 | MPU_BANK1)
# define MPU_FIFO_CFG (0x76 | MPU_BANK1)

# define MPU_SELF_TEST_X_GYRO (0x02 | MPU_BANK2)
# define MPU_SELF_TEST_Y_GYRO (0x03 | MPU_BANK2)
# define MPU_SELF_TEST_Z_GYRO (0x04 | MPU_BANK2)
# define MPU_SELF_TEST_X_ACCEL (0x0E | MPU_BANK2)
# define MPU_SELF_TEST_Y_ACCEL (0x0F | MPU_BANK2)
# define MPU_SELF_TEST_Z_ACCEL (0x10 | MPU_BANK2)
# define MPU_XA_OFFS_H (0x14 | MPU_BANK2)
# define MPU_XA_OFFS_L (0x15 | MPU_BANK2)
# define MPU_YA_OFFS_H (0x17 | MPU_BANK2)
# define MPU_YA_OFFS_L (0x18 | MPU_BANK2)
# define MPU_ZA_OFFS_H (0x1A | MPU_BANK2)
# define MPU_ZA_OFFS_L (0x1B | MPU_BANK2)
# define MPU_TIMEBASE_CORRECTIO (0x28 | MPU_BANK2)

# define MPU_GYRO_SMPLRT_DIV (0x00 | MPU_BANK3)
# define MPU_GYRO_CONFIG_1 (0x01 | MPU_BANK3)
# define MPU_GYRO_CONFIG_1_GYRO_FCHOICE BIT(0)
# define MPU_GYRO_CONFIG_1_GYRO_FS_SEL(x) (((x) & 0x3) << 1)
# define MPU_GYRO_CONFIG_1_GYRO_FS_SEL_250DPS MPU_GYRO_CONFIG_1_GYRO_FS_SEL(0)
# define MPU_GYRO_CONFIG_1_GYRO_FS_SEL_500DPS MPU_GYRO_CONFIG_1_GYRO_FS_SEL(1)
# define MPU_GYRO_CONFIG_1_GYRO_FS_SEL_1000DPS MPU_GYRO_CONFIG_1_GYRO_FS_SEL(2)
# define MPU_GYRO_CONFIG_1_GYRO_FS_SEL_2000DPS MPU_GYRO_CONFIG_1_GYRO_FS_SEL(3)
# define MPU_GYRO_CONFIG_1_GYRO_DLPFCFG(x) (((x) & 0x3) << 3)
# define MPU_GYRO_CONFIG_2 (0x02 | MPU_BANK3)
# define MPU_XG_OFFS_USRH (0x03 | MPU_BANK3)
# define MPU_XG_OFFS_USRL (0x04 | MPU_BANK3)
# define MPU_YG_OFFS_USRH (0x05 | MPU_BANK3)
# define MPU_YG_OFFS_USRL (0x06 | MPU_BANK3)
# define MPU_ZG_OFFS_USRH (0x07 | MPU_BANK3)
# define MPU_ZG_OFFS_USRL (0x08 | MPU_BANK3)
# define MPU_ODR_ALIGN_EN (0x09 | MPU_BANK3)
# define MPU_ACCEL_SMPLRT_DIV_1 (0x10 | MPU_BANK3)
# define MPU_ACCEL_SMPLRT_DIV_2 (0x11 | MPU_BANK3)
# define MPU_ACCEL_INTEL_CTRL (0x12 | MPU_BANK3)
# define MPU_ACCEL_WOM_THR (0x13 | MPU_BANK3)
# define MPU_ACCEL_CONFIG (0x14 | MPU_BANK3)
# define MPU_ACCEL_CONFIG_ACCEL_FCHOICE BIT(0)
# define MPU_ACCEL_CONFIG_ACCEL_FS_SEL(x) (((x) & 0x3) << 1)
# define MPU_ACCEL_CONFIG_ACCEL_FS_SEL_2G MPU_ACCEL_CONFIG_ACCEL_FS_SEL(0)
# define MPU_ACCEL_CONFIG_ACCEL_FS_SEL_4G MPU_ACCEL_CONFIG_ACCEL_FS_SEL(1)
# define MPU_ACCEL_CONFIG_ACCEL_FS_SEL_8G MPU_ACCEL_CONFIG_ACCEL_FS_SEL(2)
# define MPU_ACCEL_CONFIG_ACCEL_FS_SEL_16G MPU_ACCEL_CONFIG_ACCEL_FS_SEL(3)
# define MPU_ACCEL_CONFIG_ACCEL_DLPFCFG(x) (((x) & 0x3) << 3)
# define MPU_ACCEL_CONFIG_2 (0x15 | MPU_BANK3)
# define MPU_FSYNC_CONFIG (0x52 | MPU_BANK3)
# define MPU_TEMP_CONFIG (0x53 | MPU_BANK3)
# define MPU_MOD_CTRL_USR (0x54 | MPU_BANK3)

# define MPU_I2C_MST_ODR_CONFIG (0x00 | MPU_BANK4)
# define MPU_I2C_MST_CTRL (0x01 | MPU_BANK4)
# define MPU_I2C_MST_DELAY_CTRL (0x02 | MPU_BANK4)
# define MPU_I2C_SLV0_ADDR (0x03 | MPU_BANK4)
# define MPU_I2C_SLV0_REG (0x04 | MPU_BANK4)
# define MPU_I2C_SLV0_CTRL (0x05 | MPU_BANK4)
# define MPU_I2C_SLV0_DO (0x06 | MPU_BANK4)
# define MPU_I2C_SLV1_ADDR (0x07 | MPU_BANK4)
# define MPU_I2C_SLV1_REG (0x08 | MPU_BANK4)
# define MPU_I2C_SLV1_CTRL (0x09 | MPU_BANK4)
# define MPU_I2C_SLV1_DO (0x0A | MPU_BANK4)
# define MPU_I2C_SLV2_ADDR (0x0B | MPU_BANK4)
# define MPU_I2C_SLV2_REG (0x0C | MPU_BANK4)
# define MPU_I2C_SLV2_CTRL (0x0D | MPU_BANK4)
# define MPU_I2C_SLV2_DO (0x0E | MPU_BANK4)
# define MPU_I2C_SLV3_ADDR (0x0F | MPU_BANK4)
# define MPU_I2C_SLV3_REG (0x10 | MPU_BANK4)
# define MPU_I2C_SLV3_CTRL (0x11 | MPU_BANK4)
# define MPU_I2C_SLV3_DO (0x12 | MPU_BANK4)
# define MPU_I2C_SLV4_ADDR (0x13 | MPU_BANK4)
# define MPU_I2C_SLV4_REG (0x14 | MPU_BANK4)
# define MPU_I2C_SLV4_CTRL (0x15 | MPU_BANK4)
# define MPU_I2C_SLV4_DO (0x16 | MPU_BANK4)
# define MPU_I2C_SLV4_DI (0x17 | MPU_BANK4)
#endif
/**\endcond*/
/**
 * MPU9250 Gravity Constant 
 */
#define MPU_GRAVITY 16384

/**
 * Private Structure of MPU9250 driver
 */
struct mpu9250;
/**
 * Accelerator Driver Structure
 */
struct mpu9250_accel {
	/**
	 * Generic Accelerator Structure
	 */
	struct accel_generic gen;
	/**
	 * MPU9250 Instance
	 */
	struct mpu9250 *mpu;
	/**
	 * Basis Values
	 */
	struct vector accelBasis;
};
/**
 * Gyroscope Driver Structure
 */
struct mpu9250_gyro {
	/**
	 * Generic Gyroscope Structure
	 */
	struct gyro_generic gen;
	/**
	 * MPU9250 Instance
	 */
	struct mpu9250 *mpu;
	/**
	 * Basis Values
	 */
	struct vector gyroBasis;
};

/**
 * Private Structure of MPU9250 driver
 */
struct mpu9250 {
	struct hal gen;
	/**
	 * SPI Options
	 */
	const struct spi_opt opt;
	/**
	 * SPI Index ID
	 */
	const uint32_t spi;

	/**
	 * Index of Driver
	 */
	uint32_t index;
	/**
	 * SPI Slave Instance
	 */
	struct spi_slave *slave;
	/**
	 * Accelerator Instance
	 */
	struct mpu9250_accel *accel;
	/**
	 * Gyro Instance
	 */
	struct mpu9250_gyro *gyro;
	/**
	 * last seleced bank only used with ICM20948
	 */
	uint32_t bank;
};
/**
 * Vector
 */
struct mpu9250_vector {
	/**
	 * x Value
	 */ 
	float x;
	/**
	 * y Value
	 */
	float y;
	/**
	 * z Value
	 */
	float z;
};
/**
 * MPU9250 init
 * \param index Driver Index
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return MPU9250 Instance or NULL on error
 */
struct mpu9250 *mpu9250_init(uint32_t index, struct spi *spi, uint8_t cs, uint16_t gpio, TickType_t waittime);
/**
 * Deinit MPU9250 Instance
 * \param mpu MPU9250 Instance
 * \return -1 on error 0 on ok
 */
int32_t mpu9250_deinit(struct mpu9250 *mpu);
/**
 * Reset MPU9250
 * \param mpu MPU9250 Instance
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t mpu9250_reset(struct mpu9250 *mpu, TickType_t waittime);
/**
 * Get actual Accelerator Value
 * \param mpu MPU9250 Instance
 * \param vec Value
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t mpu9250_getAccel(struct mpu9250 *mpu, struct mpu9250_vector *vec, TickType_t waittime);
/**
 * Get actual Gyroscope Value
 * \param mpu MPU9250 Instance
 * \param vec Value
 * \param waittime max waittime in mutex or isr lock see xSemaphoreTake()
 * \return -1 on error 0 on ok
 */
int32_t mpu9250_getGyro(struct mpu9250 *mpu, struct mpu9250_vector *vec, TickType_t waittime);
/**\cond INTERNAL*/
#define ACCEL_PRV
#include <accel_prv.h>
#define GYRO_PRV
#include <gyro_prv.h>
extern const struct gyro_ops mpu9250_gyro_ops;
extern const struct accel_ops mpu9250_accel_ops;
/**\endcond*/
/** 
 * The Number of MPU9250 is Board spezific, the User Code must Call
 * this Macro to define a new MPU9250 dev
 * \warning do not use the created Variable directly! Use the init function!
 * \param id Unique identifier
 * \param spi_id Index of SPI Dev
 * \param cs_id CS Number or SPI_OPT_CS_DIS
 * \param gpio_id GPIO Pin Number or SPI_OPT_GPIO_DIS
 * \param baut Bautrate
 */
#define MPU9250_ADDDEV(id) \
		struct mpu9250 mpu9250_##id; \
		struct mpu9250_accel mpu9250_accel_##id = { \
			ACCEL_INIT_DEV(mpu9250) \
			HAL_NAME("MPU9250 " #id " Accelerator") \
			.mpu = &mpu9250_##id, \
			.accelBasis = {0, 0, 0}, \
		};\
		struct mpu9250_gyro mpu9250_gyro_##id = { \
			GYRO_INIT_DEV(mpu9250) \
			HAL_NAME("MPU9250 " #id " Gyro") \
			.mpu = &mpu9250_##id, \
			.gyroBasis = {0, 0, 0}, \
		};\
		struct mpu9250 mpu9250_##id = { \
			HAL_NAME("MPU9250 " #id) \
			.gen.init = false, \
			.slave = NULL, \
			.accel = &mpu9250_accel_##id, \
			.gyro = &mpu9250_gyro_##id, \
			.bank = 0, \
		}; \
		ACCEL_ADDDEV(mpu9250, mpu9250_accel_##id); \
		GYRO_ADDDEV(mpu9250, mpu9250_gyro_##id); \
		HAL_ADD(mpu9250, mpu9250_##id)
/**
 * Get MPU9250 based on id
 */
#define MPU9250_ID(id) HAL_GET_ID(hal, mpu9250, mpu9250_##id)
/**\}*/
#endif
