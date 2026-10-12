/**
 * @file     bsp_adc_singleconv.c
 * @author   embedfire
 * @version  V1.0
 * @date     2024
 * @brief    adc转换
 */

#include "bsp.h"



ADC_HandleTypeDef             AdcHandle;
/**
 * 通道配置结构体：句柄管“外设整体配置”，通道配置管“采哪个引脚/在规则组里的顺序”
 */
ADC_ChannelConfTypeDef        sConfig;

/**
 * 低电压阈值（mV）。3700mV 是 1S 锂电池的“该降落了”门限（满电 4.2V、放到 3.7V 大约剩 30%）
 */
#define LOW_VOLTAGE_THRESHOLD 3700
// 全局变量
float filtered_voltage = 0.0; // 滤波后的电压值

/* 最新一次测得的电池电压（毫伏）。 为什么要存成全局：imu/rf_telemetry.c 要把它随遥测帧回传给遥控器， 而 get_voltage() 里的 voltage 是局部变量。只做记录，不参与任何判定。 */
float battery_voltage = 0.0f;
/* static：只在本文件可见，避免和别的文件里的同名变量冲突（也省 Flash/RAM 符号表） */

static int low_count=0;
const float alpha = 0.3;      // 滤波系数，根据实际情况调整 (0.05 ~ 0.3)

/* @brief 初始化ADC相关MSP（MSP = MCU Support Package，芯片底层：时钟 + 引脚） 【参数】hadc：ADC 句柄，HAL 用它判断是哪个 ADC 实例 【返回】无 【为什么这么写/调用关系】 */

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (hadc->Instance == ADC1)
  {
    __HAL_RCC_ADC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin =  GPIO_PIN_4;                     /* PA4 = ADC_IN4：电池分压采样脚 */
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;               /* 模拟模式：数字输入缓冲关闭，避免漏电影响精度 */
    GPIO_InitStruct.Pull = GPIO_NOPULL;                    /* 不加上下拉：分压电阻本身就是直流通路，加了下拉会拉偏读数 */
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

/* @brief  初始化 ADC1（12bit、软件触发、连续转换），并校准 【参数】无 【返回】无 【算法/为什么这么写】 */
void ADC_Init(void)
{
    __HAL_RCC_ADC_FORCE_RESET();
    __HAL_RCC_ADC_RELEASE_RESET();                                                 /* 复位ADC（模数转换器） */
    __HAL_RCC_ADC_CLK_ENABLE();                                                    /* ADC时钟使能 */
    
    AdcHandle.Instance = ADC1;
                /* ADC校准 */
                /* 校准必须在外设使能、参数配置之前做：它测量并补偿 ADC 内部的失调误差 */
    HAL_ADCEx_Calibration_Start(&AdcHandle);
    
    AdcHandle.Instance = ADC1;           /* 校准函数可能改动句柄，这里再赋一次，确保 HAL_ADC_Init 拿到正确的实例 */
    AdcHandle.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV1;                      /* 设置ADC时钟 */

    AdcHandle.Init.Resolution = ADC_RESOLUTION_12B;                                /* 转换分辨率12bit */
    AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;                                /* 数据右对齐 */
    AdcHandle.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;                      /* 扫描序列方向：向上 */
    AdcHandle.Init.EOCSelection = ADC_EOC_SINGLE_CONV;                             /* 单次采样 */
    AdcHandle.Init.LowPowerAutoWait = ENABLE;                                      /* 等待转换模式开启 */
    AdcHandle.Init.ContinuousConvMode = ENABLE;                                   /* 单次转换模式 */
                /* 注意：这里使能的是“连续转换模式”，而原作者注释写的是“单次转换模式”， 注释与代码不一致（疑似原作者笔误）。对结果无影响：本文件用轮询方式取值， 连续转换只会让数据寄存器里的值一直是“最新的一次”，未改动代码。 */
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;                                /* 不使能非连续模式 */
    AdcHandle.Init.ExternalTrigConv = ADC_SOFTWARE_START;                          /* 软件触发 */
    AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;           /* 触发边沿无 */
    AdcHandle.Init.DMAContinuousRequests = DISABLE;                                /* 不使能DMA（直接存储器访问） */
    AdcHandle.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;                             /* 当过载发生时，覆盖上一个值 */
    AdcHandle.Init.SamplingTimeCommon=ADC_SAMPLETIME_13CYCLES_5;                   /* 设置采样周期 */
                /* 初始化ADC */
    HAL_ADC_Init(&AdcHandle);
    
    sConfig.Channel = ADC_CHANNEL_4;                                               /* ADC通道选择 */
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;                                        /* 设置加入规则组通道 */
                /* 配置ADC通道 */
    HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
}



// float get_voltage(void)
// {
// float voltage;
// HAL_ADC_PollForConversion(&AdcHandle, 10);
// voltage = (float)(HAL_ADC_GetValue(&AdcHandle)*3288*2/4096);
// if(voltage<3700)
// {
// temp_buff[0]=1;
// fc.flags.bLanding=1;
// //	printf("voltage:%f\r\n",voltage);
// }

// return voltage;
// }

// 在电压采样循环中
void update_battery_voltage() {
    float voltage;
		HAL_ADC_PollForConversion(&AdcHandle, 10); 
		voltage = (float)(HAL_ADC_GetValue(&AdcHandle)*3288*2/4096);; // 读取原始ADC值并转换为电压
    filtered_voltage = (1 - alpha) * filtered_voltage + alpha * voltage;
}

/* @brief  读一次电池电压并做低电保护（轮询方式，约每 1 秒被调用一次） 【参数】无 【返回】无（结果通过 fc.flags / temp_buff / LED 三个出口生效） 【算法】 */
// 在低电压判断中
void get_voltage(void) {
	
		float voltage;
		
		HAL_ADC_PollForConversion(&AdcHandle, 10); 
		voltage = (float)(HAL_ADC_GetValue(&AdcHandle)*3288*2/4096);; // 读取原始ADC值并转换为电压
		battery_voltage = voltage;   				/* 记录给遥测用（见 imu/rf_telemetry.c） */
// if(filtered_voltage==0)
// {
// filtered_voltage = voltage;
// }
// else
// {
// filtered_voltage = (1 - alpha) * filtered_voltage + alpha * voltage;
// }
	
// 使用滤波后的电压，并且持续一段时间才判断为低电量
// if(fc.flags.bLanding==0)
// {

			if (voltage < LOW_VOLTAGE_THRESHOLD) {
					low_count++;      										/* 本次偏低：累计次数，连续偏低才认定真低电 */
			} else {
					low_count = 0;    										/* 一旦回到阈值以上立刻清零：这就是“连续”判定 */
			}
// printf("voltage:%f\r\n",voltage);
			if (low_count > 5) { // 假设循环周期为10ms，则持续3秒
						/* 注意：原作者注释按“循环周期 10ms”估算成 3 秒，但 get_voltage() 实际由 main.c 的 _cnt1s >= 1000 分支调用，周期约 1 秒，所以 6 次 ≈ 6 秒才报警。 结论不变（都是“持续偏低才报警”），注释与调用频率不一致，未改动代码。 */
					fc.flags.bLanding=1;   										/* 交给 imu/fc.c 的飞行状态机 → 自动降落 */
					temp_buff[1] = 2;      										/* 通信回传标记：2 = 低电；见 deal_pack.c 的 temp_buff 用法 */
// Led_SetState(RGB_LED, LED_STATE_BLINKING,  200);
					Led_SetAlert(LED_PRIORITY_LOW_VOLTAGE, LED_STATE_BLINKING, 500);
																				/* 低电压是对外可见性最高的报警：led.c 的 Led_EvaluateState() 里它优先级最高， 会同时把 RGB 灯与 LED4 都变成闪烁（见 led.c 优先级链） */
					Led_SetState(LED4, LED_STATE_BLINKING,  200);
			}
			else
			{
					temp_buff[1] = 1;
// Led_SetAlert(LED_PRIORITY_LOW_VOLTAGE, LED_STATE_OFF, 0);
// Led_SetState(LED4, LED_STATE_ON, 0);
			}
// else
// {
// Led_SetState(RGB_LED, LED_STATE_ON, 0);
// fc.flags.bLanding=0;
// Led_SetState(LED4, LED_STATE_ON, 0);
// }
// }
}



