#include "vbat_vref.h"
#include "adc.h"
#include "flash.h"

#include "buzzer.h"

// VREFINT（内部参考电压源）：芯片自带的 “标准砝码”
// VREF_REAL（VREFINT 的真实值）：“标准砝码的实际重量”
// VREF（ADC 参考电压）：ADC 测量用的 “尺子”

uint16_t bat_adc=0;

uint16_t battery=4200;	//电池电压-mV
uint8_t bat_vol_flag;

// GetCalibratedBatVoltage
uint16_t vref_real;
// 全局变量：校准表（内存中）和VREF_real
BatCalibPoint bat_calib_table[CALIB_POINT_NUM];  // 校准表

extern Joystick_Pair_Config joystick_pair_str;

uint16_t real_bat_cal_point[CALIB_POINT_NUM] = {3000,3600,4000,4200,5000};
uint16_t real_bat_cal_point_adc[CALIB_POINT_NUM] = {362,395,439,463,554};

// //求内部基准电压的ADC数值的平均值
// uint16_t readVrefIntAverageAdc(void) {
// uint32_t sum = 0;
// for (uint8_t i = 0; i < 10; i++) {  // 采样10次取平均
// sum += aADCxConvertedData[5];  // 读取电池分压节点的ADC值
// HAL_Delay(20);
// }
// return sum / 10;
// }

/* ADC多次读取取平均 */
uint16_t adcReadAverage(uint16_t channelAdc, uint8_t maxSamples) {
    uint32_t sum = 0;
    for (uint8_t i = 0; i < maxSamples; i++) {
        sum += channelAdc;
        HAL_Delay(20);
    }
    return sum / maxSamples;
}

// 内部基准电压校准获取
void getCalibratedVrefIntVoltage(void){
// uint16_t vref_adc_temp = adcReadAverage(aADCxConvertedData[5],bat_sum_max);
// vref_real =  TEST_VCC_MV * vref_adc_temp / 4095 ;
// if(abs(vref_real-1200)<50) vref_real = 1200;
// printf("vref_adc_temp = %d \r\n" ,vref_real);
	joystick_pair_str.vref_real_mv = 1200 ;
}



/* 获取当前VCC电压（mV） */
uint16_t get_vcc(void) {
    uint16_t adc_vref = adcReadAverage(aADCxConvertedData[5],bat_sum_max);
	uint16_t vcc = (4095 * 1200) / adc_vref;
	if(abs(vcc - 3300)<50) vcc = 3300;
        // return ((4095 * vref_real) / adc_vref);
	 return vcc;
}
// 自动校准K值：用户传入实际电池电压，代码自动计算并存储校准表 这里用三个
void autoCalibrateBatK(void) {
	    for (uint8_t i = 0; i < CALIB_POINT_NUM; i++) {
        uint16_t bat_voltage = real_bat_cal_point[i];
        printf("校准点%d/%d - 输入实际电池电压（mV）：", i+1, CALIB_POINT_NUM);
        
				// 计算当前点的K值
        uint16_t vcc = get_vcc();  // 获取当前VCC
			
                // 读取电池ADC值（20次平均）
			uint16_t adc_value = real_bat_cal_point_adc[i];
                // uint16_t adc_value = adcReadAverage(aADCxConvertedData[4], 20);
			printf("vcc:%d\r\n", vcc);


        
        float v_adc = (adc_value / 4095.0f) * vcc;  // 分压后电压
			 printf("adc_value  %.2f\r\n", v_adc);
        float k = bat_voltage / v_adc;  // 计算K值
        printf("计算得到K值：%.3f\r\n", k);

                // 保存到校准表
        bat_calib_table[i].adc = adc_value;
        bat_calib_table[i].k = k;
    }


	
	    	    // 校准表排序（按ADC值从小到大）
    for (uint8_t i = 0; i < CALIB_POINT_NUM-1; i++) {
        for (uint8_t j = i+1; j < CALIB_POINT_NUM; j++) {
            if (bat_calib_table[i].adc > bat_calib_table[j].adc) {
                BatCalibPoint temp = bat_calib_table[i];
                bat_calib_table[i] = bat_calib_table[j];
                bat_calib_table[j] = temp;
            }
        }
    }
	
		// 复制数组内容（核心代码）// 目标地址：结构体中的数组// 源地址：要复制的数组// 复制的字节数
	memcpy(joystick_pair_str.calib_table, bat_calib_table,sizeof(BatCalibPoint) * CALIB_POINT_NUM);  
	    	    // 打印校准结果
    printf("电池校准完成，校准表：\r\n");
    for (uint8_t i = 0; i < CALIB_POINT_NUM; i++) {
        printf("ADC=%d, K=%.3f\r\n", joystick_pair_str.calib_table[i].adc,joystick_pair_str.calib_table[i].k);
    }
}

/* 线性插值获取当前K值 */
float get_segment_k(uint16_t adc) {
        // 边界处理
    if (adc <= bat_calib_table[0].adc) return bat_calib_table[0].k;
    if (adc >= bat_calib_table[CALIB_POINT_NUM-1].adc) return bat_calib_table[CALIB_POINT_NUM-1].k;

        // 查找区间并插值
    for (uint8_t i = 0; i < CALIB_POINT_NUM-1; i++) {
        if (adc >= bat_calib_table[i].adc && adc <= bat_calib_table[i+1].adc) {
            float k1 = bat_calib_table[i].k;
            float k2 = bat_calib_table[i+1].k;
            uint16_t adc1 = bat_calib_table[i].adc;
            uint16_t adc2 = bat_calib_table[i+1].adc;
            return k1 + (k2 - k1) * (adc - adc1) / (adc2 - adc1);
        }
    }
    return bat_calib_table[0].k;  // 异常情况返回默认值
}
uint16_t VREF;

int16_t batADCAverageFilter(uint16_t adcValue,uint8_t  maxSamples){
	static uint32_t bat_sum = 0;  // 用int32_t避免累加溢出
	static uint32_t vref_sum = 0;  // 用int32_t避免累加溢出
    static uint8_t bat_n = 0;
    int16_t filteredResult = -1;  // 未完成时返回-1（可根据需求调整）

        // 累加当前采样值，计数+1
    bat_sum += adcValue;
	vref_sum += aADCxConvertedData[5];
    bat_n++;

        // 当采样次数达到设定的最大值时，计算平均值并重置状态
    if (bat_n >= maxSamples) {
        filteredResult = (int16_t)(bat_sum / maxSamples);  // 取平均
		 VREF = (uint16_t)((4095 * vref_real)/(vref_sum / maxSamples));  // 取平均
		if(abs(TEST_VCC_MV-VREF)<20) VREF=3300;
                // 重置累计和与计数，准备下一轮滤波
        bat_sum = 0;
        bat_n = 0;
		vref_sum = 0;
    }

    return filteredResult;
}



// uint32_t get_battery_voltage(void) {
// uint16_t adc_bat = batADCAverageFilter(aADCxConvertedData[4],bat_sum_max);  // 电池ADC值（滤波）
// float k = get_segment_k(adc_bat);  // 分段K值
// float v_adc = (adc_bat / 4095) * VREF;  // 分压后电压
// return (uint32_t)(v_adc * k);  // 电池电压
// }

void battery_check(void)
{
	int16_t bat_adc_temp;
	int16_t bat_delta;	

	bat_adc_temp = batADCAverageFilter(aADCxConvertedData[4],bat_sum_max);
		// printf("joystick_pair_str.vref_real_mv= %d \r\n",joystick_pair_str.vref_real_mv);
	
	if(bat_adc_temp != -1)
	{
				// printf("bat_adc_temp= %d \r\n",bat_adc_temp);
		bat_delta=abs(bat_adc_temp-(int16_t)bat_adc);   //获取本次与上次数据的差值
		if(bat_delta<10)
			bat_adc=bat_adc*0.7+bat_adc_temp*0.3;
// else if(bat_delta<50)
// bat_adc=bat_adc*0.95+bat_adc_temp*0.05;
		else 
			bat_adc=bat_adc_temp;
				// printf("bat_adc= %d mV\r\n",bat_adc);
				// printf("bat_adc= %d \r\n",bat_adc);
				// printf("bat_adc= %d \r\n",bat_adc);
// battery = bat_adc*(0.38/463)*11000;
		float k = get_segment_k(bat_adc);  // 分段K值
				// printf("k= %f \r\n",k);
		 float v_adc = (bat_adc / 4095.0f) * VREF;  // 分压后电压
				// printf("VREF= %d mV\r\n",VREF);




		
		battery = (uint16_t)(v_adc * k);
		
		if(battery<3700){
			BUZZER_PORT->BSRR = (uint32_t)BUZZER_PIN;
		}
		else
			BUZZER_PORT->BRR = (uint32_t)BUZZER_PIN;
				// printf("v_adc= %f mV\r\n",v_adc);
				// printf("battery= %d mV\r\n",battery);
		
	}
}



