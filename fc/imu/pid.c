
/**
 * 文件：imu/pid.c                          【角色】姿态控制的最后一环：把角度误差变成电机输出
 */

#include "bsp.h"
#include "flip.h"

/* PID 参数实体。pid.h 里用 extern 声明，所以全项目共享这一份。 【为什么参数不在头文件里写死，而要放到这个结构体】 方便运行时调整（例如自稳/定高模式切换时改参数）， 也让"参数"和"算法"分离——调参时只动数值，不动逻辑。 */
pid_conf_s pidconf;

extern Flip_Dirs flip_dir;   /* 翻滚动作状态，来源 user/flip.c */
extern sum_sum sum;          /* 遥控器通道数据（含按键状态），来源 imu/decode.c */
// 光流部分
// float optGyro[2] = {0};			a
// float optGyroLast[2] = {0};
// int32_t opticalflowPID[2] = {0, 0};

/* 这三个是 PID 各分量的调试观察量（定义在别处）， 主要用于串口打印出来看 P/I/D 各自贡献多少，方便调参。 */
extern int32_t p_add,i_add,d_add;



/* @brief  电机混控（motor mixing）：把三个轴的 PID 修正量分配到四个电机 【参数】 @param X  横滚(ROLL)在本电机上的权重，取 -1 或 +1 @param Y  俯仰(PITCH)在本电机上的权重，取 -1 或 +1 */
static inline int PIDMIX(int X, int Y, int Z, int16_t axisPID[]) {
    if(altitude_mode == 0) {
// Led_SetState(LED3, LED_STATE_ON, 0);

        return constrain(1500+BaroPID, 1000, 2000) + axisPID[ROLL]*X + axisPID[PITCH]*Y + axisPID[YAW]*Z;
    } else {
// Led_SetState(LED3, LED_STATE_OFF, 0);
        return constrain(rcCommand[THROTTLE], 1000, 2000) + axisPID[ROLL]*X + axisPID[PITCH]*Y + axisPID[YAW]*Z;
    }
}
extern uint8_t wait_up;
extern uint8_t flip_over;
uint8_t flip_count;

extern uint32_t target_rate_qh;   /* 翻滚：前后轴目标角速度，见 user/flip.c */
extern uint32_t target_rate_zy;   /* 翻滚：左右轴目标角速度，见 user/flip.c */
/* @brief  串级 PID 主函数：姿态误差 → 四个电机输出（每 1ms 调用一次） 【参数】 @param deltaT  距上次调用的时间间隔（单位：秒，实际约 0.001） 来源 imu/fc.c: 用 TIM16 的微秒计数差值算出 */
void pid(float deltaT)
{		
	
  int32_t target_angle, target_rate;	 	
									  
    /* ===== 跨调用保持的状态（static）===== 【关键字 static —— 函数内使用时的含义】 这里的 static 与"文件内可见"完全无关。函数内的 static 表示： 这个变量只在第一次进入函数时初始化，函数返回后依然存在**。 */
  static int32_t last_rate[3] = {0,0,0};  
  int32_t current_rate;   		                                 
  int32_t rate_error;  
  
  static float rate_i[3] = {0.0f, 0.0f, 0.0f}; 	
  static float delta1[3] = {0.0f, 0.0f, 0.0f}, delta2[3] = {0.0f, 0.0f, 0.0f};	  
  float delta, deltaSum;
  volatile int32_t p, i, d;            
  int16_t axisPID[3];	
                                              
  int16_t motor[4], maxMotor; 
  int8_t axis, index;	
			
    /* ===== 积分限幅阈值 ===== 【关键字 const —— 修饰局部变量】 const 表示"只读"：编译期不允许对它们赋值，若误写会直接编译报错。 但它**不保证**放在 Flash 里 —— const 局部变量仍在栈上， */
  const float _imaxYaw   = 228.0f;			//360°	积分限制
  const float _imaxPR    = 350.0f;				
  
    /* ===== PID 参数赋值 ===== 每 1ms 都重新赋一遍同样的常数 —— 看起来多余，实际是编译器友好写法： 这些都是常量，编译器会直接内联成立即数，不产生运行时开销。 好处是调参时只改这一处，且能清楚看到三个轴参数的对比。 */
	pidconf.PAngleToRate[ROLL] = 1.0f;   pidconf.PAngleToRate[PITCH] = 1.0f;   pidconf.PAngleToRate[YAW] = 0.3f;
	pidconf.PRate[ROLL] = 0.10f;         pidconf.PRate[PITCH] = 0.10f;         pidconf.PRate[YAW] = 0.3f;	
	pidconf.IRate[ROLL] = 0.25f;         pidconf.IRate[PITCH] = 0.25f;         pidconf.IRate[YAW] = 0.6f;  
	pidconf.DRate[ROLL] = 0.0012f;       pidconf.DRate[PITCH] = 0.0012f;       pidconf.DRate[YAW] = 0.000f;  
  
        /* 强制偏航角为 0：本项目不做航向锁定（没有磁力计做绝对航向）， 偏航只做角速度控制，所以把偏航角清零避免它参与角度环计算。 */
  angle[2] = 0; 



  for(axis = 0; axis < 3; axis++) {	
        /* ---- 步骤 1：外环（角度环）---- 20 * rcCommand[axis]：遥控器通道值范围是 -500~+500（见 decode.c）， 乘 20 后变成 -10000~+10000， 而 angle[] 单位是"度×100"，即 ±10000 对应 ±100 度。 */
    target_angle = constrain(20*rcCommand[axis], -30000, +30000) + rcCommandTrim[axis] - angle[axis];	
                /* ---- 步骤 2：外环输出"期望角速度" ---- */
		target_rate  = target_angle * pidconf.PAngleToRate[axis];
				/* wait_up：起飞前的等待标志（见 flip.c/main.c）。等待期间不给角速度指令， 避免解锁瞬间电机猛冲。 */
	if(wait_up){
		target_rate = 0;
	}
	  
// if(axis < 2) {
// if(rcCommand[axis] != 0) {
// target_angle = constrain(20*rcCommand[axis], -30000, +30000) + rcCommandTrim[axis] - angle[axis];
// } else {
// target_angle = constrain(20*rcCommand[axis], -30000, +30000) + rcCommandTrim[axis] - angle[axis] ;			//+ opticalflowPID[axis]
// }
// } else {
// target_angle = constrain(20*rcCommand[axis], -30000, +30000) + rcCommandTrim[axis] - angle[axis];
// }



	  if(sum.zyzy || sum.qhqh){
		  target_rate = 0;
	  }
	  
		if(sum.zyzy && axis==0){
			target_rate  = target_rate_zy;  
		}
		else if (sum.qhqh && axis==1){
			target_rate  = target_rate_qh;  
		}
// if(flip_over){
// target_rate = 0;
// flip_count++;
// if(flip_count>=6) {
// flip_count =0;
// flip_over = 0;
// }
// }
					
                /* ---- 步骤 4：内环（角速度环）---- 直接用陀螺仪测到的角速度做反馈，这是内环"快"的来源： 陀螺仪每 1ms 就有新值，比角度环的响应快得多。 */
    current_rate = accgyro.gyroData[axis];  
        
    rate_error = target_rate - current_rate;

                /* ---- 步骤 5：P 项 —— 与误差成正比，决定"反应力度" ---- */
    p = rate_error * pidconf.PRate[axis];
    		
        /* ---- 步骤 6：I 项 —— 误差随时间的累积，用来消除稳态偏差 ---- 乘以 deltaT 是标准做法：让积分量不受调用周期变化影响。 紧接着做限幅（下面 if(axis==2) 那段），防止积分饱和。 */
    rate_i[axis] += (rate_error * pidconf.IRate[axis]) * deltaT;   
	  if(axis == 2){	   			
	    if(rate_i[axis] < -_imaxYaw)     rate_i[axis] = -_imaxYaw;
      else if(rate_i[axis] > _imaxYaw) rate_i[axis] = _imaxYaw;
	  } else {	
			if(rate_i[axis] < -_imaxPR)     rate_i[axis] = -_imaxPR;			  
			else if(rate_i[axis] > _imaxPR) rate_i[axis] = _imaxPR;	
		}    		
    i = rate_i[axis];



    delta = rate_error - last_rate[axis];
    last_rate[axis] = rate_error;
    delta /= deltaT;
    
    deltaSum     = delta1[axis] + delta2[axis] + delta;
    delta2[axis] = delta1[axis];
    delta1[axis] = delta; 
    d = deltaSum * pidconf.DRate[axis];
		
                /* ---- 步骤 8：三轴合成最终修正量 ---- */
    axisPID[axis] = p + i + d;	
  }								 		   			
    
  motor[0] = PIDMIX(-1,-1,-1, axisPID); 
  motor[1] = PIDMIX(-1,+1,+1, axisPID); 
  motor[2] = PIDMIX(+1,-1,+1, axisPID); 
  motor[3] = PIDMIX(+1,+1,-1, axisPID); 

        /* 油门相关常量（电调标准：1000=停转，1100=怠速，2000=满油门） */
  #define MAXTHROTTLE 	2000
  #define MINTHROTTLE		1100
  #define MOTORSTOPVAL	1000
	
		// -------------------------------------------------------------------------------------
        /* ================= 限幅与安全处理 ================= */

    /* 第一步：找出四个电机里的最大值。 为什么找最大值？因为混控是"相对加减"：若某个电机加完修正超过 2000， 不能简单把它截到 2000 —— 那样四个电机的差值就变了，姿态会被带偏。 正确做法是整体下压（下面那行 -=），保持相对差值不变。 */
  maxMotor = motor[0];
  for(axis=1; axis< 4; axis++) if(motor[axis] > maxMotor) maxMotor = motor[axis];
  for(axis = 0; axis < 4; axis++){
	  	  	  	  /* 整体下压：最大值超了 2000，四个电机一起减掉超出量 */
	  if(maxMotor > MAXTHROTTLE) motor[axis] -= maxMotor - MAXTHROTTLE;
    motor[axis] = constrain(motor[axis], MINTHROTTLE, MAXTHROTTLE);   	          



    if(!fc.flags.motorArmed || fc.flags.motorIdle){
      if(fc.flags.motorIdle) {
        motor[axis] = MINTHROTTLE;
      } else {
        motor[axis] = MOTORSTOPVAL;
      }
        	
      for(index=0; index<3; index++){
        rate_i[index] = 0;
        delta1[index] = 0;
        delta2[index] = 0;
        last_rate[index] = 0;
      }							
    }       
  }
// printf("1:%d 2:%d 3:%d 4:%d\r\n",motor[1],motor[0],motor[3],motor[2]);



	motorPWMValue[0] = (uint16_t)motor[1] - MOTORSTOPVAL; 	//0~1000   右上
	motorPWMValue[1] = (uint16_t)motor[0] - MOTORSTOPVAL;     // 左上
	motorPWMValue[2] = (uint16_t)motor[3] - MOTORSTOPVAL;
	motorPWMValue[3] = (uint16_t)motor[2] - MOTORSTOPVAL;		
// printf("1:%d 2:%d 3:%d 4:%d\r\n",motorPWMValue[0],motorPWMValue[1],motorPWMValue[2],motorPWMValue[3]);
}



