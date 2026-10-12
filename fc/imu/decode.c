#include "bsp.h"

#define VEL_CON_STICK_LIMIT_UP    1550				//控制无人机上升下降速度，油门在中间范围无人机悬停
#define VEL_CON_STICK_LIMIT_DOWN  1450

uint8_t calibrationEnbTimer;		//校准时间

uint8_t temp_buff[__PN298_PAYLOAD];

uint16_t fcCmd[FC_CMD_LENGTH];
int16_t rcCommand[4] = {0}, rcCommandTrim[3] = {0};

int16_t setVelocity = 0;



// uint8_t printTestTimer;
float speed_level;

void annexCode(void)
{
  int16_t codeRoll, codePitch, codeGas, codeYaw;
// int16_t codeRollTrim, codePitchTrim;
  static uint8_t armCheckEnb = 0;

	if(fc.flags.uartCmdNew) 
	{	 	
		fc.flags.uartCmdNew= 0;	
												
				// 指令解析
				// 调整后的指令方向:
				// 1)左右 CodeRoll[rcCommand-Roll],  左小右大
				// 2)前后 CodePitch[rcCommand-Pitch],后小前大
		codeGas 	= fcCmd[FC_CMD_THROTTLE];			//油门
		codeRoll	= fcCmd[FC_CMD_ROLL];					//横滚
		codePitch = fcCmd[FC_CMD_PITCH];				//俯仰
		codeYaw 	= fcCmd[FC_CMD_YAW];					//偏航
// printf("THROTTLE:%d ROLL:%d ,PITCH:%d ,YAW:%d \r\n",codeGas,codeRoll,codePitch,codeYaw);
		
// codeRollTrim = fcCmd[FC_CMD_ROLL_TRIM];
// codePitchTrim = fcCmd[FC_CMD_PITCH_TRIM];
    
// codeGas   = constrain(codeGas*4, 0, 1000);	//扩大4倍，映射到0~1000
// codeRoll  -= STICK_ENCODE_LIMIT_MIDDLE;		//减去128，得出移动量
// codePitch -= STICK_ENCODE_LIMIT_MIDDLE;
// codeYaw   -= STICK_ENCODE_LIMIT_MIDDLE;
// codeRollTrim  -= STICK_TRIM_LIMIT_RANGE;
// codePitchTrim -= STICK_TRIM_LIMIT_RANGE;

// codeRoll = applyDeadband(codeRoll, 28);	//100~156为中间值
// codePitch = applyDeadband(codePitch, 28);
// codeYaw = applyDeadband(codeYaw, 37); //91~165为中间值




    
    if((codeGas > VEL_CON_STICK_LIMIT_UP)) {
      fc.flags.bVelocityControl = 1;	
      setVelocity = (codeGas - VEL_CON_STICK_LIMIT_UP) * 4; //4
      if(setVelocity > VELOCITY_SPEEDUP_MAX) setVelocity = VELOCITY_SPEEDUP_MAX;	
      fc.flags.motorIdle = 0;
			fc.flags.bTakeoff = 0;
			fc.flags.bLanding = 0;
    } else if(codeGas < VEL_CON_STICK_LIMIT_DOWN) {
      fc.flags.bVelocityControl = 1;						
      setVelocity = (codeGas - VEL_CON_STICK_LIMIT_DOWN) * 1;	//3
			if(setVelocity < VELOCITY_SPEEDDOWN_MAX) setVelocity = VELOCITY_SPEEDDOWN_MAX;	 
			fc.flags.bTakeoff = 0;
			fc.flags.bLanding = 0;   
// if(fc.flags.motorIdle) {
// fc.flags.motorArmed = 0;
// fc.flags.motorIdle = 0;
// }

    } else {	
      setVelocity = 0;
    }  
    
    if((fc.flags.motorArmed == 0) && (fc.flags.motorIdle == 0))
		{	
				if((codeGas<=1050) && (codeYaw>=450) && (codePitch <= -450) && (codeRoll <= -450))
				{
						accgyro.gyroCalibrating = 1;		
						accgyro.accCalibrating = 1;   
				}
		}
        // 解锁按键按下
    if(fcCmd[FC_CMD_FLAGS_1]) { 
				if(armCheckEnb) {
						armCheckEnb = 0;
						
						if(fc.flags.bLanding) {
								fc.flags.motorArmed = 0;
								fc.flags.motorIdle = 0;
								fc.flags.bLanding = 0;
						} else {                
								if(!fc.flags.motorArmed) {
										fc.flags.motorArmed = 1;
										fc.flags.motorIdle = 1;
										fc.flags.bAccGyroCalibrationEnb = 0;
										
										fc.flags.bTakeoff = 1;
										fc.takeofflandTimer = 0;
										fc.flags.bLanding = 0;
										fc.takeoffAlt = EstAlt;
								} 
								else if(!fc.flags.motorIdle){		//空转
										fc.flags.bLanding = 1;
										fc.takeofflandTimer = 0;       
										fc.flags.bTakeoff = 0;
								}
						} 
				}
    } else {
      armCheckEnb = 1;
    }    
        // 急停
    if(fcCmd[FC_CMD_FLAGS_1]==0) {
      fc.flags.motorArmed = 0;
      fc.flags.motorIdle = 0;
    }
// printf("time:%d\r\n",fc.takeofflandTimer);
								// 一键降落
		if(fc.flags.bLanding) {  
				
				fc.flags.bVelocityControl = 1;	

				setVelocity = -650; 		// - fc.takeofflandTimer*10
				if(setVelocity < VELOCITY_SPEEDDOWN_MAX) setVelocity = VELOCITY_SPEEDDOWN_MAX;
				if(accgyro.accData[2]>6500)
				{
						  fc.flags.motorArmed = 0;
							fc.flags.motorIdle = 0;
							fc.flags.bLanding=0;
				}
// temp_buff[3] = (uint8_t)((accSmooth[2] >> 24) & 0xFF);  // 提取高8位
// temp_buff[2] = (uint8_t)((accSmooth[2] >> 16) & 0xFF);  // 提取高8位
// temp_buff[1] = (uint8_t)((accSmooth[2] >> 8) & 0xFF);  // 提取高8位
// temp_buff[0] = (uint8_t)(accSmooth[2] & 0xFF);         // 提取低8位
		}
			
    rcCommand[THROTTLE]   = constrain((codeGas), +1000, +2000);	      //1000 ~ 2000
// rcCommand[THROTTLE]=1800;
		rcCommand[ROLL]       = constrain(codeRoll,   -500, +500)*speed_level;	  						//-500 ~ +500
		rcCommand[PITCH]      = constrain(codePitch,  -500, +500)*speed_level;
		rcCommand[YAW]        = -constrain(codeYaw, -500, +500);
// printf("throttle:%d\r\n",rcCommand[YAW]);
// rcCommandTrim[ROLL]   = codeRollTrim * 10;  		//微调值
// rcCommandTrim[PITCH]  = codePitchTrim * 10;   	//微调值
// rcCommandTrim[YAW]    = 0;
		
	} 



}



