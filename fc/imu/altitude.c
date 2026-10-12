/**
 * ==== baro_pow_0p295：由 build-gcc/tools/patch_baro_pow.py 自动注入的声明 ====
 */



#include "bsp.h"
#include "flip.h"

int16_t tiltAngle = 0;		// 飞行器倾斜角度（横滚和俯仰中的较大值）

#define BCALIBRATING_BARO_CYCLES      50		// 气压计校准周期数
  
// Baro => BaroAlt baroVel
#define BARO_TAB_SIZE_MAX                    7 	//21 //15  //11 //21	// 气压历史数据窗口大小
#define PRESSURE_SAMPLES_MEDIAN              5	//3  //5   //7  //3		// 中值滤波器采样点数

uint16_t calibratingB = BCALIBRATING_BARO_CYCLES; 		// 气压计校准计数器
int32_t baroGroundAltitude = 0;		// 地面基准高度（毫米）

int32_t BaroAlt = 0, lastBaroAlt = 0;// 当前相对高度/上次高度（毫米）
int32_t baroVel = 0;	 		// 基于气压计计算的垂直速度（毫米/秒）

// Acc => vel accAlt(EstAlt=accAlt)  & EstAlt
int32_t  accSumZ[3] = {0, 0, 0}; 	// XYZ三轴加速度累计值
uint16_t accSumZCount = 1; 				// 加速度采样计数
float    accSumZTimer = 0;				// 加速度采样时间间隔
int16_t  accZGlobal = 0;					// Z轴全局加速度值
float    vel = 0;									// 融合后的垂直速度（毫米/秒）
float    accAlt = 0.0f;   				// 基于加速度计的高度估计（毫米）
int32_t  EstAlt;                  	// 最终估计高度（毫米）               // in mm
// XYZ三轴速度（毫米/秒）
// XYZ三轴位移（毫米）
float velGlobal[3] = {0.0f, 0.0f, 0.0f}, distanceGlobal[3] = {0.0f, 0.0f, 0.0f};

int32_t AltHold;	// 目标保持高度（毫米）
int32_t BaroPID = 0;	// 高度控制PID输出

// 控制变量
uint8_t altitudeCtrlTimer = 0;	// 高度控制定时器
int16_t setVel = 0;							// 目标垂直速度（毫米/秒）
			
// PID参数（位置环和速度环）
float Pf_Pos = 1.00f;      	// 位置环比例系数
float Pf_Vel = 0.50f;        // 速度环比例系数
float If_Vel = 1.00f;        // 速度环积分系数
float Df_Vel = 0.005f;  		// 速度环微分系数
	
int32_t DispAccAvg,    DispError, DispVolicityPID, DispVel, DispBaroVel, DispSetV;
int32_t DispMatrix0,   DispMatrix1, DispMatrix2, DispAcc0, DispAcc1, DispAcc2;
int32_t DispAngleRoll, DispAnglePitch, DispMidGyroRoll, DispMidGyroPitch, DispMidAccRoll, DispMidAccPitch;
		
int32_t integralBaroThrottle = 0;					
		
void getEstimatedAltitude(void);

// --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------
// 气压计中值滤波器
// 输入：新气压读数
// 输出：滤波后的气压值
// --------------------------------------------------------------------------------------------------------------
double applyBarometerMedianFilter(double newPressureReading)
{
    static double barometerFilterSamples[PRESSURE_SAMPLES_MEDIAN];	 // 采样缓冲区
    static uint16_t currentFilterSampleIndex = 0;											// 当前采样索引
    static bool medianFilterReady = false;														// 滤波器就绪标志
    uint16_t nextSampleIndex;
	
        // 更新环形缓冲区索引
    nextSampleIndex = (currentFilterSampleIndex + 1);
    if (nextSampleIndex == PRESSURE_SAMPLES_MEDIAN) {
        nextSampleIndex = 0;
        medianFilterReady = true;		// 缓冲区填满后标记就绪
    }
		
				// 存储新样本并更新索引
    barometerFilterSamples[currentFilterSampleIndex] = newPressureReading;
    currentFilterSampleIndex = nextSampleIndex;
   
				// 当缓冲区填满时执行中值滤波
    if (medianFilterReady) {
						// 根据配置选择不同大小的中值滤波器
      if(PRESSURE_SAMPLES_MEDIAN == 3) return quickMedianFilter3(barometerFilterSamples);
      else if(PRESSURE_SAMPLES_MEDIAN == 5) return quickMedianFilter5(barometerFilterSamples);
      else if(PRESSURE_SAMPLES_MEDIAN == 7) return quickMedianFilter7(barometerFilterSamples);
      else return newPressureReading;			// 未配置时返回原始值
    }
    else
        return newPressureReading;
}

// --------------------------------------------------------------------------------------------------------------
// 定高(外环PID（比例-积分-微分控制器）)
// --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------
// 气压计高度更新（外环处理）
// 功能：处理原始气压数据，计算高度和垂直速度
// --------------------------------------------------------------------------------------------------------------
void altitude_baro_update(void)
{     
    static double baroHistTab[BARO_TAB_SIZE_MAX];	// 气压历史数据表
    static uint16_t baroHistIdx = 0, indexplus1, bBaroSumReady = 0;	 // 历史数据索引		// 索引辅助变量/求和就绪标志
    static double baroPressureSum = 0, baroGroundPressure = 0;	// 气压求和值		// 地面基准气压
    double baroPressureAvg;		// 平均气压值
    int32_t BaroAlt_tmp;				// 临时高度值
	  uint8_t i, j;								// 循环控制变量
			
		// 当气压计有新数据时执行
		if(baro_update()){ 			
						// ----------------------------------------------------------------------------------------------------
            // 确定采样次数（根据飞行模式调整）
			if(fc.flags.bVelocityControl || (!fc.flags.motorArmed && !fc.flags.motorIdle)) {
				j = BARO_TAB_SIZE_MAX / 1;//3;		 // 速度控制或未解锁时使用完整窗口
			} else {
				j = 1;			// 普通模式每次处理1个样本
			}
						// 更新气压历史数据表
			for(i = 0; i < j; i++) { 	
        indexplus1 = (baroHistIdx + 1);	// 计算下一个索引位置
                // 环形缓冲区处理
        if(indexplus1 == BARO_TAB_SIZE_MAX) { indexplus1 = 0; bBaroSumReady = 1; }	// 标记缓冲区已填满
								// 应用中值滤波并存储到历史表
				baroHistTab[baroHistIdx] = applyBarometerMedianFilter(baroPressure);
								// 更新滑动窗口求和
				baroPressureSum += baroHistTab[baroHistIdx];	// 添加新值
        baroPressureSum -= baroHistTab[indexplus1];	  // 移除最旧值
        baroHistIdx = indexplus1;				// 更新索引
			}
			 			 // 计算平均气压（窗口填满后使用滑动平均值）
		  if(bBaroSumReady) 
			{
					baroPressureAvg = (float)baroPressureSum / (BARO_TAB_SIZE_MAX - 1);	
			}
			else baroPressureAvg = baroPressure;		// 窗口未满时使用单次测量值
						// ----------------------------------------------------------------------------------------------------
			
						// 计算当前高度
						// 气压计校准阶段（起飞前或未解锁状态）
			if ((calibratingB>0) || (!fc.flags.motorArmed && !fc.flags.motorIdle)) {
				if(calibratingB == BCALIBRATING_BARO_CYCLES) baroGroundPressure = baroPressureAvg * 10;		// 初始校准：设置地面基准气压
			   			   // 递归平均更新地面气压
				baroGroundPressure -= baroGroundPressure / 10;
			  baroGroundPressure += baroPressureAvg; 
								// 将气压转换为海拔高度（毫米）
				baroGroundAltitude = (1.0f - baro_pow_0p295((baroGroundPressure / 10.0f) / 101325.0f)) * 44330000.0f; //in mm
// printf("%d \r\n",baroGroundAltitude);
								// 重置速度和高度状态
				vel    = 0;
				accAlt = 0;
								// 递减校准计数器
        if(calibratingB > 0) calibratingB--;					
			}

						// calculates height from ground via baro readings
						// see: https://github.com/diydrones/ardupilot/blob/master/libraries/AP_Baro/AP_Baro.cpp#L140
						// BaroAlt_tmp = (1.0f - powf(((float)baroPressureAvg) / 101325.0f, 0.190295f)) * 4433000.0f; // in cm
						// 计算当前相对高度（基于国际标准大气模型）
				
			BaroAlt_tmp = (1.0f - baro_pow_0p295(((float)baroPressureAvg) / 101325.0f)) * 44330000.0f; // in mm
// printf("%d,%d \r\n",baroPressureAvg,BaroAlt_tmp);
// printf("%d \r\n",BaroAlt_tmp);
			BaroAlt_tmp -= baroGroundAltitude;		// 减去地面基准高度
						// 应用低通滤波器平滑高度数据（α=0.7）
			BaroAlt = (float)BaroAlt * 0.1f + (float)BaroAlt_tmp * (1.0f - 0.1f); // additional LPF to reduce baro noise
// printf("%d \r\n",BaroAlt);
            // 计算速度	mm/s
						// 计算垂直速度（毫米/秒）
            // 方法：一阶低通滤波 + 高度差分放大
			baroVel = baroVel*3.0f/4.0f + (BaroAlt - lastBaroAlt)*8;
						// 限制速度范围（±2米/秒）
			baroVel = constrain(baroVel, -2000, 2000);         



// constrain baro velocity +/- 5000mm/s
      baroVel = applyDeadband(baroVel, 100);                                     // to reduce noise near zero	 10cm/s			去温飘
			
			lastBaroAlt = BaroAlt;// 更新上次高度值
            
						// printf("%d,%d\n", BaroAlt,baroVel);
		}			 
			
}



// --------------------------------------------------------------------------------------------------------------
// 定高(中环PID)
// --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------
// 高度估计处理（中环PID）
// 功能：融合加速度计和气压计数据，执行高度PID控制
// --------------------------------------------------------------------------------------------------------------
void getEstimatedAltitude(void)
{
    static uint32_t previousT;                   // 上次执行时间戳   //Used for update vel & altitude
    uint32_t currentT;  											// 当前时间戳
    float dt;								// 时间间隔（秒）
    float velAcc, accTemp;		// 加速度转换值
	
	 	 // 加速度转速度的缩放因子（mm/s2/LSB（最低有效位））
	  const float accVelScale = 9.80665f * 1000.0f / 4096.0f;        //mm/sec^2
	  uint8_t axis;				// 轴索引（0=X, 1=Y, 2=Z）
	
    int32_t error;	// 当前误差
		static float errorVelocityI = 0; 		 // 速度误差积分项
    static int32_t velErrorLast = 0;			// 上次速度误差
    int32_t baroD;						// 速度误差微分
	  int16_t vel_tmp;					// 临时速度值
	
				// 计算最大倾斜角度（横滚和俯仰中的较大值）
    tiltAngle = mlib_max(mlib_abs(angle[ROLL]), mlib_abs(angle[PITCH])); 
	  			
				// 计算时间间隔
	  currentT = micros16();  		
    dt = (uint16_t)(currentT-previousT)*1e-6;
    previousT = currentT;	
			
        // -------------------------------------------------------------------------------------
        // ------------------------ 加速度积分处理 ---------------------------
				// Integrator - velocity, cm/sec^2
		for(axis = 0; axis < 3; axis++) { 
			if(accSumZCount == 0) accSumZCount = 1;		// 防止除零错误
						// 保存Z轴当前状态（用于积分计算）
			if(axis == 2) { 
				velGlobal[axis]      = vel;
				distanceGlobal[axis] = accAlt;
			}
						// 计算平均加速度
			accTemp              = accSumZ[axis] / accSumZCount;			
						// 将加速度转换为速度变化量
			velAcc               = (float)accTemp * accVelScale * accSumZTimer; 
						// 积分计算位移：s = s0 + v0*t + 0.5*a*t2
			distanceGlobal[axis] += (velAcc * 0.5f) * accSumZTimer + velGlobal[axis] * accSumZTimer;  // integrate velocity to get distance (x= a/2 * t^2)
						// 积分计算速度：v = v0 + a*t
			velGlobal[axis]      += velAcc;
						// 重置加速度累计值
			accSumZ[axis] = 0; 
						// 更新Z轴专用变量
			if(axis == 2) { 
				accAlt   = distanceGlobal[axis];		// 更新高度估计
				vel      = velGlobal[axis]; 				 // 更新速度估计
			}										
		}
				// 重置加速度计数器
		accSumZCount = 0;
		accSumZTimer = 0;
        
// printf("%d,%d,%d,%d,%d,%d\n", (int)velGlobal[0], (int)velGlobal[1], (int)velGlobal[2], (int)distanceGlobal[0], (int)distanceGlobal[1], (int)distanceGlobal[2]);
				
        // Integrator - Altitude in cm
        // accAlt += (vel_acc * 0.5f) * dt + vel * dt;  // integrate velocity to get distance (x= a/2 * t^2)
		 		 // ---------------------- 高度融合与估计 ----------------------
        // 使用互补滤波器融合加速度计和气压计高度
        // 加速度计数据（高频响应好）权重0.965，气压计数据（低频准确）权重0.035
		accAlt = accAlt * 0.985f + (float)BaroAlt * (1.0f - 0.985f); 		// complementary filter for altitude estimation (baro & acc)
	  EstAlt = accAlt;					 // 更新最终高度估计值
				
        // -------------------------------------------------------------------------------------
        // apply Complimentary Filter to keep the calculated velocity based on baro velocity (i.e. near real velocity).
        // By using CF it's possible to correct the drift of integrated accZ (velocity) without loosing the phase, i.e without delay
                // ---------------------- 速度融合与估计 ----------------------
        // 使用互补滤波器融合加速度计和气压计速度
        // 加速度积分速度权重0.987，气压计速度权重0.013
		vel = vel * 0.995f + baroVel * 0.005f;
				// 限制速度范围s
		vel = constrain_float(vel, VELOCITY_SPEEDDOWN_MAX - 200, VELOCITY_SPEEDUP_MAX + 200);	
			        
        // ----------------------------------------------------------------------------
        // 如果马达未解锁或者处于待机状态，所有状态 归零
		if(!fc.flags.motorArmed) {
			fc.flags.bVelocityControl = 0;			// 关闭速度控制
			AltHold = EstAlt;				// 设置目标高度为当前位置
					
			errorVelocityI = 0;			// 重置所有控制状态
			BaroPID = 0;
			vel = 0;		

      velGlobal[0] = 0;			// 重置全局运动状态
      velGlobal[1] = 0;
      velGlobal[2] = 0;
      distanceGlobal[0] = 0;
      distanceGlobal[1] = 0;
      distanceGlobal[2] = 0;
		}				
    
        // ----------------------------------------------------------------------------
		vel_tmp = vel;			// 保存当前速度
    		
	  	  // Part2
				// 仅当倾斜角度<80°时执行高度控制（确保推力方向基本向下）
    if ((tiltAngle < 8000)|| (flip_state != FLIP_IDLE)) { // only calculate pid if the copters thrust is facing downwards(<80deg)
        if (flip_state == FLIP_IDLE) {
                // Altitude P-Controller
			  if(!fc.flags.bVelocityControl) {		//非悬停  位置控制模式
// Pf_Pos = 0.8f;
// Pf_Vel = 1.5f;
// If_Vel = 0.0f;
// Df_Vel = 1.0f;
          Pf_Pos = 1.1f;			 // 位置比例系数
          Pf_Vel = 1.25f;       // 速度比例系数
          If_Vel = 0.00f;       // 速度积分系数
          Df_Vel = 3.0f;  		 // 速度微分系数
// Pf_Pos = 0.9f;
// Pf_Vel = 1.05f;
// If_Vel = 1.5f;
// Df_Vel = 1.0f;
          
          error  = constrain(AltHold - EstAlt,  -5000, 5000);	// 计算高度误差（限制在±5米内）
                    // 位置环P控制：生成目标速度（限制在±3米/秒）
					setVel = constrain(Pf_Pos * error, -5000, +5000); // limit velocity 极限速度to +/- 3 m/s
				} 
		else {	  // 速度控制模式
// if(flip_state != FLIP_IDLE)  Pf_Vel = 2.00f;
// else Pf_Vel = 1.50f;			// 速度比例系数
			      Pf_Vel = 0.90f;
            If_Vel = 0.00f; 
            Df_Vel = 2.0f;	
// Pf_Vel = 1.00f;			// 速度比例系数
// If_Vel = 0.00f;     // 禁用积分项
// Df_Vel = 0.5f;			// 速度微分系数
// Pf_Vel = 0.80f;
// If_Vel = 0.50f;
// Df_Vel = 1.50f;


											      
// setVel = setVelocity;
// 平滑过渡到零速（防止突变）
          if(setVelocity == 0) {                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
						if(setVel > 800)           setVel = 800;
						else if(setVel <-800)      setVel = -800;
						else if(mlib_abs(setVel) < 200) setVel = 0;
						else {							 // 渐进调整
						  if(setVel > setVelocity) {
							  setVel -= 15;
							  if(setVel < setVelocity) setVel = setVelocity;
						  }
						  if(setVel < setVelocity) {
							  setVel += 15;
							  if(setVel > setVelocity) setVel = setVelocity;
						  }
					  }
					} else {
						setVel = setVelocity;			// 直接设置目标速度
					}

          if(setVel == 0 ) { 
            accAlt  = BaroAlt;	// 重置高度估计
						AltHold = accAlt;		// 设置目标高度为当前位置
						EstAlt  = accAlt;

            fc.flags.bVelocityControl = 0;
						
												// 新增：重置速度环积分项和上次误差，避免切换时的积分累积影响
						errorVelocityI = 0;
						velErrorLast = 0;
					}					
				}
				
                // ============= 速度环PID控制器 =============
                // P项：比例控制
                // Velocity PID-Controller
                // P
				#define ALTITUDE_PROPORTION_LIMITED     500  	// 比例项限幅
        error = setVel - vel_tmp;						  
        BaroPID = constrain((Pf_Vel * error), -ALTITUDE_PROPORTION_LIMITED, +ALTITUDE_PROPORTION_LIMITED);

                // I  // I项：积分控制
				#define ALTITUDE_INTEGRAL_LIMITED       400  
				errorVelocityI += (If_Vel * error) * dt;	// 积分项限幅
			  if(errorVelocityI > ALTITUDE_INTEGRAL_LIMITED)  errorVelocityI = ALTITUDE_INTEGRAL_LIMITED;
			  if(errorVelocityI < -ALTITUDE_INTEGRAL_LIMITED) errorVelocityI = -ALTITUDE_INTEGRAL_LIMITED;	
			  BaroPID += errorVelocityI;

                // D
				#define ALTITUDE_DIFFERENTIAL_LIMITED   400  
        baroD = error - velErrorLast;			 // 计算误差变化率
        velErrorLast = error;								// 保存当前误差
          
        BaroPID += constrain(baroD * Df_Vel, -ALTITUDE_DIFFERENTIAL_LIMITED , ALTITUDE_DIFFERENTIAL_LIMITED);	
	}		
// // 空翻状态：不执行常规高度PID（使用Mode_Controler中设置的BaroPID）
    else {
                // 空翻时禁用高度环积分项累积（避免姿态恢复时波动）
        errorVelocityI = 0;
    }

// printf("%d,%d,%d,%d,%d\n", baroVel, vel_tmp, BaroAlt, EstAlt, AltHold);
// printf("%d,%d,%d,%d,%d\n", setVelocity, setVel, BaroAlt, EstAlt, AltHold);
				
    } else {			
		  fc.flags.motorArmed = 0;
			
			fc.flags.bVelocityControl = 0;
			AltHold = EstAlt;
			errorVelocityI = 0;
      BaroPID = 0;
			vel = 0;
			
			error = 0;
    }
							
}
// --------------------------------------------------------------------------------------------------------------
// 高度控制主函数
// 功能：协调高度更新和高度控制处理
// --------------------------------------------------------------------------------------------------------------
// 定高
void altitude_ctrl(void)
{
	altitude_baro_update();	
	
	if (altitudeCtrlTimer >= 20) {
		altitudeCtrlTimer = 0;
		getEstimatedAltitude();				// 执行高度估计和PID控制
	}
}



