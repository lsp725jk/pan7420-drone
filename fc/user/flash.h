/**
 * @file    main.h
 * @author  MCU Application Team
 * @brief   Header for main.c file.
 * @attention
 */

/* 一、这个文件在项目里干什么 定义“存在哪里、存什么、怎么算大小”三件事： 存在哪里 —— FLASH_USER_START_ADDR：片内 Flash 的最后一页； 存什么   —— Pair_Code_Config 结构体（对码地址、射频信道、加速度/陀螺零偏、设备 ID）； */
/* Define to prevent recursive inclusion ------------------------------------- */
#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "py32f0xx_hal.h"

/* 用户参数区起始地址：64KB Flash 的最后一页开头（0x08000000 + 0xF000）。 改这个值等于换存储位置，必须同时确认不会和代码段/其它数据重叠 */
#define FLASH_USER_START_ADDR 0x800F000
#define JOYSTICK_PAIR_SIZE sizeof(Pair_Code_Config)   /* 参数结构体占多少字节（编译期由 sizeof 算，自动跟着结构体改） */
#define ARR_Flash_SIZE sizeof(flash_arr)               /* 中转数组占多少字节（flash.c 里的 uint32_t flash_arr[32] ⇒ 128 字节） */

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



/* 用户设置结构体：这就是“掉电保存的全部内容”。 字段含义： pairflag          —— 是否已经写过一次（首次开机为 0xFF，写完置成约定值） paircode[5]       —— 对码得到的 5 字节通信地址（遥控器与飞控必须一致才能通信） */
typedef struct // 用户设置结构体
{
	uint16_t pairflag;//是否第一次写入=2字节16位
	uint8_t paircode[5];
	uint8_t rf_ch;
	int16_t accRaw_offset[3];
  int16_t gyroRaw_offset[3];
	uint8_t device_id;
}Pair_Code_Config;

/* 打印芯片 UID（内容被 #ifdef DEBUG 包住，量产固件不输出）。实现在 flash.c */
void Printf_UID(void);
/* 注意：Get_Pair_Code() 只有声明，flash.c 里没有实现、工程里也没有调用（历史遗留），未改动 */
void Get_Pair_Code(void);
/* 把 pair_data 写进 Flash：先整页擦除再写。对码成功与传感器校准完成后调用 */
void flash_write(uint32_t flash_start_addr);
/* 从 Flash 读回 pair_data：只在开机时调用一次（见 main.c） */
void flash_read(uint32_t flash_start_addr);

#ifdef __cplusplus
}
#endif

#endif 

