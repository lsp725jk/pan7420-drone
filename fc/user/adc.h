/**
 * 文件：user/adc.h                        【角色】电池电压采集模块的对外声明
 */
#ifndef __ADC_H
#define __ADC_H

/**
 * 包含其他头文件
 */
#include "py32f0xx_hal.h"



/**
 * 全局变量声明
 */
/**
 * ADC 句柄：定义在 adc.c，配置信息（分辨率、采样时间等）都挂在这里
 */
extern ADC_HandleTypeDef             AdcHandle;
/* ADC 采样值数组：定义在 adc.c。本工程只用 [0]（电池电压通道），[1][2] 未使用 */
extern uint32_t adc_value[3];



/* 函数声明 */
/* 初始化 ADC1 + 通道 4（PA4），并做一次校准；实现见 adc.c */
void ADC_Init(void);

/* 读一次电池电压，判断是否低电并联动 LED/自动降落；实现见 adc.c */
void get_voltage(void);



#endif /* __BSP_ADC_SINGLECONV_POLLING_H */

