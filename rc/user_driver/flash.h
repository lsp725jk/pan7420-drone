/**
 * @file    main.h
 * @author  MCU Application Team
 * @brief   Header for main.c file.
 * @attention
 */

/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "py32f0xx_hal.h"
#include "vbat_vref.h"

#define FLASH_USER_START_ADDR 0x800F000
#define JOYSTICK_PAIR_SIZE sizeof(Joystick_Pair_Config)
#define ARR_Flash_SIZE sizeof(flash_arr)

// 定义UID结构体
typedef struct {
// char lotNumber[8];      // 批次号，7个ASCII字符+终止符
	uint8_t lotNumber[7];
    uint8_t waferNumber;    // 晶圆编号
    uint8_t internalCode1;  // 内部编码1
    uint16_t xCoordinate;   // X坐标
    uint16_t yCoordinate;   // Y坐标
    uint8_t fixedCode;      // 固定码(0x78)
	uint8_t internalCode2;  // 内部编码2
    uint8_t internalCode3;  // 内部编码3
    uint8_t internalCode4;  // 内部编码4
	uint8_t myuin;
} PY32F030_UID_TypeDef;

// typedef struct // 用户设置结构体
// {
// uint8_t write_pairflag;//是否第一次写入=2字节16位
// uint8_t paircode[5];
// uint16_t rf_ch;

// }Pair_Code_Config;

typedef struct // 用户设置结构体
{
	uint16_t write_count;//是否第一次写入=2字节16位
	uint8_t paircode[5];
	uint8_t rf_ch;
	uint8_t device_id;
	int16_t adjustValue[4];//微调PWM占空比=2字节*chNum
	uint16_t chMin[4];//遥杆的最小值
	uint16_t chMiddle[4];//遥杆的中值
	uint16_t chUpper[4];//遥杆的最大值
	uint8_t deadband[4];// 各轴死区
    uint16_t calibrated;        // 校准完成标志(0:未校准 1:已校准)
    uint16_t vref_real_mv;       //内部真实的基准电压
	
	BatCalibPoint calib_table[CALIB_POINT_NUM];
	
		// uint16_t chReverse[chNum];//通道的正反，1为正常，0为反转
		// uint16_t throttlePreference;//左右手油门，1为左手油门
	
}Joystick_Pair_Config;

void Printf_UID(void);
void Get_Pair_Code(void);
void flash_write(uint32_t flash_start_addr);
void flash_read(uint32_t flash_start_addr);
void Set_Joystick_Data(void);
#ifdef __cplusplus
}
#endif

#endif 

