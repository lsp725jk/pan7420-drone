#include "joystick.h"
#include "adc.h"
#include "tim.h"
#include "deal_data.h"
#include "flash.h"
#include "led.h"
#include "buzzer.h"
#include "vbat_vref.h"
// AxisRange roll;           // 横滚通道
// AxisRange pitch;          // 俯仰通道
// AxisRange throttle;       // 油门通道
// AxisRange yaw;            // 偏航通道

uint16_t joystick_filter_adc[4];
uint16_t joystick_min[8]={0,0,0,0,0,0,0,0};
uint16_t joystick_mid[8]={2047,2047,2047,2047,2047,2047,2047,2047};
uint16_t joystick_max[8]={4080,4080,4080,4080,4080,4080,4080,4080};
uint16_t hardware_value[8];

uint8_t joystick_deadband[4]={20,20,20,20};

int16_t joystick_ch_offset[4]; //这是两个摇杆的微调
extern uint8_t crsf_send_en;

extern Joystick_Pair_Config joystick_pair_str;



// //	这个函数的主要作用是计算摇杆的中间位置的值，将其映射到172，992，1811

// //TODO 通过一个按键调用这个功能
// void auto_calibrate_joystick_center(void){
// HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
// for (uint8_t i =0 ;i<4;i++){
// roll.center += aADCxConvertedData[0];
// pitch.center += aADCxConvertedData[1];
// throttle.center += aADCxConvertedData[2];
// yaw.center += aADCxConvertedData[3];
// HAL_Delay(30);
// }
// roll.center >>= 2;
// pitch.center >>= 2;
// throttle.center >>= 2;
// yaw.center >>= 2;
// //	roll.center = aADCxConvertedData[0];
// //	pitch.center = aADCxConvertedData[1];
// //	throttle.center = aADCxConvertedData[2];
// //	yaw.center = aADCxConvertedData[3];
// joystick_mid[0] = roll.center;
// joystick_mid[1] = pitch.center;
// joystick_mid[2] = throttle.center;
// joystick_mid[3] = yaw.center;

// //#ifdef DEBUG
// printf("ADC1: %d center1: %d\r\n",joystick_mid[0],roll.center);
// printf("ADC2: %d center2: %d\r\n",joystick_mid[1],pitch.center);
// printf("ADC3: %d center3: %d\r\n",joystick_mid[2],throttle.center);
// printf("ADC4: %d center4: %d\r\n",joystick_mid[3],yaw.center);
// //#endif
// }



// //TODO 未完成
// void calibrate_joystick_max(void){
// HAL_NVIC_DisableIRQ(DMA1_Channel1_IRQn);
// roll.min = 0;
// pitch.min = 0;
// throttle.min = 0;
// yaw.min = 0;
// while(1){
// if(joystick_max[0]<aADCxConvertedData[0])joystick_max[0] = aADCxConvertedData[0];
// if(joystick_max[1]<aADCxConvertedData[1])joystick_max[1] = aADCxConvertedData[1];
// if(joystick_max[2]<aADCxConvertedData[2])joystick_max[2] = aADCxConvertedData[2];
// if(joystick_max[3]<aADCxConvertedData[3])joystick_max[3] = aADCxConvertedData[3];
// #ifdef DEBUG
// printf("ADC1: %d\r\n",joystick_max[0]);
// printf("ADC2: %d\r\n",joystick_max[1]);
// printf("ADC3: %d\r\n",joystick_max[2]);
// printf("ADC4: %d\r\n",joystick_max[3]);
// #endif
// HAL_Delay(200);
// }

// }
void joystick_init(void){
	MX_ADC_Init();
	MX_TIM1_Init();
}

int abs(int v)
{
	if(v>=0)return v;
	else return -v;
}
/* 摇杆adc的值进行互补滤波再映射 */
void joystick_filter_map(void){
	int16_t hardware_input_temp;
	int16_t delta;
	for(uint8_t i=0;i<4;i++)
	{
				// 获取硬件输入原始值
		hardware_input_temp = aADCxConvertedData[i];
		delta=abs(hardware_input_temp-joystick_filter_adc[i]);   //获取本次与上次数据的差值
		if(delta<20)
			joystick_filter_adc[i]=joystick_filter_adc[i]*0.9+hardware_input_temp*0.1;
		else if(delta<50)
			joystick_filter_adc[i]=joystick_filter_adc[i]*0.7+hardware_input_temp*0.3;
// else if(delta<50)
// joystick_filter_adc[i]=joystick_filter_adc[i]*0.5+hardware_input_temp*0.5;
		else if(delta<80)
			joystick_filter_adc[i]=joystick_filter_adc[i]*0.5+hardware_input_temp*0.5;
		else 
			joystick_filter_adc[i]=hardware_input_temp;	
				// 数据限位--防止超出ADC校准范围
// if(joystick_max[i]<=joystick_filter_adc[i]) joystick_filter_adc[i]=joystick_max[i];
// else if(joystick_min[i]>=joystick_filter_adc[i]) joystick_filter_adc[i]=joystick_min[i];
				// 预算硬件输入值，主要是引入adc校准参数
				// mapChValue();
// if(joystick_filter_adc[i]<joystick_mid[i])
// hardware_value[i]=joystick_filter_adc[i]*500/joystick_mid[i]+1000+joystick_ch_offset[i];
// //		else if (joystick_filter_adc[i] == joystick_mid[i]) hardware_value[i] = 1500;
// else  hardware_value[i]=(joystick_filter_adc[i]-joystick_mid[i])*500/(4095-joystick_mid[i])+1500+joystick_ch_offset[i];
		if(crsf_send_en == 0){
			hardware_value[0] = mapChValue(joystick_filter_adc[0],0,joystick_mid[0],joystick_max[0],0,-joystick_ch_offset[0],10);
			hardware_value[1] = mapChValue(joystick_filter_adc[1],0,joystick_mid[1],joystick_max[1],1,joystick_ch_offset[1],10);
// hardware_value[0] = mapChValue(joystick_filter_adc[0],0,joystick_mid[0],joystick_max[0],0,0,10);
// hardware_value[1] = mapChValue(joystick_filter_adc[1],0,joystick_mid[1],joystick_max[1],1,0,10);
			hardware_value[2] = mapChValue(joystick_filter_adc[2],0,joystick_mid[2],joystick_max[2],1,0,10);
			hardware_value[3] = mapChValue(joystick_filter_adc[3],0,joystick_mid[3],joystick_max[3],0,0,10);
		}
		else{
			hardware_value[0] = mapChValue(joystick_filter_adc[0],0,joystick_mid[0],joystick_max[0],0,0,10);
			hardware_value[1] = mapChValue(joystick_filter_adc[1],0,joystick_mid[1],joystick_max[1],1,0,10);
			hardware_value[2] = mapChValue(joystick_filter_adc[2],0,joystick_mid[2],joystick_max[2],1,0,10);
			hardware_value[3] = mapChValue(joystick_filter_adc[3],0,joystick_mid[3],joystick_max[3],0,0,10);
		}

	}
		// printf("ADC1: %d ADC2: %d ADC3: %d ADC4: %d\r\n", hardware_value[0],hardware_value[1],hardware_value[2],hardware_value[3]);
	#ifdef DEBUG
	printf("ADC1: %d ADC2: %d ADC3: %d ADC4: %d\r\n", hardware_value[0],hardware_value[1],hardware_value[2],hardware_value[3]);
	#endif
}

/* @brief 校准各轴的最大最小值 */
void calibrateExtremes(void) {
// uint32_t startTime;
    uint16_t currentValue;
    uint8_t allAxesValid = 0; // 所有轴是否满足行程要求
    uint8_t retryCount = 0;    // 重试计数器，避免无限循环
    
        // 最多重试3次，仍失败则提示硬件问题
    while(!allAxesValid && retryCount < 3) {
                // 初始化/重置极值
        // 初始化校准数据
    for(uint8_t i = 0; i < 4; i++) {
        joystick_pair_str.chMin[i] = 4095;  // 初始化为最大值
        joystick_pair_str.chUpper[i] = 0;     // 初始化为最小值
    }  

                // 提示用户开始摇杆绕圈（可通过LED闪烁：快闪3次表示开始采样）
// indicateCalibrationStage(1, false); // 自定义函数：阶段1，未完成
// startTime = HAL_GetTick();
        
        // 持续采样5秒，确保用户有足够时间完成摇杆绕圈
	 while(1) {
      // while(HAL_GetTick() - startTime < 5000) {
        for(uint8_t i = 0; i < 4; i++) {
            currentValue = aADCxConvertedData[i];
            
                        // 更新最小值
            if(currentValue < joystick_pair_str.chMin[i]) {
                joystick_pair_str.chMin[i] = currentValue;
            }
            
                        // 更新最大值
            if(currentValue > joystick_pair_str.chUpper[i]) {
                joystick_pair_str.chUpper[i] = currentValue;
            }
        }
        HAL_Delay(10); // 100Hz采样率
		
		if(is_pair_key()){
			while(is_pair_key()){};
				break;
		}
    }
        
                // 校验各轴行程是否有效
        allAxesValid = 1; // 先假设所有轴有效
        for(uint8_t i = 0; i < 4; i++) {
                        // 行程阈值设为3000（根据硬件调整）
           if(joystick_pair_str.chUpper[i] - joystick_pair_str.chMin[i] < 4000) {
                allAxesValid = 0; // 标记为无效
                retryCount++;
                
// // 提示对应轴行程不足（例如：第i轴对应LED闪烁）
// indicateAxisError(i);
// delayMs(2000); // 等待2秒让用户看到提示

                break; // 跳出当前轴检查，准备重试
            }
        }
    }
    
        // 最终校验结果处理
    if(!allAxesValid) {
                // 多次重试失败，提示硬件故障（例如：所有LED常亮）
// indicateCalibrationError();
		BUZZER_PORT->BSRR = (uint32_t)BUZZER_PIN;
        while(1); // 终止校准，等待人工干预
    }
    Led_SetState(LED2, LED_STATE_ON, 0);
// // 提示极值校准完成（例如：LED长亮1秒）
// indicateCalibrationStage(1, true);
// delayMs(1000);
}

/* @brief 校准中间值和死区 */
void calibrateMidAndDeadband(void) {
    uint32_t startTime = HAL_GetTick();
    uint16_t samples[4][50]; // 存储采样值
    uint8_t sampleCount = 0;
    
        // 提示用户将摇杆归位到中间位置
        // 例如:点亮另一个LED表示进入中间值校准阶段
    
        // 采集50个样本(约1秒)
    while(sampleCount < 50) {
        for(uint8_t i = 0; i < 4; i++) {
            samples[i][sampleCount] = aADCxConvertedData[i];
        }
        sampleCount++;
        HAL_Delay(20); // 50Hz采样率
    }
    
        // 计算中间值(平均值)
    for(uint8_t i = 0; i < 4; i++) {
        uint32_t sum = 0;
        for(uint8_t j = 0; j < 50; j++) {
            sum += samples[i][j];
        }
        joystick_pair_str.chMiddle[i] = sum / 50;
        
                // 计算死区(基于采样值的波动)
        uint16_t maxVal = samples[i][0];
        uint16_t minVal = samples[i][0];
        for(uint8_t j = 1; j < 50; j++) {
            if(samples[i][j] > maxVal) maxVal = samples[i][j];
            if(samples[i][j] < minVal) minVal = samples[i][j];
        }
        
                // 死区设为波动范围的1.5倍，确保覆盖所有抖动
        uint8_t deadband = maxVal - minVal;
        joystick_pair_str.deadband[i] = (uint8_t)(deadband * 1.5f);
        
                // 死区最小为5，防止过小导致误触发
        if(joystick_pair_str.deadband[i] < 5) {
            joystick_pair_str.deadband[i] = 5;
        }
		printf("deadband = %d------\r\n",deadband);
    }
// Led_SetState(LED2, LED_STATE_ON, 0);
// HAL_Delay(1000);
        // 提示用户校准完成，按确认键保存
}
/* @brief 开始校准流程 */
void startCalibration(void) {
	Led_SetState(LED2, LED_STATE_ON, 0);
        // 阶段1: 校准极值(Min/Max)
	while(!is_pair_key()){
	}
	 Led_SetState(LED2, LED_STATE_OFF, 0);
	printf("开始 = %d\r\n",joystick_pair_str.calibrated );
    calibrateExtremes();
    
// // 等待用户确认
// while(!is_pair_key()) {
// // 可以添加指示LED闪烁等提示
// //		Led_SetState(LED2, LED_STATE_OFF, 0);

// }


	
// while(!is_pair_key()){}; //这个打开校准效果就变成，还要再按下key5才会进行中间值校准
// Led_SetState(LED2, LED_STATE_OFF, 0);//这个打开校准效果就变成，还要再按下key5才会进行中间值校准
        // 阶段2: 校准中间值和死区
	printf("开始 2= %d\r\n",joystick_pair_str.calibrated );
    calibrateMidAndDeadband();
	
		// 这个是校准内部基准参考电压的
    getCalibratedVrefIntVoltage();
	
	autoCalibrateBatK();
        // 标记校准完成
    joystick_pair_str.calibrated = 1;
    
// // 计算校验值
// joystick_pair_str.crc = calculateCRC((uint8_t*)&calData, sizeof(CalibrationData) - sizeof(uint16_t));
    
        // 保存到Flash
	flash_write(FLASH_USER_START_ADDR);
    
        // 校准完成，可以重启或进入正常模式
}



// #define SLIDING_WINDOW_SIZE 5  // 滑动窗口大小

// // 滑动滤波结构体（增加count字段）
// typedef struct {
// uint16_t window[SLIDING_WINDOW_SIZE];
// uint8_t index;
// uint8_t count;  // 有效数据计数
// uint32_t sum;
// } SlidingFilter;

// // 初始化滑动滤波器（增加count初始化）
// void sliding_filter_init(SlidingFilter *filter) {
// for (int i = 0; i < SLIDING_WINDOW_SIZE; i++) {
// filter->window[i] = 0;
// }
// filter->index = 0;
// filter->count = 0;  // 初始有效数据为0
// filter->sum = 0;
// }

// // 修正后的滑动滤波处理函数
// uint16_t sliding_filter_process(SlidingFilter *filter, uint16_t value) {
// // 窗口未满时：直接累加，不减去旧值
// if (filter->count < SLIDING_WINDOW_SIZE) {
// filter->window[filter->index] = value;
// filter->sum += value;
// filter->count++;  // 有效数据增加
// }
// // 窗口已满时：移除最早数据，添加新数据
// else {
// filter->sum -= filter->window[filter->index];
// filter->window[filter->index] = value;
// filter->sum += value;
// }

// // 更新索引
// filter->index = (filter->index + 1) % SLIDING_WINDOW_SIZE;

// // 使用有效数据数量计算平均值
// return filter->sum / filter->count;
// }

// void battery_check() {
// // 读取原始ADC值
// uint16_t adc_raw = aADCxConvertedData[4];

// // 第一级：滑动滤波处理
// uint16_t adc_sliding = sliding_filter_process(&bat_sliding_filter, adc_raw);

// // 第二级：EWMA滤波处理
// bat_adc = ewma_filter_process(&bat_ewma_filter, adc_sliding);

// // 计算电池电压
// battery = bat_adc * bat_vol / 100.0;
// }



// #define SLIDING_WINDOW_SIZE 5  // 滑动窗口大小
// #define EWMA_ALPHA_LOW   0.02  // 小变化时的EWMA系数
// #define EWMA_ALPHA_HIGH  0.10  // 大变化时的EWMA系数

// // EWMA滤波结构体
// typedef struct {
// uint16_t last_value;  // 上一次滤波值
// uint16_t threshold;   // 判定变化大小的阈值
// } EwmaFilter;



// // 初始化EWMA滤波器
// void ewma_filter_init(EwmaFilter *filter, uint16_t initial_value, uint16_t threshold) {
// filter->last_value = initial_value;
// filter->threshold = threshold;
// }

// // EWMA滤波处理（带自适应系数）
// uint16_t ewma_filter_process(EwmaFilter *filter, uint16_t value) {
// uint16_t delta = (value > filter->last_value) ?
// (value - filter->last_value) :
// (filter->last_value - value);

// // 根据变化大小选择不同的α系数
// float alpha = (delta > filter->threshold) ?
// EWMA_ALPHA_HIGH : EWMA_ALPHA_LOW;

// // 执行EWMA计算
// filter->last_value = (uint16_t)(filter->last_value * (1.0 - alpha) + value * alpha);
// return filter->last_value;
// }
